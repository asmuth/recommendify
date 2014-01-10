require ::File.expand_path('../spec_helper', __FILE__)

describe Recommendify::Base do
  class BaseRecommender
    include Recommendify::Base
  end

  before(:each) do
    flush_redis!
    BaseRecommender.input_matrices = {}
  end

  describe "configuration" do

    it "should return default max_neighbors if not configured" do
      Recommendify::DEFAULT_MAX_NEIGHBORS.should == 50
      sm = BaseRecommender.new
      sm.max_neighbors.should == 50
    end

    it "should remember max_neighbors if configured" do
      BaseRecommender.max_neighbors(23)
      sm = BaseRecommender.new
      sm.max_neighbors.should == 23
    end

    it "should add an input_matrix by 'key'" do
      BaseRecommender.input_matrix(:myinput, :similarity_func => :jaccard)
      BaseRecommender.input_matrices.keys.should == [:myinput]
    end

    it "should retrieve an input_matrix on a new instance" do
      BaseRecommender.input_matrix(:myinput, :similarity_func => :jaccard)
      sm = BaseRecommender.new
      lambda{ sm.myinput }.should_not raise_error
    end

    it "should retrieve an input_matrix on a new instance and correctly overload respond_to?" do
      BaseRecommender.input_matrix(:myinput, :similarity_func => :jaccard)
      sm = BaseRecommender.new
      sm.respond_to?(:process!).should be_true
      sm.respond_to?(:myinput).should be_true
      sm.respond_to?(:fnord).should be_false
    end

    it "should retrieve an input_matrix on a new instance and intialize the correct class" do
      BaseRecommender.input_matrix(:myinput, :similarity_func => :jaccard)
      sm = BaseRecommender.new
      sm.myinput.should be_a(Recommendify::JaccardInputMatrix)
    end

  end

  describe "process_item!" do

    it "should call similarities_for on each input_matrix" do
      BaseRecommender.input_matrix(:myfirstinput, :similarity_func => :jaccard)
      BaseRecommender.input_matrix(:mysecondinput, :similarity_func => :jaccard)
      sm = BaseRecommender.new
      sm.myfirstinput.should_receive(:process_item!).with("fnorditem").and_return([["fooitem",0.5]])
      sm.mysecondinput.should_receive(:process_item!).with("fnorditem").and_return([["fooitem",0.5]])
      sm.process_item!("fnorditem")
    end

    it "should call similarities_for on each input_matrix and add all outputs to the similarity matrix" do
      BaseRecommender.input_matrix(:myfirstinput, :similarity_func => :jaccard)
      BaseRecommender.input_matrix(:mysecondinput, :similarity_func => :jaccard)
      sm = BaseRecommender.new
      sm.myfirstinput.should_receive(:process_item!).and_return([["fooitem",0.5]])
      sm.mysecondinput.should_receive(:process_item!).and_return([["fooitem",0.75], ["baritem", 1.0]])
      sm.process_item!("fnorditem")
    end

    it "should call similarities_for on each input_matrix and add all outputs to the similarity matrix with weight" do
      BaseRecommender.input_matrix(:myfirstinput, :similarity_func => :jaccard, :weight => 4.0)
      BaseRecommender.input_matrix(:mysecondinput, :similarity_func => :jaccard)
      sm = BaseRecommender.new
      sm.myfirstinput.should_receive(:process_item!).and_return([["fooitem",0.5]])
      sm.mysecondinput.should_receive(:process_item!).and_return([["fooitem",0.75], ["baritem", 1.0]])
      sm.process_item!("fnorditem")
    end

    it "should retrieve all items from all input matrices" do
      BaseRecommender.input_matrix(:anotherinput, :similarity_func => :jaccard)
      BaseRecommender.input_matrix(:yetanotherinput, :similarity_func => :jaccard)
      sm = BaseRecommender.new
      sm.anotherinput.add_set('a', ["foo", "bar"])
      sm.yetanotherinput.add_set('b', ["fnord", "shmoo"])
      sm.all_items.length.should == 4
      sm.all_items.should include("foo")
      sm.all_items.should include("bar")
      sm.all_items.should include("fnord")
      sm.all_items.should include("shmoo")
    end

    it "should retrieve all items from all input matrices (uniquely)" do
      BaseRecommender.input_matrix(:anotherinput, :similarity_func => :jaccard)
      BaseRecommender.input_matrix(:yetanotherinput, :similarity_func => :jaccard)
      sm = BaseRecommender.new
      sm.anotherinput.add_set('a', ["foo", "bar"])
      sm.yetanotherinput.add_set('b', ["fnord", "bar"])
      sm.all_items.length.should == 3
      sm.all_items.should include("foo")
      sm.all_items.should include("bar")
      sm.all_items.should include("fnord")
    end

  end

  describe "process!" do

    it "should call process_item for all input_matrix.all_items's" do
      BaseRecommender.input_matrix(:anotherinput, :similarity_func => :jaccard)
      BaseRecommender.input_matrix(:yetanotherinput, :similarity_func => :jaccard)
      sm = BaseRecommender.new
      sm.anotherinput.add_set('a', ["foo", "bar"])
      sm.yetanotherinput.add_set('b', ["fnord", "shmoo"])
      sm.anotherinput.should_receive(:process!).exactly(1).times
      sm.yetanotherinput.should_receive(:process!).exactly(1).times
      sm.process!
    end

    it "should call process_item for all input_matrix.all_items's (uniquely)" do
      BaseRecommender.input_matrix(:anotherinput, :similarity_func => :test)
      BaseRecommender.input_matrix(:yetanotherinput, :similarity_func => :test)
      sm = BaseRecommender.new
      sm.anotherinput.add_set('a', ["foo", "bar"])
      sm.yetanotherinput.add_set('b', ["fnord", "bar"])
      sm.anotherinput.should_receive(:process!).exactly(1).times
      sm.yetanotherinput.should_receive(:process!).exactly(1).times
      sm.process!
    end

  end

  describe "for(item_id)" do

    it "should not throw exception for non existing items" do
      sm = BaseRecommender.new
      sm.similarities_for("not_existing_item").length.should == 0
    end

    it "should not call split on nil when retrieving a non-existent item (return an empty array)" do
      sm = BaseRecommender.new
      sm.similarities_for("NONEXISTENT").should == []
    end

  end

  describe "delete_item!" do

    it "should call delete_item on each input_matrix" do
      BaseRecommender.input_matrix(:myfirstinput, :similarity_func => :jaccard)
      BaseRecommender.input_matrix(:mysecondinput, :similarity_func => :jaccard)
      sm = BaseRecommender.new
      sm.myfirstinput.should_receive(:delete_item).with("fnorditem")
      sm.mysecondinput.should_receive(:delete_item).with("fnorditem")
      sm.delete_item!("fnorditem")
    end

    it "should delete the item from the similarity matrix"

    it "should delete all occurences of the item in other similarity sets from the similarity matrix"

  end

end
