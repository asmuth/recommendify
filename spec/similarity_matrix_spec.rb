require ::File.expand_path('../spec_helper', __FILE__)

describe Recommendify::SimilarityMatrix do

  describe "configuration" do

    it "should return default max_neighbors if not configured" do
      Recommendify::DEFAULT_MAX_NEIGHBORS.should == 50
      Recommendify::SimilarityMatrix.claass_variable_set(:@@max_neighbors, nil)
      sm = Recommendify::SimilarityMatrix.new
      sm.max_neighbors.should == 50
    end
      
    it "should remember max_neighbors if configured" do
      Recommendify::SimilarityMatrix.max_neighbors(23)
      sm = Recommendify::SimilarityMatrix.new      
      sm.max_neighbors.should == 23
    end

    it "should add an input_matrix by 'key'"

    it "should retrieve an input_matrix"

  end

  describe "process_item!" do

    it "should call similarities_for on each input_matrix"

    it "should add all similarities correctly"

    it "should add all similarities correctly with weight"

    it "should calculate the top max_neighbors items"

    it "should store the top max_neighbors items"

    it "should retrieve the top max_neighbors items after stored"

  end

  describe "process!" do

    it "should call process_item for all input_matrix.all_items's"

    it "should call process_item for all input_matrix.all_items's (uniquely)"

  end

end
