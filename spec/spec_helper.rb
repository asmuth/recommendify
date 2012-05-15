require "rspec"
require "redis"

require ::File.expand_path('../../lib/recommendify', __FILE__)

require ::File.expand_path('../input_matrix_shared.rb', __FILE__)
require ::File.expand_path('../cc_matrix_shared.rb', __FILE__)

def flush_redis!
  Recommendify.redis = Redis.new
  Recommendify.redis.keys("recommendify-test*").each do |k|
    Recommendify.redis.del(k)
  end
end

class Recommendify::Base

  def redis_prefix
    "recommendify-test"
  end

end

class Recommendify::TestInputMatrix

  def initialize(opts)
    @opts = opts
  end

  def method_missing(method, *args)
    @opts[method]
  end

end
