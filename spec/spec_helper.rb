require "rspec"
require "redis"

require ::File.expand_path('../../lib/recommendify', __FILE__)

require ::File.expand_path('../input_matrix_shared.rb', __FILE__)
require ::File.expand_path('../cc_matrix_shared.rb', __FILE__)

def flush_redis!
  Recommendify.redis = Redis.new
  Recommendify.redis.keys("recommendify*").each do |k|
    Recommendify.redis.del(k)
  end
end

class TestRecommender < Recommendify::SimilarityMatrix

  input_matrix :jaccard_one, 
    :similarity_func => :jaccard

end