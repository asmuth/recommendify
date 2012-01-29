require ::File.expand_path('../spec_helper', __FILE__)

describe Recommendify do

  it "should store a redis connection" do
    Recommendify.redis = "asd"
    Recommendify.redis.should == "asd"
  end

  it "should raise an exception if unconfigured redis connection is accessed" do
    Recommendify.redis = nil
    lambda{ ecommendify.redis }.should raise_error
  end

end
