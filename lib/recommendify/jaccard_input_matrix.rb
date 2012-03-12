class Recommendify::JaccardInputMatrix < Recommendify::InputMatrix

  include Recommendify::CCMatrix

  def initialize(opts={})
    check_native if opts[:native]
    super(opts)
  end

  def similarity(item1, item2)
    calculate_jaccard_cached(item1, item2)
  end

  def similarities_for(item1)
    return run_native(item1) if @opts[:native]
    calculate_similarities(item1)
  end

private

  def calculate_similarities(item1)
    (all_items - [item1]).map do |item2|
      [item2, similarity(item1, item2)]
    end
  end

  def calculate_jaccard_cached(item1, item2)
    val = ccmatrix[item1, item2]
    val.to_f / (item_count(item1)+item_count(item2)-val).to_f
  end

  def calculate_jaccard(set1, set2)
    (set1&set2).length.to_f / (set1 + set2).uniq.length.to_f
  end

  def run_native(item_id)
    res = %x{#{native_path} --jaccard "#{redis_key}" "#{item_id}" "#{redis_url}"}
    raise "error: dirty exit (#{$?})" if $? != 0
    res.split("\n").map do |line|
      sim = line.match(/OUT: \(([^\)]*)\) \(([^\)]*)\)/)
      raise "error: #{res}" if !sim && !(res||"").include?('exit:')
      [sim[1], sim[2].to_f]
    end
  end

  def check_native
    return true if ::File.exists?(native_path)
    raise "recommendify_native not found - you need to run rake build_native first"
  end

  def native_path
    ::File.expand_path('../../../bin/recommendify', __FILE__)
  end

  def redis_url
    Recommendify.redis.client.location
  end

end
