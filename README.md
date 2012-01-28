redis_similar_items
===================

Incremental, distributed item-based collaborative filtering. You feed in user -> item interactions and it spits out the "related items". 

Nearest neighbor implementation in pure ruby. This is possible thanks to redis sorted sets. Fnord is not a draft!


use cases
---------

+ "Users that bought this product also bought...". 
+ "Users that viewed this video also viewed...". 
+ "Users that follow this person also follow...". 

etc.



synopsis
--------

```
  class RecommendedItem < RedisSimilarItems::Model

    # we'll use a blah blah vector similarity function
    similarity_function :pearson

  end


  # track three "interactions" (e.g. view, sale) with a score of 1
  RecommendedItem.add_interaction("user123", "item123", 1.0)
  RecommendedItem.add_interaction("user123", "item645", 1.0)
  RecommendedItem.add_interaction("user563", "item123", 1.0)


  RecommendedItem.for("item123") 
    => [ <RecommendedItem item_id:"item645" similarity:0.23> ]

```



ideas
-----

+ rake benchmark CLASS=MySimilarityProcessor
+ NativeSimilarityProcessor (ruby_ext)



distributed similiarity matrix
------------------------------




memory usage
------------

number of keys for all possible combinations would be O(0 + 1 + 2 + 3 ... + n), which would result in
2000001 million pairs for 2 million products...

however, we compute e.g. a maximum number of 30 neighbors per product and each id is at most 50 chars long we can assume that no set should be bigger than 50 + (50 * 30) = 2250bytes for the ids + (50 * 32) = 1600bytes for the scores, so a total of 3850bytes + 25% redis overhead = 4812bytes per set. 

this means a similarity matrix of 2 million items will - in the worst case - require 2000000 * 2 * 4812bytes = 18,3 gigabyte of memory.




individual recommendations
--------------------------

    max(item.rank in item = ( preference.items in preference = ( preferences where user=user ) ) )




concept
-------

```                      
        [queue]   preferences: user_id, item_id, score
         
                  |
                  V
         
  [incremental]   item<->item similarity matrix (distributed)

                  |
                  V

  [sorted set]    item_m = { item_n1: similarity, item_n2: similarity (...) }

                  |
                  V

   [on demand]    items = max(item.similarity * item.rank)[0..max_items]

                  |
                  V

       [cache]    items => related products

```
