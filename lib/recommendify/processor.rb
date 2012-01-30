class Recommendify::Processor

  def initialize(opts)
    @opts = opts
  end

  def redis_key(append=nil)
    [@opts.fetch(:redis_prefix), @opts.fetch(:key), append].flatten.compact.join(":")
  end

  # add a set of item_ids to the matrix
  def add_set(set_id, item_ids)
  	raise "implemented in subclass"
  end

  # add a single item to a set of item_ids to the matrix
  def add_singele(set_id, item_id, other_item_ids)
  	raise "implemented in subclass"
  end

  # calculate the similarity between item1 and item1 (0.0-1.0)
  def similarity(item1, item2)
  	raise "implemented in subclass"
  end

  # calculate all similarities to other items in the matrix for item1
  def similarities_for(item1)
  	# return => [ ["item23", 0.6], ["item42", 0.23], (...) ]
  	raise "implemented in subclass"
  end

  # retrieve all item_ids in the matrix
  def all_items
  	# retzrb => [ "item23", "item42", "item17", (...) ]
  	raise "implemented in subclass"
  end

end