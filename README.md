recommendify
============

Incremental and distributed item-based "Collaborative Filtering" for binary ratings with ruby and redis. In a nutshell: You feed in user->item interactions and it spits out similarity vectors between items ("related items"). 

Fnord is not a draft!


use cases
---------

+ "Users that bought this product also bought...". 
+ "Users that viewed this video also viewed...". 
+ "Users that follow this person also follow...". 

etc.



usage
-----

```ruby

class RecommendedItem < Recommendify::Base

  # calculate a maximum of fifty neighbors per item
  max_neighbors 50

  # truncate user/actor rows to 200 items
  max_coconcurrent 200

end

# clear the similarity matrix
RecommendedItem.delete!

# ...or degrade the similarity matrix by 0.5
RecommendedItem.degrade!(0.5)

# create a new input/co-concurrence matrix
m = RecommendedItem.create_matrix

# add user->item interactions to the matrix
m.add_vector("user1", "item54")
m.add_vector("user1", "item38")
m.add_vector("user1", "item47")
m.add_vector("user2", "item65")
m.add_vector("user2", "item54")

# process the matrix (distributed)
m.process!

# apply the processed co-concurrence matrix to the similarity matrix with a score of 5
m.apply!(5)

# delete the co-concurrence matrix
m.delete!

# retrieve similar items to "item54"
RecommendedItem.for("item54") 
  => [ <RecommendedItem item_id:"item65" similarity:0.23>, (...) ]

# get the similarity of a result
RecommendedItem.for("item54").similarity
  => 0.23

# get the item_id of a result
RecommendedItem.for("item54").item_id
  => "item65"

```


does it scale?
--------------

The number of keys in the similarity matrix grows O(n^2) and and would result in a maximum of 2000001 million keys for 2 million items. However, in a real scenario it is very unlikely that all item<->item combinations appear in a interaction set.

The size of the computed nearest neighbors grows O(n). If we compute e.g. a maximum of 30 neighbors per item and assume no item_id is longer than 50 chars, then no set should be bigger than 50 + (50 * 30) = 2250bytes for the ids + (50 * 32) = 1600bytes for the scores, a total of 3850bytes + 25% redis overhead = 4812bytes per set. 

This means 2 million items will - in the worst case - require 2000000 * 2 * 4812bytes = 18,3 gigabyte of memory.


ideas
-----

+ rake benchmark CLASS=MySimilarityProcessor



Sources / References
--------------------

[1] Sawar B., G. Karypis, Konstan J. and Riedel J. (2001). Item-Based Collaborative Filtering Recommendation Algorithms (GroupLens Research Group/Army HPC Research Center, University of Minnesota)

[2] Herlocker J., Konstan J. Terveen L. and Ried J. (2004) Evaluating Collaborative Filtering Recommender Systems (GroupLens Research Group/Army HPC Research Center, University of Minnesota)

[3] Schafer J.B, Konstan J. and Riedl J. (1999). Recommender Systems in E-Commerce (Proceedings of ACM E-Commerce 1999 conference)

[4] Adomavicius G. and Tuzhilin A. (2005). Towards the Next Generation of Recommender Systems: A Survey of the State-of-the-Art and Possible Extensions (IEEE Transactions on knowledge and data engineering)

[5] Grcar M, Fortuna B. and Mladenic D. (2005). kNN Versus SVM in the Collaborative Filtering Framework (Jozef Stefan Institut, IST Programme of the European Community)



License
-------

Copyright (c) 2011 Paul Asmuth

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to use, copy and modify copies of the Software, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.