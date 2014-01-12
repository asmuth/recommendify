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
    it "should add an input_matrix by 'key'" do
      BaseRecommender.input_matrix(:myinput)
      BaseRecommender.input_matrices.keys.should == [:myinput]
    end

    it "should retrieve an input_matrix on a new instance" do
      BaseRecommender.input_matrix(:myinput)
      sm = BaseRecommender.new
      lambda{ sm.myinput }.should_not raise_error
    end

    it "should retrieve an input_matrix on a new instance and correctly overload respond_to?" do
      BaseRecommender.input_matrix(:myinput)
      sm = BaseRecommender.new
      sm.respond_to?(:process!).should be_true
      sm.respond_to?(:myinput).should be_true
      sm.respond_to?(:fnord).should be_false
    end

    it "should retrieve an input_matrix on a new instance and intialize the correct class" do
      BaseRecommender.input_matrix(:myinput)
      sm = BaseRecommender.new
      sm.myinput.should be_a(Recommendify::InputMatrix)
    end

  end

  describe "process_item!" do
    it "should call process_item! on each input_matrix" do
      BaseRecommender.input_matrix(:myfirstinput)
      BaseRecommender.input_matrix(:mysecondinput)
      sm = BaseRecommender.new
      sm.myfirstinput.should_receive(:process_item!).with("fnorditem").and_return([["fooitem",0.5]])
      sm.mysecondinput.should_receive(:process_item!).with("fnorditem").and_return([["fooitem",0.5]])
      sm.process_item!("fnorditem")
    end

    it "should call process_item! on each input_matrix and add all outputs to the similarity matrix" do
      BaseRecommender.input_matrix(:myfirstinput)
      BaseRecommender.input_matrix(:mysecondinput)
      sm = BaseRecommender.new
      sm.myfirstinput.should_receive(:process_item!).and_return([["fooitem",0.5]])
      sm.mysecondinput.should_receive(:process_item!).and_return([["fooitem",0.75], ["baritem", 1.0]])
      sm.process_item!("fnorditem")
    end

    it "should call process_item! on each input_matrix and add all outputs to the similarity matrix with weight" do
      BaseRecommender.input_matrix(:myfirstinput, :weight => 4.0)
      BaseRecommender.input_matrix(:mysecondinput)
      sm = BaseRecommender.new
      sm.myfirstinput.should_receive(:process_item!).and_return([["fooitem",0.5]])
      sm.mysecondinput.should_receive(:process_item!).and_return([["fooitem",0.75], ["baritem", 1.0]])
      sm.process_item!("fnorditem")
    end
  end

  describe "all_items" do
    it "should retrieve all items from all input matrices" do
      BaseRecommender.input_matrix(:anotherinput)
      BaseRecommender.input_matrix(:yetanotherinput)
      sm = BaseRecommender.new
      sm.anotherinput.add_set('a', ["foo", "bar"])
      sm.yetanotherinput.add_set('b', ["fnord", "shmoo"])
      sm.all_items.length.should == 4
      sm.all_items.should include("foo", "bar", "fnord", "shmoo")
    end

    it "should retrieve all items from all input matrices (uniquely)" do
      BaseRecommender.input_matrix(:anotherinput)
      BaseRecommender.input_matrix(:yetanotherinput)
      sm = BaseRecommender.new
      sm.anotherinput.add_set('a', ["foo", "bar"])
      sm.yetanotherinput.add_set('b', ["fnord", "bar"])
      sm.all_items.length.should == 3
      sm.all_items.should include("foo", "bar", "fnord")
    end
  end

  describe "process!" do
    it "should call process_item for all input_matrix.all_items's" do
      BaseRecommender.input_matrix(:anotherinput)
      BaseRecommender.input_matrix(:yetanotherinput)
      sm = BaseRecommender.new
      sm.anotherinput.add_set('a', ["foo", "bar"])
      sm.yetanotherinput.add_set('b', ["fnord", "shmoo"])
      sm.anotherinput.should_receive(:process!).exactly(1).times
      sm.yetanotherinput.should_receive(:process!).exactly(1).times
      sm.process!
    end
  end

  describe "process_predictions" do
    it "should do something"
  end

  describe "predictions_for" do
    it "should do something"
  end

  describe "similarities_for(item_id)" do
    it "should not throw exception for non existing items" do
      sm = BaseRecommender.new
      sm.similarities_for("not_existing_item").length.should == 0
    end
  end

  describe "sets_for" do
    it "should return all the sets the given item is in" do
      BaseRecommender.input_matrix(:set1)
      BaseRecommender.input_matrix(:set2)
      sm = BaseRecommender.new
      sm.set1.add_set "item1", ["foo", "bar"]
      sm.set1.add_set "item2", ["nada", "bar"]
      sm.set2.add_set "item3", ["bar", "other"]
      sm.sets_for("bar").length.should == 3
      sm.sets_for("bar").should include("item1", "item2", "item3")
      sm.sets_for("other").should == ["item3"]
    end
  end

  describe "delete_item!" do

    it "should call delete_item on each input_matrix" do
      BaseRecommender.input_matrix(:myfirstinput)
      BaseRecommender.input_matrix(:mysecondinput)
      sm = BaseRecommender.new
      sm.myfirstinput.should_receive(:delete_item!).with("fnorditem")
      sm.mysecondinput.should_receive(:delete_item!).with("fnorditem")
      sm.delete_item!("fnorditem")
    end

    it "should delete the item from the similarity matrix"

    it "should delete all occurences of the item in other similarity sets from the similarity matrix"

  end

end
