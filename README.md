recommendify
============

Incremental and distributed item-based "Collaborative Filtering" for binary ratings with ruby and redis. In a nutshell: You feed in `user -> item` interactions and it spits out similarity vectors between items ("related items"). 

[ ![Build status - Travis-ci](https://secure.travis-ci.org/paulasmuth/recommendify.png) ](http://travis-ci.org/paulasmuth/recommendify)

### use cases

+ "Users that bought this product also bought...". 
+ "Users that viewed this video also viewed...". 
+ "Users that follow this person also follow...". 



### how it works

Recommendify keeps an incrementally updated `item x item` matrix, the "co-concurrency matrix". This matrix stores the number of times that a combination of two items has appeared in an interaction/preferrence set. The co-concurrence counts are processed with a similarity measure to retrieve another `item x item` similarity matrix, which is used to find the N most similar items for each item. This approach was described by Miranda, Alipio et al. [1]

1. Group the input user->item pairs by user-id and store them into interaction sets
2. For each item<->item combination in the interaction set increment the respective element in the co-concurrence matrix
3. For each item<->item combination in the co-concurrence matrix calculate the item<->item similarity
3. For each item store the N most similar items in the respective output set.


Fnord is not a draft!



usage
-----

You can add new interaction-sets to the processor incrementally, but the similarity matrix has to be manually re-processed after new interactions were added to any of the applied processors. However, the processing happens on-line and you can keep track of the changed items so you only have to re-calculate the changed rows of the matrix.

```ruby

# Our similarity matrix, we calculate the similarity via co-concurrence 
# of items in "orders" and the co-concurrence of items in user-likes using 
# two `item x item` matrices and the jaccard/cosine similarity measure.
class MyRecommender < Recommendify::Base

  # store a maximum of fifty neighbors per item
  max_neighbors 50

  # define an input data set "order_item_s". we'll add "order_id->item_id"
  # pairs to this input and use the jaccard coefficient to retrieve a 
  # "customers that ordered item i1 also ordered item i2" statement and apply
  # the result to the item<->item similarity matrix with a weight of 5.0
  input_matrix :order_items, 
    :similarity_func => :jaccard,
    :weight => 5.0
  
  # define an input data set "like_item_s". we'll add "user_id->item_id"
  # pairs to this input and use a cosine-based similarity measure to retrieve 
  # a "users that liked item i1 also liked item i2" statement and apply the 
  # result to the item<->item similarity matrix with a weight of 1.0
  input_matrix :like_items
    :similarity_func => :cosine,
    :weight => 1.0

end

recommender = MyRecommender.new

# add `order_id->item_id` interactions to the order_item_sim input
# you can add data incrementally and call RecommendedItem.process! to update
# the similarity matrix at any time.
recommender.order_items.add_set("order1", ["item23", "item65", "item23"])
recommender.order_items.add_set("order2", ["item14", "item23"])

# add `user_id->item_id` interactions to the like_time_sim input
recommender.like_items.add_set("user1", ["item23", "item65", "item23"])
recommender.like_items.add_set("user2", ["item14", "item23"])


# Calculate all elements of the similarity matrix
recommender.process!

# ...or calculate a specific row of the similarity matrix (a specific item)
# use this to avoid re-processing the whole matrix after incremental updates
recommender.process_item!("item65")


# retrieve similar items to "item23"
recommender.for("item23") 
  => [ <Recommendify::Neighbor item_id:"item65" similarity:0.23>, (...) ]


# remove "item23" from the similarity matrix and the input matrices. you should 
# do this if your items 'expire', since it will speed up the calculation
recommender.remove_item!("item23") 
```



### demo?

[ ![Example Results](https://raw.github.com/paulasmuth/recommendify/master/doc/example.png) ](http://falbala.23loc.com/~paul/recommendify_out_1.html)

full snippet: http://falbala.23loc.com/~paul/recommendify_out_1.html 

These recommendations were calculated from 2,3mb "profile visit"-data (taken from www.talentsuche.de). Initially processing the 120.047 `visitor_id->profile_id` pairs currently takes around half an hour on a single core and creates a 126.64mb hashtable in redis. You can try this for yourself; the complete data and code is in `doc/example.rb` and `doc/example_data.csv`.




### does it scale?

The maximum number of entries in the co-concurrence and similarity matrix is k(n) = (n^2)-(n/2), it grows O(n^2). However, in a real scenario it is very unlikely that all item<->item combinations appear in a interaction set and we use a sparse matrix which will only use memory for elemtens with a value > 0. The size of the similarity grows O(n). 





Sources / References
--------------------

[1] Miranda C. and Alipio J. (2008). Incremental collaborative ﬁltering for binary ratings (LIAAD - INESC Porto, University of Porto)

[2] George Karypis (2000) Evaluation of Item-Based Top-N Recommendation Algorithms (University of Minnesota, Department of Computer Science / Army HPC Research Center)

[3] Shiwei Z., Junjie W. Hui X. and Guoping X. (2011) Scaling up top-K cosine similarity search (Data & Knowledge Engineering 70)



License
-------

Copyright (c) 2011 Paul Asmuth

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to use, copy and modify copies of the Software, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.



### todo

+ rake benchmark CLASS=MySimilarityMatrix
+ optimize JaccardInputMatrix
+ implement CosineInputMatrix
+ implement NativeJaccardInputMatrix (C)
+ implement NativeCosineInputMatrix (C)
+ todo: remove item (remove from all matrices)
+ redis prefix issue
+ forbid ':' and '|' in item_ids
+ recommendify::base no key part issue
+ optimize sparsematrix memory usage (somehow)
+ make max_row length configurable
+ option: only add items where co-concurreny/appearnce-count > n

