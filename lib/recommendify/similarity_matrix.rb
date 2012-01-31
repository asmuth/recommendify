class Recommendify::SimilarityMatrix

  attr_reader :write_queue

  def initialize(opts={})
    @write_queue = Hash.new{ |h,k| h[k] = {} }
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
      raise "here be dragons"
    end
  end

  def commit_item!(item_id)
    @write_queue.delete(item_id)
  end

  def retrieve_item(item_id)

  end

private

  # optimize: implement a better sort. never add more than 50 items 
  # the the array
  def serialize_item(item_id, max_precision=5)
    items = @write_queue[item_id].to_a
    items.sort!{ |a,b| b[1] <=> a[1] }
    items.map{ |i,s| "#{i}:#{s.to_s[0..max_precision]}" } * "|"
  end

end