$: << ::File.expand_path("../lib", __FILE__)
require "recommendify"
require "redis"

# configure redis
Recommendify.redis = Redis.new

# load some test data
buckets = Hash.new{ |h,k| h[k]=[] }
IO.read("/home/paul/talentsuche_visits.csv").split("\n").each do |l|
  user_id, item_id = l.split(",")
  next if user_id.length == 0
  buckets[user_id] << item_id
end

def item_url(item_id)
  "http://www.talentsuche.de/u.#{item_id.gsub("profile_", "")}.FNORD.html"
end

class UserRecommender < Recommendify::Base

  #max_neighbors 50

  input_matrix :visits, 
    :similarity_func => :jaccard
end


recommender = UserRecommender.new

out_items = []
recommender.all_items[0..100].each do |item_id|
  out_recs = recommender.for(item_id)[0..5].map do |rec|
    <<-HTML
      <div class="rec">
        <span>#{rec.similarity}</span>
        <img src="#{item_url(rec.item_id)}" />
      </div>
    HTML
  end
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

exit!



buckets.each do |user_id, items|
  puts "#{user_id} -> #{items.join(",")}"  
  recommender.visits.add_set(user_id, items)
end

recommender.process!
