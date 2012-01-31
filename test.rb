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



class UserRecommender < Recommendify::Base

  #max_neighbors 50

  input_matrix :visits, 
    :similarity_func => :jaccard
end


recommender = UserRecommender.new

buckets.each do |user_id, items|
  puts "#{user_id} -> #{items.join(",")}"  
  recommender.visits.add_set(user_id, items)
end

recommender.process!
