share_examples_for Recommendify::InputMatrix do
  
  it "should build the correct keys" do
    @matrix.redis_key.should == "recommendify-test:mymatrix"
  end

  it "should respond to add_set" do
    @matrix.respond_to?(:add_set).should == true
  end

  it "should respond to add_single" do
    @matrix.respond_to?(:add_single).should == true
  end

  it "should respond to similarity" do
    @matrix.respond_to?(:similarity).should == true
  end

  it "should respond to similarities_for" do
    @matrix.respond_to?(:similarities_for).should == true
  end

  it "should respond to all_items" do
    @matrix.respond_to?(:all_items).should == true
  end

end
