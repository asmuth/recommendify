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

  it "should capitalize a string" do
    Recommendify.capitalize("fuubar").should == "Fuubar"
    Recommendify.capitalize("fuUBar").should == "Fuubar"
    Recommendify.capitalize("FUUBAR").should == "Fuubar"
    Recommendify.capitalize("Fuubar").should == "Fuubar"
  end

  it "should constantize a string" do
    obj = Recommendify.constantize("JaccardInputMatrix")
    # should_be doesn't work here...
    obj.inspect.should == "Recommendify::JaccardInputMatrix"
  end

end
