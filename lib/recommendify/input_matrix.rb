class Recommendify::InputMatrix

  def self.create(opts)
    klass = "#{Recommendify.capitalize(opts[:similarity_func])}InputMatrix"
    Recommendify.constantize(klass.intern).new(opts)
  end

  def initialize(opts)
    @opts = opts
  end

  def redis_key(*append)
    ([@opts.fetch(:redis_prefix), @opts.fetch(:key)] + append).flatten.compact.join(":")
  end

  def weight
    (@opts[:weight] || 1).to_f
  end

  def add_set(set_id, item_ids)
    Recommendify.redis.multi do
      item_ids.each { |item| add_single_nomulti(set_id, item) }
    end
  end

  def add_single(set_id, item_id)
    Recommendify.redis.multi do
      add_single_nomulti(set_id, item_id)
    end
  end

  def all_items
    Recommendify.redis.smembers(redis_key(:all_items))
  end

  def items_for(set)
    Recommendify.redis.smembers redis_key(:sets, set)
  end

  def sets_for(item)
    Recommendify.redis.sunion redis_key(:items, item)
  end

  # calculate all similarities to other items in the matrix for item1
  def similarities_for(item1)
    # return => [ ["item23", 0.6], ["item42", 0.23], (...) ]
    raise "implemented in subclass"
  end

  # delete item_id from the matrix
  def delete_item(item_id)
    raise "implemented in subclass"
  end

  private

  def add_single_nomulti(set_id, item_id)
    Recommendify.redis.sadd(redis_key(:all_items), item_id)
    Recommendify.redis.sadd(redis_key(:sets, set_id), item_id)
    # add the set_id to the item_id's set--inverting the sets
    Recommendify.redis.sadd(redis_key(:items, item_id), set_id)
  end
end