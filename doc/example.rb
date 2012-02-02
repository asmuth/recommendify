$: << ::File.expand_path("../../lib", __FILE__)
require "recommendify"
require "redis"

# configure redis
Recommendify.redis = Redis.new

# our recommender class
class UserRecommender < Recommendify::Base

  #max_neighbors 50

  input_matrix :visits, 
    :similarity_func => :jaccard
end

recommender = UserRecommender.new

# load some test data
buckets = Hash.new{ |h,k| h[k]=[] }
IO.read("example_data.csv").split("\n").each do |l|
  user_id, item_id = l.split(",")
  next if user_id.length == 0
  buckets[user_id] << item_id
end

# add the test data to the recommender
buckets.each do |user_id, items|
  puts "#{user_id} -> #{items.join(",")}"  
  recommender.visits.add_set(user_id, items)
end

# process all items (equivalent to recommender.process!)
num_items = (all_items = recommender.all_items).size
all_items.each_with_index do |item_id, n|
  puts "processing #{item_id} (#{n}/#{num_items})"
  recommender.process_item!(item_id)
end


# generate a html page
def item_url(item_id)
  "http://#{ENV["URL"]}/u.#{item_id.gsub("profile_", "")}.FNORD.html"
end

out_items = []
recommender.all_items.shuffle[0..2000].each do |item_id|
  out_recs = recommender.for(item_id)[0..5].map do |rec|
    <<-HTML
      <div class="rec">
        <span>#{rec.similarity}</span>
        <img src="#{item_url(rec.item_id)}" />
      </div>
    HTML
  end
  next if out_recs.length < 5
  out_items << <<-HTML
    <div class="item">
      <img src="#{item_url(item_id)}" class="item" />
      #{out_recs.join("\n")}
      <br class="clear" />
    </div>
  HTML
end

out = <<-HTML
<!DOCTYPE html>
<html>
  <head>
    <title>foo</title>
    <style>
      .item{ padding:50px; border-bottom:1px dotted #333; }
      .item img.item{ float:left; margin-right:30px; }
      .item .rec{ float:left; width:130px; overflow:hidden; }
      .clear{ clear:both; }
    </style>
  </head>
  <body>
    #{out_items.join("\n")}
  </body>
</html>
HTML

File.open('/tmp/recommendify_out_1.html', "w+"){ |f| f.write(out) }
