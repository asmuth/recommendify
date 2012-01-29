class Recommendify::JaccardProcessor

  attr_reader :ccmatrix

  def initialize(opts={})
    @opts = opts
    @ccmatrix = Recommendify::SparseMatrix.new(
      :redis_prefix => @opts.fetch(:redis_prefix),
      :key => [@opts.fetch(:key), :ccmatrix].join(":")
    )
  end

  def redis_key(append=nil)
    [@opts.fetch(:redis_prefix), @opts.fetch(:key), append].flatten.compact.join(":")
  end

  def add_set(set_id, item_ids)
    item_ids.each do |item_id|
      item_count_incr(item_id)
    end
    all_pairs(item_ids).map do |pair| 
      i1, i2 = pair.split(":") 
      @ccmatrix.incr(i1, i2)
    end
  end

  def similarity(item1, item2)
    calculate_jaccard_cached(item1, item2)
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

  def calculate_jaccard_cached(item1, item2)
    val = @ccmatrix[item1, item2]
    val.to_f / (item_count(item1)+item_count(item2)-val).to_f
  end

  def calculate_jaccard(set1, set2)
    (set1&set2).length.to_f / (set1 + set2).uniq.length.to_f
  end

end