redis_similar_items
===================

Incremental, distributed item-based collaborative filtering. 

Nearest neighbor implementation in pure ruby. Fnord is not a draft!



use cases
---------

+ "Users that bought this product also bought...". 
+ "Users that viewed this video also viewed...". 
+ "Users that follow this person also follow...". 
etc.



synopsis
--------


  class SimilarItems < RedisSimilarItems::Model

    # we'll use a blah blah vector similarity function
    similarity_function :pearson

    # add the preference to the similarity map on insert
    process_on_insert

  end


  # track three "interactions" (e.g. view, sale) with a score of 1
  SimilarItems.add_interaction("user123", "item123", 1.0)
  SimilarItems.add_interaction("user123", "item645", 1.0)
  SimilarItems.add_interaction("user563", "item123", 1.0)


  SimilarItems.for("item123") 
    => [ <RedisSimilarItems::Result item_id:"item645" similarity:0.23> ]




concept
-------

      [on creation]   view, sale, pin, comment, heart, etc.
             
                      |
                      V
                      
    [offline queue]   preferences: user_id, item_id, score
             
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






distributed similiarity matrix
------------------------------




computational complexity
------------------------




memory usage
------------

number of keys for all possible combinations would be O(0 + 1 + 2 + 3 ... + n), which would result in
2000001 million keys for 2 million products....

so we compute a maximum number of 50 neighbors per product. if we assume each key would take up 128 bytes
in redis, that would mean (2000000 * 50 * 128) / (1024 ** 2) => 12,2 gigabyte of memory usage

with a maximum of 30 neighbors (and 128byte per key) we'd use 7,3 gigabyte

additionally, the second caching pass (2 million sorted sets for 2 million products) will need another 1,9g
of memory (if we assume a sorted set size of 1kbyte) 






fnord
-----

  up_score(product, user)     => {user-->views,pins,rates,sales<--product}
  p_rank(product)             => {product.gmv,total_saes;product.seller.gmv,total_sales}
  p_similarity(prod1, prod2)  => {prod1-->number_of_shared_cats<--prod2, same_seller, same_color}



data points
-----------
  
  product->product scores (discrete)
  user->product scores (not discrete)



aggregatables
-------------
  
  products[n].similar_products = [product1, product2, product3...]



individual recommendations
--------------------------

    max(item.rank in item = ( preference.items in preference = ( preferences where user=user ) ) )

