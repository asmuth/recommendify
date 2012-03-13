share_examples_for Recommendify::CCMatrix do

  it "should build a sparsematrix with the correct key" do
    @matrix.ccmatrix.redis_key.should == "recommendify-test:mymatrix:ccmatrix"
  end

  it "should increment all item counts on set addition" do
    Recommendify.redis.hset("recommendify-test:mymatrix:items", "bar", 2)
    @matrix.add_set("user123", ["foo", "bar"])
    Recommendify.redis.hget("recommendify-test:mymatrix:items", "bar").to_i.should == 3
    Recommendify.redis.hget("recommendify-test:mymatrix:items", "foo").to_i.should == 1
  end

  it "should increment all item<->item pairs on set addition" do
    @matrix.ccmatrix["bar", "foo"] = 2
    res = @matrix.add_set("user123", ["foo", "bar", "fnord"])
    res.length.should == 3
    @matrix.ccmatrix["bar", "foo"].should == 3
    @matrix.ccmatrix["foo", "fnord"].should == 1    
  end

  it "should increment all item<->item paris on single item addition" do
    @matrix.ccmatrix["bar", "fnord"] = 2
    @matrix.add_single("user123", "fnord", ["foo", "bar"])
    @matrix.ccmatrix["bar", "fnord"].should == 3
    @matrix.ccmatrix["foo", "fnord"].should == 1   
  end

  it "should increment the item count on single item addition" do
    @matrix.send(:item_count_incr, "fnordfnord")
    @matrix.send(:item_count_incr, "fnordfnord")
    @matrix.send(:item_count_incr, "foofnord")
    @matrix.add_single("user123", "fnordfnord", ["foofnord", "barfnord"])
    @matrix.send(:item_count, "foofnord").should == 1
    @matrix.send(:item_count, "barfnord").should == 0
    @matrix.send(:item_count, "fnordfnord").should == 3
  end

  it "should calculate all item<->item pairs (3)" do
    res = @matrix.send(:all_pairs, ["foo", "bar", "fnord"])
    res.length.should == 3
    res.should include("bar:foo")
    res.should include("fnord:foo")
    res.should include("bar:fnord")
  end

  it "should calculate all item<->item pairs (6)" do
    res = @matrix.send(:all_pairs, ["foo", "bar", "fnord", "blubb"])
    res.length.should == 6
    res.should include("bar:foo")
    res.should include("fnord:foo")
    res.should include("bar:fnord")
    res.should include("blubb:foo")
    res.should include("blubb:fnord")
    res.should include("bar:blubb")
  end

  it "should return all_items" do
    @matrix.add_set("user42", ["fnord", "blubb"])
    @matrix.add_set("user23", ["hans", "wurst"])
    @matrix.all_items.length.should == 4
    @matrix.all_items.should include("wurst")
    @matrix.all_items.should include("fnord")
    @matrix.all_items.should include("hans")
    @matrix.all_items.should include("wurst")
  end

  it "should delete all item<->item pairs on item deletion" do
    @matrix.ccmatrix["foo", "fnord"] = 2
    @matrix.add_set("user123", ["foo", "bar", "fnord"])
    @matrix.add_set("user456", ["fnord", "blubb"])
    @matrix.ccmatrix["bar", "foo"].should == 1
    @matrix.ccmatrix["foo", "fnord"].should == 3
    @matrix.ccmatrix["blubb", "fnord"].should == 1
    @matrix.delete_item("fnord")
    @matrix.ccmatrix["bar", "foo"].should == 1
    @matrix.ccmatrix["foo", "fnord"].should == 0
    @matrix.ccmatrix["blubb", "fnord"].should == 0  
  end

  it "should delete the item count on deletion" do    
    @matrix.add_set("user123", ["foo", "bar", "fnord"])
    @matrix.add_set("user456", ["fnord", "blubb"])
    @matrix.send(:item_count, "fnord").should == 2
    @matrix.delete_item("fnord")
    @matrix.send(:item_count, "fnord").should == 0
  end

end