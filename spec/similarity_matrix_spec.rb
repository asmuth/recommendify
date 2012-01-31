require ::File.expand_path('../spec_helper', __FILE__)

describe Recommendify::SimilarityMatrix do

  before(:each) do
    flush_redis!
    @matrix = Recommendify::SimilarityMatrix.new(:redis_prefix => "recommendify-test", :key => "mymatrix")
  end

  describe "sorted sets per item" do

    it "should store member similarities" do
      @matrix.update("item_foo", [["item_bar", 0.7], ["item_fnord", 0.3]])
      @matrix.write_queue["item_foo"].length.should == 2
    end

    it "should store member similarities uniquely" do
      @matrix.update("item_fnord", [["item_bar", 0.7], ["item_bar", 0.3]])
      @matrix.write_queue["item_fnord"].length.should == 1
    end

    it "should store member similarities uniquely and sum the scores" do
      @matrix.update("item_fnord", [["item_bar", 0.7], ["item_bar", 0.3]])
      @matrix.write_queue["item_fnord"].should == {"item_bar" => 1.0}
    end

    it "should store multiple member similarities uniquely and sum the scores" do
      @matrix.update("item_fnord", [["item_bar", 0.7], ["item_bar", 0.3], ["item_foo", 0.75]])
      @matrix.write_queue["item_fnord"].should == {"item_bar" => 1.0, "item_foo" => 0.75}
    end

    it "should store multiple member similarities uniquely and sum the scores on multiple updates" do
      @matrix.update("item_fnord", [["item_bar", 0.7], ["item_foo", 0.75]])
      @matrix.update("item_fnord", [["item_fnord", 0.3], ["item_bar", 0.3], ["item_foo", 0.75]])
      @matrix.write_queue["item_fnord"].should == {"item_bar" => 1.0, "item_foo" => 1.5, "item_fnord" => 0.3}
    end

    it "should retrieve the members" do
      @matrix.update("item_fnord", [["item_bar", 0.7], ["item_bar", 0.3], ["item_foo", 0.75]])
      @matrix["item_fnord"].should == {"item_bar" => 1.0, "item_foo" => 0.75}
    end

  end

  describe "serialization/loading to/from redis" do

    it "should serialize a member of the write_queue correctly" do
      @matrix.update("item_fnord", [["item_fnord", 0.3], ["item_bar", 0.3], ["item_foo", 0.75]])
      @matrix.update("item_fnord", [["item_bar", 0.7], ["item_bar", 0.3]])
      @matrix.send(:serialize_item, "item_fnord").should == "item_bar:1.3|item_foo:0.75|item_fnord:0.3"
    end

    it "should write the n-most similar members and scores to redis on commit_item!" do
      @matrix.update("item_fnord", [["item_fnord", 0.3], ["item_bar", 0.3], ["item_foo", 0.75]])
      @matrix.update("item_fnord", [["item_bar", 0.7], ["item_bar", 0.3]])
      @matrix.commit_item!("item_fnord")
      Recommendify.redis.hget("recommendify-test:mymatrix", "item_fnord").should == "item_bar:1.3|item_foo:0.75|item_fnord:0.3"
    end

    it "should not write more than max_neighbors scores to redis on commit_item!" do
      60.times{ |n| @matrix.update("item_fnord", [["item_#{n}", n.to_f/100.0]]) }
      @matrix.commit_item!("item_fnord")
      Recommendify.redis.hget("recommendify-test:mymatrix", "item_fnord").split("|").length.should == 50
    end

    it "should clear the item from the write_queue after commit_item!" do
      60.times{ |n| @matrix.update("item_fnord", [["item_#{n}", n.to_f/100.0]]) }
      @matrix.write_queue["item_fnord"].length.should == 60
      @matrix.commit_item!("item_fnord")
      @matrix.write_queue["item_fnord"].length.should == 0
    end

    it "should retrieve the n-most similar neighbors after stored"

    it "should retrieve the n-most similar neighbors after stored with scores"

  end

end
