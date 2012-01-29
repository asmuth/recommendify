require ::File.expand_path('../spec_helper', __FILE__)

describe Recommendify::JaccardProcessor do

  before(:all) do
    Recommendify.redis = Redis.new
    @jaccard = Recommendify::JaccardProcessor.new(:redis_prefix => "recommendify", :key => "myjeccard")
  end

  before(:each) do
    Recommendify.redis.keys("recommendify*").each do |k|
      Recommendify.redis.del(k)
    end
  end
  
  it "should build the correct keys" do
    @jaccard.redis_key.should == "recommendify:myjeccard"
  end

  it "should build a sparsematrix with the correct key" do
    @jaccard.ccmatrix.redis_key.should == "recommendify:myjeccard:ccmatrix"
  end


end
