recommendify
============

Incremental and distributed item-based "Collaborative Filtering" for binary ratings with ruby and redis. In a nutshell: You feed in `user -> item` interactions and it spits out similarity vectors between items ("related items"). 

### use cases

+ "Users that bought this product also bought...". 
+ "Users that viewed this video also viewed...". 
+ "Users that follow this person also follow...". 
etc.


### how it works

Recommendify keeps an incrementally updated `item x item` matrix, the "co-concurrency matrix". This matrix stores the number of times that a combination of two items has appeared in an interaction/preferrence set. The co-concurrence counts are processed with a consine distance/similarity measure to retrieve another `item x item` similarity matrix, which is used to find the N most similar items for each item. This approach was described by Miranda, Alipio et al. [6]

1. Group the input user->item pairs by user-id and store them into interaction sets
2. For each item<->item combination in the interaction increment the respective element in the co-concurrence matrix
3. For each item<->item combination in the co-concurrence matrix calculate (i,j): sim(i, j) = #(I∩J) / √#I×√#J 
3. For each item store the N most similar items in the respective output set.


Fnord is not a draft!



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

# process the matrix (distributed)
m.process!

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

The maximum number of entries in the co-concurrence and similarity matrix is k(n) = (n^2)-(n/2), it grows O(n^2) and and would result in a maximum of 2000001 million entries for 2 million items. However, in a real scenario it is very unlikely that all item<->item combinations appear in a interaction set and we use a sparse matrix which will only use memory for elemtens with a value > 0.

The size of the computed nearest neighbors grows O(n). If we compute e.g. a maximum of 30 neighbors per item and assume no item_id is longer than 50 chars, then no set should be bigger than 50 + (50 * 30) = 2250bytes for the ids + (50 * 32) = 1600bytes for the scores, a total of 3850bytes + 25% redis overhead = 4812bytes per set. 

This means 2 million items will - in the worst case - require 2000000 * 2 * 4812bytes = 18,3 gigabyte of memory for the output data.


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

[6] Miranda C. and Alipio J. (2008). Incremental collaborative ﬁltering for binary ratings (LIAAD - INESC Porto, University of Porto)


License
-------

Copyright (c) 2011 Paul Asmuth

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to use, copy and modify copies of the Software, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.