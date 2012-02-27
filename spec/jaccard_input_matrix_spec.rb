require ::File.expand_path('../spec_helper', __FILE__)

describe Recommendify::JaccardInputMatrix do

  before(:all) do
    @matrix = Recommendify::JaccardInputMatrix.new(:redis_prefix => "recommendify-test", :key => "mymatrix")
  end

  before(:each) do
    flush_redis!
  end

  it_should_behave_like Recommendify::InputMatrix
  it_should_behave_like Recommendify::CCMatrix

  it "should calculate the correct jaccard index" do
    @matrix.send(:calculate_jaccard, 
      ["foo", "bar", "fnord", "blubb"],
      ["bar", "fnord", "shmoo", "snafu"]
    ).should == 2.0/6.0
  end

  it "should calculate the correct similarity between two items" do
    add_two_item_test_data!(@matrix)
    # sim(fnord,blubb) = (users(fnord) & users(blub)) / (users(fnord) + users(blubb))
    # => {user42 user48} / {user42 user46 user48 user50} + {user42 user44 user48}
    # => {user42 user48} / {user42 user44 user46 user48 user50}
    # => 2 / 5 => 0.4
    @matrix.similarity("fnord", "blubb").should == 0.4
    @matrix.similarity("blubb", "fnord").should == 0.4
  end

  it "should calculate all similarities for an item (1/3)" do
    add_three_item_test_data!(@matrix)
    res = @matrix.similarities_for("fnord")
    res.length.should == 2
    res.should include ["shmoo", 0.75]
    res.should include ["blubb", 0.4]
  end

  it "should calculate all similarities for an item (2/3)" do
    add_three_item_test_data!(@matrix)
    res = @matrix.similarities_for("shmoo")
    res.length.should == 2
    res.should include ["blubb", 0.2]
    res.should include ["fnord", 0.75]
  end


  it "should calculate all similarities for an item (3/3)" do
    add_three_item_test_data!(@matrix)
    res = @matrix.similarities_for("blubb")
    res.length.should == 2
    res.should include ["shmoo", 0.2]
    res.should include ["fnord", 0.4]
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


