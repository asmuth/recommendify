module Recommendify::Base
  def self.included(base)
    base.extend(ClassMethods)
  end

  module ClassMethods
    def input_matrix(key, opts={})
      @matrices ||= {}
      @matrices[key] = opts
    end

    def input_matrices=(val)
      @matrices = val
    end

    def input_matrices
      @matrices
    end

    def max_neighbors(n=nil)    
      return @max_neighbors unless n
      @max_neighbors = n
    end
  end

  def input_matrices
    @input_matrices ||= Hash[self.class.input_matrices.map{ |key, opts|
      opts.merge!(:key => key, :redis_prefix => redis_prefix)
      [ key, Recommendify::InputMatrix.create(opts) ]
    }]
  end

  def similarity_matrix
    @similarity_matrix ||= Recommendify::SimilarityMatrix.new(
      :max_neighbors => max_neighbors,
      :key => similarity_matrix_key,
      :redis_prefix => redis_prefix
    )
  end

  def max_neighbors
    self.class.max_neighbors || Recommendify::DEFAULT_MAX_NEIGHBORS
  end

  def redis_prefix
    "recommendify"
  end

  def similarity_matrix_key
    "#{redis_prefix}_similarities"
  end

  def method_missing(method, *args)
    if input_matrices.has_key?(method)
      input_matrices[method]
    else
      raise NoMethodError.new(method.to_s)
    end
  end

  def respond_to?(method)
    input_matrices.has_key?(method) ? true : super
  end

  def all_items
    input_matrices.map{ |k,m| m.all_items }.flatten.uniq
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