class Recommendify::JaccardInputMatrix < Recommendify::InputMatrix

  include Recommendify::CCMatrix

  def initialize(opts={})
    super(opts)
  end

  def similarity(item1, item2)
    calculate_jaccard_cached(item1, item2)
  end

  # optimize: get all item-counts and the cc-row with 2 redis hmgets.
  # optimize: don't return more than sm.max_neighbors items (truncate set while collecting)
  def similarities_for(item1)
    # todo: optimize native. execute with own redis conn and write top K to stdout
    # native_ouput = %x{recommendify_native jaccard "#{redis_key}" "#{item1}"}
    # return native_output.split("\n").map{ |l| l.split(",") }
    (all_items - [item1]).map do |item2|
      [item2, similarity(item1, item2)]
    end
  end

private

  def calculate_jaccard_cached(item1, item2)
    val = @ccmatrix[item1, item2]
    val.to_f / (item_count(item1)+item_count(item2)-val).to_f
  end

  def calculate_jaccard(set1, set2)
    (set1&set2).length.to_f / (set1 + set2).uniq.length.to_f
  end

end