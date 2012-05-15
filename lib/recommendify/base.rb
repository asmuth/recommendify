class Recommendify::Base

  attr_reader :similarity_matrix, :input_matrices, :max_neighbors

  def initialize(input_matrices = nil, opts = {})
    @max_neighbors = opts[:max_neighbors] || Recommendify::DEFAULT_MAX_NEIGHBORS

    @input_matrices = if input_matrices
      Hash[input_matrices.map{ |key, opts|
        opts.merge!(:key => key, :redis_prefix => redis_prefix)
        [ key, Recommendify::InputMatrix.create(opts) ]
      }]
    else
      {}
    end

    @similarity_matrix = Recommendify::SimilarityMatrix.new(
      :max_neighbors => max_neighbors,
      :key => :similarities,
      :redis_prefix => redis_prefix
    )
  end

  def redis_prefix
    "recommendify"
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