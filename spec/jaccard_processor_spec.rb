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

  it "should increment all item counts on addition" do
    Recommendify.redis.hset("recommendify:myjeccard:items", "bar", 2)
    @jaccard.add_set("user123", ["foo", "bar"])
    Recommendify.redis.hget("recommendify:myjeccard:items", "bar").to_i.should == 3
    Recommendify.redis.hget("recommendify:myjeccard:items", "foo").to_i.should == 1
  end

  it "should increment all item<->item pairs on set addition" do
    @jaccard.ccmatrix["bar", "foo"] = 2
    res = @jaccard.add_set("user123", ["foo", "bar", "fnord"])
    res.length.should == 3
    @jaccard.ccmatrix["bar", "foo"].should == 3
    @jaccard.ccmatrix["foo", "fnord"].should == 1    
  end

  it "should calculate all item<->item pairs (3)" do
    res = @jaccard.send(:all_pairs, ["foo", "bar", "fnord"])
    res.length.should == 3
    res.should include("bar:foo")
    res.should include("fnord:foo")
    res.should include("bar:fnord")
  end

  it "should calculate all item<->item pairs (6)" do
    res = @jaccard.send(:all_pairs, ["foo", "bar", "fnord", "blubb"])
    res.length.should == 6
    res.should include("bar:foo")
    res.should include("fnord:foo")
    res.should include("bar:fnord")
    res.should include("blubb:foo")
    res.should include("blubb:fnord")
    res.should include("bar:blubb")
  end

  it "should calculate the correct jaccard index" do
    @jaccard.send(:calculate_jaccard, 
      ["foo", "bar", "fnord", "blubb"],
      ["bar", "fnord", "shmoo", "snafu"]
    ).should == 2.0/6.0
  end

  it "should calculate the correct similarity between to items" do
    @jaccard.add_set("user42", ["fnord", "blubb"])
    @jaccard.add_set("user44", ["blubb"])
    @jaccard.add_set("user46", ["fnord"])
    @jaccard.add_set("user48", ["fnord", "blubb"])
    @jaccard.add_set("user50", ["fnord"])
    # sim(fnord,blubb) = (users(fnord) & users(blub)) / (users(fnord) + users(blubb))
    # => {user42 user48} / {user42 user46 user48 user50} + {user42 user44 user48}
    # => {user42 user48} / {user42 user44 user46 user48 user50}
    # => 2 / 5 => 0.4
    @jaccard.similarity("fnord", "blubb").should == 0.4
    @jaccard.similarity("blubb", "fnord").should == 0.4
  end

end

