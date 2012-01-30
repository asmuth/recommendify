share_examples_for Recommendify::InputMatrix do
  
  it "should build the correct keys" do
    @matrix.redis_key.should == "recommendify:mymatrix"
  end

  it "should respond to add_set" do
    @matrix.respond_to(:add_set).should == true
  end

  it "should respond to add_single" do
    @matrix.respond_to(:add_set).should == true
  end

  it "should respond to similarity" do
    @matrix.respond_to(:add_set).should == true
  end

  it "should respond to similarities_for" do
    @matrix.respond_to(:add_set).should == true
  end

  it "should respond to all_items" do
    @matrix.respond_to(:add_set).should == true
  end

end
