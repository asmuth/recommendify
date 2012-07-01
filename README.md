recommendify
============

_Recommendify is a ruby/redis based recommendation engine_  - The recommendations can be updated/processed incrementally and on multiple hosts. The worker is implemented in plain ruby and native C. 

[ ![Build status - Travis-ci](https://secure.travis-ci.org/paulasmuth/recommendify.png) ](http://travis-ci.org/paulasmuth/recommendify)

---

#### usecases

+ __"Users that bought this product also bought..."__ from `user_id--bought-->product_id` pairs
+ __"Users that viewed this video also viewed..."__ from `user_id--viewed-->video_id` pairs
+ __"Users that like this venue also like..."__ from `user_id--likes-->venue_id` pairs



synopsis
--------

Your input data (the so called interaction-sets) should look like this:

```
# FORMAT A: user bought products (select buyerid, productid from sales group_by buyerid)
[user23] product5 produt42 product17
[user42] product8 produt16 product5

# FORMAT B: user watched video (this can be transformed to the upper representation with a map/reduce)
user3 -> video3
user6 -> video19
user3 -> video6
user1 -> video42
```

The output data will look like this:

```
# similar products based on co-concurrent buys
product5 => product17 (0.78), product8 (0.43), product42 (0.31)
product17 => product5 (0.36), product8 (0.21), product42 (0.18)

# similar videos based on co-concurrent views
video19 => video3 (0.93), video6 (0.56), video42 (0.34)
video42 => video19 (0.32), video3 (0.21), video6 (0.08)
```

You can add new interaction-sets to the processor incrementally, but the similarities for changed items have to be re-processed after new interactions were added. You can either re-process all items (recommender.process!) from time to time or keep track of the updates and only process the changed items (recommender.process_item!)


usage
-----

```ruby

# Our similarity matrix, we calculate the similarity via co-concurrence 
# of products in "orders" using the jaccard similarity measure.
class MyRecommender < Recommendify::Base

  # store only the top fifty neighbors per item
  max_neighbors 50

  # define an input data set "order_items". we'll add "order_id->product_id"
  # pairs to this input and use the jaccard coefficient to retrieve a 
  # "customers that ordered item i1 also ordered item i2" statement and apply
  # the result to the item<->item similarity matrix with a weight of 5.0
  input_matrix :order_items,  
    # :native => true,
    :similarity_func => :jaccard,    
    :weight => 5.0

end

recommender = MyRecommender.new

# add `order_id->product_id` interactions to the order_item_sim input
# you can add data incrementally and call RecommendedItem.process! to update
# the similarity matrix at any time.
recommender.order_items.add_set("order1", ["product23", "product65", "productm23"])
recommender.order_items.add_set("order2", ["product14", "product23"])

# Calculate all elements of the similarity matrix
recommender.process!

# ...or calculate a specific row of the similarity matrix (a specific item)
# use this to avoid re-processing the whole matrix after incremental updates
recommender.process_item!("product65")

# retrieve similar products to "product23"
recommender.for("product23") 
  => [ <Recommendify::Neighbor item_id:"product65" similarity:0.23>, (...) ]

# remove "product23" from the similarity matrix and the input matrices. you should 
# do this if your items 'expire', since it will speed up the calculation
recommender.delete_item!("product23") 
```

### how it works

Recommendify keeps an incrementally updated `item x item` matrix, the "co-concurrency matrix". This matrix stores the number of times that a combination of two items has appeared in an interaction/preferrence set. The co-concurrence counts are processed with a jaccard similarity measure to retrieve another `item x item` similarity matrix, which is used to find the N most similar items for each item. This is also called "Item-based Collaborative Filtering with binary ratings" (see Miranda, Alipio et al. [1])

1. Group the input user->item pairs by user-id and store them into interaction sets
2. For each item<->item combination in the interaction set increment the respective element in the co-concurrence matrix
3. For each item<->item combination in the co-concurrence matrix calculate the item<->item similarity
3. For each item store the N most similar items in the respective output set.


### does it scale?

The maximum number of entries in the co-concurrence and similarity matrix is k(n) = (n^2)-(n/2), it grows O(n^2). However, in a real scenario it is very unlikely that all item<->item combinations appear in a interaction set and we use a sparse matrix which will only use memory for elemtens with a value > 0. The size of the similarity grows O(n). 

### native/fast worker

After you have compiled the native worker, you can pass the `:native => true` option to the input_matrix. This speeds up processing by at least 10x.

```
cd ~/.rvm/gems/ruby-1.9.3-p0/gems/recommendify-0.2.2/
bundle exec rake build_native
```

example
-------

These recommendations were calculated from 2,3mb "profile visit"-data (taken from www.talentsuche.de) - keep in mind that the recommender uses only visitor->visited data, it __doesn't know the gender__  of a user. 

[ ![Example Results](https://raw.github.com/paulasmuth/recommendify/master/doc/example.png) ](http://falbala.23loc.com/~paul/recommendify_out_1.html)

full snippet: http://falbala.23loc.com/~paul/recommendify_out_1.html 

Initially processing the 120.047 `visitor_id->profile_id` pairs currently takes around half an hour with the ruby-only implementation and ~130 seconds with the native/c implementation on a single core. It creates a 24.1mb hashtable in redis (with truncated user_rows a' max 100 items). In another real data set with very short user rows (purchase/payment data) it used only 3.4mb for 90k items with very good results. You can try this for yourself; the complete data and code is in `doc/example.rb` and `doc/example_data.csv`. 




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
