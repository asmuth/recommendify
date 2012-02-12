#ifndef CC_ITEM_H
#define CC_ITEM_H

#define ITEM_ID_SIZE 64

struct cc_item {         
  char  item_id[ITEM_ID_SIZE];
  int   coconcurrency_count;
  int   total_count;
  float similarity;  
};         

#endif
