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
  end

  def input_matrices
    @input_matrices ||= Hash[self.class.input_matrices.map{ |key, opts|
      opts.merge!(:key => key, :redis_prefix => redis_prefix)
      [ key, Recommendify::InputMatrix.new(opts) ]
    }]
  end

  def redis_prefix
    "recommendify"
  end

  def redis_key(*append)
    ([redis_prefix] + append).flatten.compact.join(":")
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
    Recommendify.redis.sunion input_matrices.map{|k,m| m.redis_key(:all_items)}
  end

  def process_predictions(set_id, matrix_label)
    key = redis_key(:predictions, set_id)
    matrix = input_matrices[matrix_label]
    redis = Recommendify.redis

    item_set = redis.smembers(matrix.redis_key(:items, set_id))

    item_keys = item_set.map do |item|
      input_matrices.map{ |k,m| m.redis_key(:similarities, item) }
    end.flatten

    item_weights = item_keys.map do |item_key|
      scores = redis.zrange item_key, 0, -1, with_scores: true
      unless scores.empty?
        1.0/scores.map{|x,y| y}.reduce(:+)
      else
        0
      end
    end

    unless item_keys.empty?
      redis.multi do |multi|
        multi.del key
        multi.zunionstore key, item_keys, weights: item_weights
        multi.zrem key, item_set
      end
      return predictions_for(set_id)
    else
      return []
    end
  end

  def predictions_for(set_id, with_scores = false)
    Recommendify.redis.zrevrange redis_key(:predictions, set_id), 0, -1, with_scores: with_scores
  end

  def ids_for(item, with_scores = false)
    similarities_for(item, with_scores)
  end

  def similarities_for(item, with_scores = false)
    keys = input_matrices.map{ |k,m| m.redis_key(:similarities, item) }
    neighbors = nil
    unless keys.empty?
      Recommendify.redis.multi do |multi|
        multi.zunionstore 'temp', keys
        neighbors = multi.zrevrange('temp', 0, -1, with_scores: with_scores)
      end
      return neighbors.value
    else
      return []
    end
  end

  def sets_for(item)
    keys = input_matrices.map{ |k,m| m.redis_key(:sets, item) }
    Recommendify.redis.sunion keys
  end

  def process!
    input_matrices.each do |k,m|
      m.process!
    end
    return self
  end

  def process_item!(item)
    input_matrices.each do |k,m|
      m.process_item!(item)
    end
    return self
  end

  def delete_item!(item_id)
    input_matrices.each do |k,m|
      m.delete_item!(item_id)
    end
    return self
  end
end