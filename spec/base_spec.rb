require ::File.expand_path('../spec_helper', __FILE__)

describe Recommendify::Base do

  describe "configuration" do

    before(:each) do
      Recommendify::Base.class_variable_set(:@@max_neighbors, nil)
      Recommendify::Base.class_variable_set(:@@input_matrices, {})
    end

    it "should return default max_neighbors if not configured" do
      Recommendify::DEFAULT_MAX_NEIGHBORS.should == 50      
      sm = Recommendify::Base.new
      sm.max_neighbors.should == 50
    end
      
    it "should remember max_neighbors if configured" do
      Recommendify::Base.max_neighbors(23)
      sm = Recommendify::Base.new      
      sm.max_neighbors.should == 23
    end

    it "should add an input_matrix by 'key'" do
      Recommendify::Base.input_matrix(:myinput, :similarity_func => :jaccard)
      Recommendify::Base.class_variable_get(:@@input_matrices).keys.should == [:myinput]
    end

    it "should retrieve an input_matrix on a new instance" do
      Recommendify::Base.input_matrix(:myinput, :similarity_func => :jaccard)
      sm = Recommendify::Base.new
      lambda{ sm.myinput }.should_not raise_error
    end

    it "should retrieve an input_matrix on a new instance and correctly overload respond_to?" do
      Recommendify::Base.input_matrix(:myinput, :similarity_func => :jaccard)
      sm = Recommendify::Base.new
      sm.respond_to?(:process!).should be_true
      sm.respond_to?(:myinput).should be_true
      sm.respond_to?(:fnord).should be_false
    end

    it "should retrieve an input_matrix on a new instance and intialize the correct class" do
      Recommendify::Base.input_matrix(:myinput, :similarity_func => :jaccard)
      sm = Recommendify::Base.new
      sm.myinput.should be_a(Recommendify::JaccardInputMatrix)
    end

  end

  describe "process_item!" do

    it "should call similarities_for on each input_matrix" do
      Recommendify::Base.input_matrix(:myfirstinput, :similarity_func => :jaccard)
      Recommendify::Base.input_matrix(:mysecondinput, :similarity_func => :jaccard)
      sm = Recommendify::Base.new
      sm.myfirstinput.should_receive(:similarities_for).with("fnorditem")
      sm.mysecondinput.should_receive(:similarities_for).with("fnorditem")
      sm.process_item!("fnorditem")
    end

    it "should add all similarities correctly" do
      Recommendify::Base.input_matrix(:myfirstinput, :similarity_func => :jaccard)
      Recommendify::Base.input_matrix(:mysecondinput, :similarity_func => :jaccard)
      sm = Recommendify::Base.new
      sm.myfirstinput.should_receive(:similarities_for).and_return(0.5)
      sm.mysecondinput.should_receive(:similarities_for).and_return(0.75)
      sm.send(:aggregated_similarities_for, "foobar").should == [ ["fnorditem", 1.25] ]
    end

    it "should add all similarities correctly with weight" do
      Recommendify::Base.input_matrix(:myfirstinput, :similarity_func => :jaccard, :weight => 4.0)
      Recommendify::Base.input_matrix(:mysecondinput, :similarity_func => :jaccard)
      sm = Recommendify::Base.new
      sm.myfirstinput.should_receive(:similarities_for).and_return(0.5)
      sm.mysecondinput.should_receive(:similarities_for).and_return(0.75)
      sm.send(:aggregated_similarities_for, "foobar").should == [ ["fnorditem", 2.75] ]
    end

    it "should calculate the top max_neighbors items"

    it "should store the top max_neighbors items"

    it "should retrieve the top max_neighbors items after stored"

  end

  describe "process!" do

    it "should call process_item for all input_matrix.all_items's"

    it "should call process_item for all input_matrix.all_items's (uniquely)"

  end

end
