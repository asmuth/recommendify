class Recommendify::JaccardInputMatrix < Recommendify::InputMatrix
  def initialize(opts={})
    super(opts)
  end

  def related_items(item_id)
      sets = Recommendify.redis.smembers(redis_key(:items, item_id))
      keys = sets.map { |set| redis_key(:sets, set) }
      if keys.length() > 0
        Recommendify.redis.sunion(keys) - [item_id]
      else
        []
      end
  end

  def similarity(item1, item2)
    Recommendify.redis.zscore(redis_key(:similarities, item1), item2)
  end

  def similarities_for(item1)
    Recommendify.redis.zrevrange(redis_key(:similarities, item1), 0, -1, :with_scores => true)
  end

  def process_item!(item)
    cache_similarities_for(item)
  end

  def process!()
    all_items.each do |item|
      process_item!(item)
    end
  end

  def delete_item(item_id)
    Recommendify.redis.watch(redis_key(:items, item_id), redis_key(:similarities, item_id)) do
      sets = Recommendify.redis.smembers(redis_key(:items, item_id))
      items = Recommendify.redis.zrange(redis_key(:similarities, item_id), 0, -1)
      Recommendify.redis.multi do |multi|
        sets.each do |set|
          multi.srem(redis_key(:sets, set), item_id)
        end

        items.each do |item|
          multi.srem(redis_key(:similarities, item), item_id)
        end

        multi.del redis_key(:items, item_id), redis_key(:similarities, item_id)
      end
    end
  end

  private

  def cache_similarity(item1, item2)
    score = calculate_jaccard(item1, item2)

    if score > 0
      Recommendify.redis.multi do |multi|
        multi.zadd(redis_key(:similarities, item1), score, item2)
        multi.zadd(redis_key(:similarities, item2), score, item1)
      end
    end
  end

  def cache_similarities_for(item)
    related_items(item).each do |i|
      cache_similarity(item, i)
    end
  end

  def calculate_jaccard(item1, item2)
    x = nil
    y = nil
    Recommendify.redis.multi do |multi|
      x = multi.sinterstore 'temp', [redis_key(:items, item1), redis_key(:items, item2)]
      y = multi.sunionstore 'temp', [redis_key(:items, item1), redis_key(:items, item2)]
    end

    if y.value > 0
      return x.value.to_f/y.value.to_f*self.weight
    else
      return 0.0
    end
  end
end
