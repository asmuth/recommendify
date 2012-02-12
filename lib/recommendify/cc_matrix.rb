module Recommendify::CCMatrix

  def ccmatrix
    @ccmatrix ||= Recommendify::SparseMatrix.new(
      :redis_prefix => @opts.fetch(:redis_prefix),
      :key => [@opts.fetch(:key), :ccmatrix].join(":")
    )
  end

  def add_set(set_id, item_ids)
    item_ids.each do |item_id|
      item_count_incr(item_id)
    end
    all_pairs(item_ids).map do |pair| 
      i1, i2 = pair.split(":") 
      ccmatrix.incr(i1, i2)
    end
  end

  def add_single(set_id, item_id, other_item_ids)
  # todo: add single item to set after set was added (incrementally)
  #   item_count_incr(item_id)
  #   other_item_ids.each do |other_item|
  #     @ccmatrix.incr(item_id, other_idem)
  #   end
  end

  def all_items
    Recommendify.redis.hkeys(redis_key(:items))
  end

  def delete_item(item_id)    
    Recommendify.redis.hdel(redis_key(:items), item_id)
    @ccmatrix.send(:k_delall, item_id)
  end

private

  def all_pairs(keys)
    keys.map{ |k1| (keys-[k1]).map{ |k2| [k1,k2].sort.join(":") } }.flatten.uniq
  end

  def item_count_incr(key)
    Recommendify.redis.hincrby(redis_key(:items), key, 1)
  end

  def item_count(key)
    Recommendify.redis.hget(redis_key(:items), key).to_i
  end

end