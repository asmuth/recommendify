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

private

  def all_pairs(keys)
  	keys.map{ |k1| (keys-[k1]).map{ |k2| [k1,k2].sort.join(":") } }.flatten.uniq
  end

  def item_count_incr(key)
  	Recommendify.redis.hincrby(redis_key(:items), key, 1)
  end

end