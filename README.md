recommendify
============

Incremental and distributed item-based "Collaborative Filtering" with ruby and redis. In a nutshell: You feed in user->item interactions and it spits out similarity vectors between items ("related items"). 

Implementation in pure ruby *and* as a native extension. The coconcurrence/similarity calculation can be run as a distributed map/reduce. All is possible thanks to redis sorted sets. Fnord is not a draft!


use cases
---------

+ "Users that bought this product also bought...". 
+ "Users that viewed this video also viewed...". 
+ "Users that follow this person also follow...". 

etc.



synopsis
--------

```ruby

class RecommendedItem < RedisSimilarItems::Base

  # we'll use a blah blah vector similarity function
  distance_function :blah

end

# five actor/user_id->item pairs (e.g. view, sale)
interactions = {
  "user1" => ["item54", "item38", "item47"],
  "user2" => ["item65", "item54"]
}

# add the user->item interactions with a weight of 1.0
RecommendedItem.add_interactions(interactions["user1"], 1.0)
RecommendedItem.add_interactions(interactions["user2"], 1.0)

# calculate similar items for each item (distributed)
RecommendedItem.process!

# retrieve similar items to "item54"
RecommendedItem.for("item54") 
  => [ <RecommendedItem item_id:"item65" similarity:0.23>, (...) ]

```


does it scale?
--------------

The number of keys in the similarity matrix grows O(n^2) and and would result in a maximum of 2000001 million keys for 2 million products. However, in a real scenario it is very unlikely that all item<->item combinations appear in a interaction set.

The size of the computed nearest neighbors grows O(n). If we compute e.g. a maximum of 30 neighbors per product and assume no item_id is longer than 50 chars, then no set should be bigger than 50 + (50 * 30) = 2250bytes for the ids + (50 * 32) = 1600bytes for the scores, a total of 3850bytes + 25% redis overhead = 4812bytes per set. 

This means 2 million items will - in the worst case - require 2000000 * 2 * 4812bytes = 18,3 gigabyte of memory.


usage
-----

One thing recommendify *won't* do for you is grouping the interactions/preferences by actor/user_id.

```ruby

class RecommendedItem < Recommendify::Base

  # use the blah blah vector similarity function
  distance_function :blah

  # calculate a maximum of fifty neighbors per item
  max_neighbors 50

  # truncate user/actor rows to 200 items
  max_coconcurrent 200

end

# clear the coconcurrency matrix
RecommendedItem.reset_matrix!

# add interactions to the coconcurrency matrix with a weight of 1.0
RecommendedItem.add_interactions(interactions, 1.0)

# calculate the coconcurrency matrix and the nearest neighbors (not distributed)
RecommendedItem.process!

# queue the tasks for the coconcurrency matrix calculation (distributed)
RecommendedItem.process_matrix_async!

# queue the tasks for the nearest neighbors calculation (distributed)
RecommendedItem.process_neighbors_async!

# fetch a single task from the queue and process it
RecommendedItem.work!

# retrieve similar items
recommended_items = RecommendedItem.for(item_id) 

# get the similarity of a result
recommended_items.first.similarity

# get the item_id of a result
recommended_items.first.item_id

```


ideas
-----

+ rake benchmark CLASS=MySimilarityProcessor
+ NativeSimilarityProcessor (ruby_ext)



Sources / References
--------------------

[1] Sawar B., G. Karypis, Konstan J. and Riedel J. (2001). Item-Based Collaborative Filtering Recommendation Algorithms (GroupLens Research Group/Army HPC Research Center, University of Minnesota)

[2] Herlocker J., Konstan J. Terveen L. and Ried J. (2004) Evaluating Collaborative Filtering Recommender Systems (GroupLens Research Group/Army HPC Research Center, University of Minnesota)

[3] Schafer J.B, Konstan J. and Riedl J. (1999). Recommender Systems in E-Commerce (Proceedings of ACM E-Commerce 1999 conference)

[4] Adomavicius G. and Tuzhilin A. (2005). Towards the Next Generation of Recommender Systems: A Survey of the State-of-the-Art and Possible Extensions (IEEE Transactions on knowledge and data engineering)
