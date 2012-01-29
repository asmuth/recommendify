class Recommendify::SparseMatrix

  def initialize(opts={})
    @opts = opts
  end

  def redis_key
    [@opts.fetch(:redis_prefix), @opts.fetch(:key)].join(":")
  end

  def [](x,y)
    k_get(key(x,y))
  end

  def []=(x,y,v)
    v == 0 ? k_del(key(x,y)) : k_set(key(x,y), v)
  end

  def incr(x,y)
    k_incr(key(x,y))
  end

private

  def key(x,y)
    [x,y].sort.join(":")
  end

  def k_set(key, val)
    Recommendify.redis.hset(redis_key, key, val)
  end

  def k_del(key)
    Recommendify.redis.hdel(redis_key, key)
  end

  def k_get(key)
    Recommendify.redis.hget(redis_key, key).to_f
  end

  def k_incr(key)
    Recommendify.redis.hincrby(redis_key, key, 1)
  end

end