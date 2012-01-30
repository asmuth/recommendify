class Recommendify::Base

  attr_reader :similarity_matrix, :input_matrices

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
    @similarity_matrix = Recommendify::SimilarityMatrix.new
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

  def max_neighbors
    self.class.max_neighbors || Recommendify::DEFAULT_MAX_NEIGHBORS
  end

  def all_items
    @input_matrices.map{ |k,m| m.all_items }.flatten.uniq
  end

  def process!
    all_items.each{ |item_id| process_item!(item_id) }
  end

  def process_item!(item_id)
    neighbors = @input_matrices.inject([]) do |o,(k,m)|
      o += m.similarities_for(item_id)
    end
    similarity_matrix.update(item_id, neighbors)
  end


  # TODO - PSEUDOCODE
  # 
  #  top_items = SortedSet.new
  #  processors.each do |p| 
  #	  sims = p.similarities_for(item_id)
  #	  top_items.merge_with_weight(sims, p.weight)
  #  end
  #  save_most_similar(item_id, top_items.range(0,max_neighbors))
  # end

end