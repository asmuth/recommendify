require "rspec"
require ::File.expand_path('../../lib/recommendify', __FILE__)

class TestRecommender < Recommendify::SimilarityMatrix

  processor :jaccard_one, 
    :similarity_func => :jaccard

end