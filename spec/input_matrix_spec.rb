require ::File.expand_path('../spec_helper', __FILE__)

describe Recommendify::InputMatrix do

  before(:all) do
    @matrix = Recommendify::InputMatrix.new(:redis_prefix => "recommendify", :key => "mymatrix")
  end

  before(:each) do
    flush_redis!
  end

  it_should_behave_like Recommendify::InputMatrix

  describe "object creation" do

     it "should create an object with the correct class" do
       obj = Recommendify::InputMatrix.create(:key => "fubar", :similarity_func => :jaccard)
       obj.should be_a(Recommendify::JaccardInputMatrix)
     end

     it "should create an object with the correct class and pass opts" do
       obj = Recommendify::InputMatrix.create(:key => "fubar", :similarity_func => :jaccard)
       obj.instance_variable_get(:@opts)[:key].should == "fubar"
     end

  end

end