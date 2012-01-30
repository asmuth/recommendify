class Recommendify::SimilarityMatrix

  def self.method_missing(method, *args)
    puts method
  end

  def process!
    all_items = processors.map(&:all_items).flatten
    all_items.each{ |item_id| process_item!(item_id) }
  end

  # TODO - PSEUDOCODE
  # def process_item!(item_id)
  #  top_items = SortedSet.new
  #  processors.each do |p| 
  #	  sims = p.similarities_for(item_id)
  #	  top_items.merge_with_weight(sims, p.weight)
  #  end
  #  save_most_similar(item_id, top_items.range(0,max_neighbors))
  # end

end