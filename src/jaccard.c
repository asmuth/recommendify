void calculate_jaccard(char *item_id, int itemCount, struct cc_item *cc_items, int cc_items_size){
  int j, n;

  for(j = 0; j < cc_items_size; j++){
    n = cc_items[j].coconcurrency_count;
    if(n>0){
      cc_items[j].similarity = (
        (float)n / (
          (float)itemCount + 
          (float)cc_items[j].total_count - 
          (float)n
        )
      );
    } else {
      cc_items[j].similarity = 0.0;
    }
  }  

}
