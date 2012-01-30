$: << ::File.expand_path("../lib", __FILE__)
require "recommendify"

class UserRecommender < Recommendify::Base

  #max_neighbors 50

  input_matrix :visits, 
    :similarity_func => :jaccard
end


buckets = Hash.new{ |h,k| h[k]=[] }

IO.read("/home/paul/talentsuche_visits.csv").split("\n").each do |l|
  user_id, item_id = l.split(",")
  buckets[user_id] << item_id
end

buckets.each do |user_id, items|
  next if user_id.length == 0
  puts "#{user_id} -> #{items.join(",")}"  
  UserRecommender.visits.add_set(user_id, items)
end

UserRecommender.process!
