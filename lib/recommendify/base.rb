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

  def self.similarity_matrix(name)
    @similarity_matrix = name
  end

  def self.similarity_matrix_key
    @similarity_matrix || :similarities
  end

  def self.redis_prefix(name)
    @redis_prefix = name
  end

  def self.redis_prefix_value
    @redis_prefix || "recommendify"
  end

  def initialize    
    @input_matrices = Hash[self.class.input_matrices.map{ |key, opts| 
      opts.merge!(:key => key, :redis_prefix => redis_prefix)
      [ key, Recommendify::InputMatrix.create(opts) ]
    }]
    @similarity_matrix = Recommendify::SimilarityMatrix.new(
      :max_neighbors => max_neighbors,
      :key => self.class.similarity_matrix_key,
      :redis_prefix => redis_prefix
    )
  end

  def redis_prefix
    self.class.redis_prefix_value
  end

  def max_neighbors
    self.class.max_neighbors || Recommendify::DEFAULT_MAX_NEIGHBORS
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

  def all_items
    @input_matrices.map{ |k,m| m.all_items }.flatten.uniq
  end

  def for(item_id)
    similarity_matrix[item_id].map do |item_id, similarity|
      Recommendify::Neighbor.new(
        :item_id => item_id, 
        :similarity => similarity
      )
    end.sort
  end

  def process!
    all_items.each{ |item_id,n| process_item!(item_id) }
  end

  def process_item!(item_id)
    input_matrices.map do |k,m|
      neighbors = m.similarities_for(item_id).map do |i,w|        
        [i,w*m.weight]
      end
      similarity_matrix.update(item_id, neighbors)
    end
    similarity_matrix.commit_item!(item_id)
  end

  def delete_item!(item_id)
    input_matrices.map do |k,m|
      m.delete_item(item_id)
    end
  end

end
