class Recommendify::JaccardProcessor

  attr_reader :ccmatrix

  def initialize(opts={})
    @opts = opts
    @ccmatrix = Recommendify::SparseMatrix.new(
      :redis_prefix => @opts.fetch(:redis_prefix),
      :key => [@opts.fetch(:key), :ccmatrix].join(":")
    )
  end

  def redis_key
    [@opts.fetch(:redis_prefix), @opts.fetch(:key)].join(":")
  end

end