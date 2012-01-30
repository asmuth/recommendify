share_examples_for Recommendify::InputMatrix do
  
  it "should build the correct keys" do
    @matrix.redis_key.should == "recommendify:mymatrix"
  end

  it "should respond to add_set"

  it "should respond to add_single"

  it "should respond to similarity"

  it "should respond to similarities_for"

  it "should respond to all_items"

end
