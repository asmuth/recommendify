require ::File.expand_path('../spec_helper', __FILE__)

describe Recommendify::SparseMatrix do

  before(:all) do
  	@sm = Recommendify::SparseMatrix.new(:redis_prefix => "recommendify", :key => "mysparsematrix")
  end

  before(:each) do
  	Recommendify.redis.keys("recommendify*").each do |k|
	  Recommendify.redis.del(k)
  	end
  end
  
  it "should build the correct keys" do
  	@sm.redis_key.should == "recommendify:mysparsematrix"
  end

  it "should retrieve a value" do
  	Recommendify.redis.hset("recommendify:mysparsematrix", "bar:foo", 43)
  	@sm["bar"]["foo"].should == 43
  end

  it "should retrieve a value regardless of parameter order" do
  	Recommendify.redis.hset("recommendify:mysparsematrix", "one:two", 14)
  	@sm["one"]["two"].should == 14
  	@sm["two"]["one"].should == 14
  end

  it "should store a value" do
  	@sm["bar"]["foo"]
  	Recommendify.redis.hset("recommendify:mysparsematrix", "bar:foo", 43)
  	.should == 43
  end


  it "should return 0 if the key is not found"

  it "should increment a value"

  it "should not create unneseccary keys"

  it "should not create a key if the value is 0"

  it "should delete a key if the value is set to 0"

end
