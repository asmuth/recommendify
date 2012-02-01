class Recommendify::SimilarityMatrix

  attr_reader :write_queue

  def initialize(opts={})
    @opts = opts
    @write_queue = Hash.new{ |h,k| h[k] = {} }
  end

  def redis_key(append=nil)
    [@opts.fetch(:redis_prefix), @opts.fetch(:key), append].flatten.compact.join(":")
  end

  def max_neighbors
    @opts[:max_neighbors] || Recommendify::DEFAULT_MAX_NEIGHBORS
  end

  def update(item_id, neighbors)
    neighbors.each do |neighbor_id, score|
      if @write_queue[item_id].has_key?(neighbor_id)
        @write_queue[item_id][neighbor_id] += score
      else
        @write_queue[item_id][neighbor_id] = score
      end
    end
  end

  def [](item_id)
    if @write_queue.has_key?(item_id)
      @write_queue[item_id]
    else
      retrieve_item(item_id)
    end
  end

  def commit_item!(item_id)
    serialized = serialize_item(item_id)
    Recommendify.redis.hset(redis_key, item_id, serialized)
    @write_queue.delete(item_id)    
  end

  # optimize: the items are already stored in a sorted fashion. we shouldn't 
  # throw away this info by storing them in a hash (and re-sorting later). maybe 
  # use activesupport's orderedhash?
  def retrieve_item(item_id)
    data = Recommendify.redis.hget(redis_key, item_id)
    Hash[data.split("|").map{ |i| (k,s=i.split(":")) && [k,s.to_f] }]
  end

private

  # optimize: implement a better sort. never add more than 50 items the the array
  def serialize_item(item_id, max_precision=5)
    items = @write_queue[item_id].to_a
    items.sort!{ |a,b| b[1] <=> a[1] }
    items = items[0..max_neighbors-1]
    items.map{ |i,s| "#{i}:#{s.to_s[0..max_precision]}" } * "|"
  end

end