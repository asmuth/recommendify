
class RecommendedProfiles < Recommendify::Base
end

RecommendedProfiles.reset_matrix!

buckets = Hash.new{ |h,k| h[k]=[] }

IO.read("/home/paul/talentsuche_visits.csv").split("\n").each do |l|
  user_id, item_id = l.split(",")
  buckets[user_id] << item_id
end

buckets.each do |user_id, items|
  puts "#{user_id} -> #{items.join(",")}"
  RecommendedProfiles.add_interactions(items)
end

RecommededProfiles.process!
