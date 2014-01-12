require ::File.expand_path('../spec_helper', __FILE__)

describe Recommendify::InputMatrix do

  before(:all) do
    @matrix = Recommendify::InputMatrix.new(:redis_prefix => "recommendify-test", :key => "mymatrix")
  end

  before(:each) do
    flush_redis!
  end

  it_should_behave_like Recommendify::InputMatrix

  describe "weight" do
    it "returns the weight configured or a default of 1" do
      @matrix.weight.should == 1.0  # default weight
      matrix = Recommendify::InputMatrix.new(redis_prefix: "recommendify-test", key: "mymatrix", weight: 5.0)
      matrix.weight.should == 5.0
    end
  end

  describe "add_set" do
    it "adds each member of the set to the 'all_items' set" do
      @matrix.all_items.should_not include("foo", "bar", "fnord", "blubb")
      @matrix.add_set "item1", ["foo", "bar", "fnord", "blubb"]
      @matrix.all_items.should include("foo", "bar", "fnord", "blubb")
    end

    it "adds each member of the set to the key's 'sets' set" do
      @matrix.items_for("item1").should_not include("foo", "bar", "fnord", "blubb")
      @matrix.add_set "item1", ["foo", "bar", "fnord", "blubb"]
      @matrix.items_for("item1").should include("foo", "bar", "fnord", "blubb")
    end

    it "adds the key to each set member's 'items' set" do
      @matrix.sets_for("foo").should_not include("item1")
      @matrix.sets_for("bar").should_not include("item1")
      @matrix.sets_for("fnord").should_not include("item1")
      @matrix.sets_for("blubb").should_not include("item1")
      @matrix.add_set "item1", ["foo", "bar", "fnord", "blubb"]
      @matrix.sets_for("foo").should include("item1")
      @matrix.sets_for("bar").should include("item1")
      @matrix.sets_for("fnord").should include("item1")
      @matrix.sets_for("blubb").should include("item1")
    end
  end

  describe "add_single" do
    it "adds the item to the 'all_items' set" do
      @matrix.all_items.should_not include("foo")
      @matrix.add_single "item1", "foo"
      @matrix.all_items.should include("foo")
    end

    it "adds the item to the key's 'sets' set" do
      @matrix.items_for("item1").should_not include("foo")
      @matrix.add_single "item1", "foo"
      @matrix.items_for("item1").should include("foo")
    end

    it "adds the key to the item's 'items' set" do
      @matrix.sets_for("foo").should_not include("item1")
      @matrix.add_single "item1", "foo"
      @matrix.sets_for("foo").should include("item1")
    end
  end

  describe "all_items" do
    it "returns all items across all sets in the input matrix" do
      @matrix.add_set "item1", ["foo", "bar", "fnord", "blubb"]
      @matrix.add_set "item2", ["foo", "bar", "snafu", "nada"]
      @matrix.add_set "item3", ["nada"]
      @matrix.all_items.should include("foo", "bar", "fnord", "blubb", "snafu", "nada")
      @matrix.all_items.length.should == 6
    end
  end

  describe "items_for" do
    it "returns the items in the given set ID" do
      @matrix.add_set "item1", ["foo", "bar", "fnord", "blubb"]
      @matrix.items_for("item1").should include("foo", "bar", "fnord", "blubb")
      @matrix.add_set "item2", ["foo", "bar", "snafu", "nada"]
      @matrix.items_for("item2").should include("foo", "bar", "snafu", "nada")
      @matrix.items_for("item1").should_not include("snafu", "nada")
    end
  end

  describe "sets_for" do
    it "returns the set IDs the given item is in" do
      @matrix.add_set "item1", ["foo", "bar", "fnord", "blubb"]
      @matrix.add_set "item2", ["foo", "bar", "snafu", "nada"]
      @matrix.sets_for("foo").should include("item1", "item2")
      @matrix.sets_for("snafu").should == ["item2"]
    end
  end

  describe "related_items" do
    it "returns the items in sets the given item is also in" do
      @matrix.add_set "item1", ["foo", "bar", "fnord", "blubb"]
      @matrix.add_set "item2", ["foo", "bar", "snafu", "nada"]
      @matrix.add_set "item3", ["nada", "other"]
      @matrix.related_items("bar").should include("foo", "fnord", "blubb", "snafu", "nada")
      @matrix.related_items("bar").length.should == 5
      @matrix.related_items("other").should == ["nada"]
      @matrix.related_items("snafu").should include("foo", "bar", "nada")
      @matrix.related_items("snafu").length.should == 3
    end
  end

  describe "similarity" do
    it "should calculate the correct similarity between two items" do
      add_two_item_test_data!(@matrix)
      @matrix.process!
      @matrix.similarity("fnord", "blubb").should == 0.4
      @matrix.similarity("blubb", "fnord").should == 0.4
    end
  end

  describe "similarities_for" do
    it "should calculate all similarities for an item (1/3)" do
      add_three_item_test_data!(@matrix)
      @matrix.process!
      res = @matrix.similarities_for("fnord", true)
      res.length.should == 2
      res[0].should == ["shmoo", 0.75]
      res[1].should == ["blubb", 0.4]
    end

    it "should calculate all similarities for an item (2/3)" do
      add_three_item_test_data!(@matrix)
      @matrix.process!
      res = @matrix.similarities_for("shmoo", true)
      res.length.should == 2
      res[0].should == ["fnord", 0.75]
      res[1].should == ["blubb", 0.2]
    end


    it "should calculate all similarities for an item (3/3)" do
      add_three_item_test_data!(@matrix)
      @matrix.process!
      res = @matrix.similarities_for("blubb", true)
      res.length.should == 2
      res[0].should == ["fnord", 0.4]
      res[1].should == ["shmoo", 0.2]
    end
  end

  describe "delete_item!" do
    before do
      @matrix.add_set "item1", ["foo", "bar", "fnord", "blubb"]
      @matrix.add_set "item2", ["foo", "bar", "snafu", "nada"]
      @matrix.add_set "item3", ["nada", "other"]
      @matrix.process!
    end

    it "should delete the item from sets it is in" do
      @matrix.items_for("item1").should include("bar")
      @matrix.items_for("item2").should include("bar")
      @matrix.sets_for("bar").should include("item1", "item2")
      @matrix.delete_item!("bar")
      @matrix.items_for("item1").should_not include("bar")
      @matrix.items_for("item2").should_not include("bar")
      @matrix.sets_for("bar").should be_empty
    end

    it "should delete the cached similarities for the item" do
      @matrix.similarities_for("bar").should_not be_empty
      @matrix.delete_item!("bar")
      @matrix.similarities_for("bar").should be_empty
    end

    it "should delete the item from other cached similarities" do
      @matrix.similarities_for("foo").should include("bar")
      @matrix.delete_item!("bar")
      @matrix.similarities_for("foo").should_not include("bar")
    end

    it "should delete the item from the all_items set" do
      @matrix.all_items.should include("bar")
      @matrix.delete_item!("bar")
      @matrix.all_items.should_not include("bar")
    end
  end

  it "should calculate the correct jaccard index" do
    @matrix.add_set "item1", ["foo", "bar", "fnord", "blubb"]
    @matrix.add_set "item2", ["bar", "fnord", "shmoo", "snafu"]
    @matrix.add_set "item3", ["bar", "nada", "snafu"]

    @matrix.send(:calculate_jaccard,
      "bar",
      "snafu"
    ).should == 2.0/3.0
  end

private

  def add_two_item_test_data!(matrix)
    matrix.add_set("user42", ["fnord", "blubb"])
    matrix.add_set("user44", ["blubb"])
    matrix.add_set("user46", ["fnord"])
    matrix.add_set("user48", ["fnord", "blubb"])
    matrix.add_set("user50", ["fnord"])
  end

  def add_three_item_test_data!(matrix)
    matrix.add_set("user42", ["fnord", "blubb", "shmoo"])
    matrix.add_set("user44", ["blubb"])
    matrix.add_set("user46", ["fnord", "shmoo"])
    matrix.add_set("user48", ["fnord", "blubb"])
    matrix.add_set("user50", ["fnord", "shmoo"])
  end

end