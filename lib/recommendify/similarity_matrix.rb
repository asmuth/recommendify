class Recommendify::SimilarityMatrix

  @@max_neighbors = nil
  @@input_matrices = {}

  def self.max_neighbors(n=nil)    
    return @@max_neighbors unless n
    @@max_neighbors = n
  end

  def self.input_matrix(key, opts)
    @@input_matrices[key] = opts
  end

  def self.input_matrices
    @@input_matrices
  end

  def initialize    
    @input_matrices = Hash[self.class.input_matrices.map{ |key, opts| 
      [ key, Recommendify::InputMatrix.create(opts.merge(:key => key)) ]
    }]
  end

  def method_missing(method, *args)
    if @input_matrices.has_key?(method)
      @input_matrices[method]
    else
      raise NoMethodError.new(method.to_s)
    end
  end

  def respond_to?(method)
    @input_matrices.has_key?(method) ? true : super
  end

  def process!
    all_items = processors.map(&:all_items).flatten
    all_items.each{ |item_id| process_item!(item_id) }
  end

  def max_neighbors
    self.class.max_neighbors || Recommendify::DEFAULT_MAX_NEIGHBORS
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