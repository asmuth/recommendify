#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <hiredis/hiredis.h>

#include "cc_item.h" 
#include "cosine.h"
#include "iikey.h"
#include "jaccard.h"
#include "output.h"

#define PARAM_OFFSET 2

int main(int argc, char **argv){
  int similarityFunc = 0;
  size_t itemCount = 0;
  char *itemID;  
  char *redisPrefix;
  redisContext *c;
  redisReply *all_items;
  redisReply *reply;
  size_t cur_batch_size;
  char* cur_batch;

	char const *redisHost = "127.0.0.1";
	unsigned short redisPort = 6379;

  size_t batch_size = 200; /* FIXPAUL: make option */
  size_t maxItems = 50; /* FIXPAUL: make option */
  
  
  /* option parsing */
  if(argc < 2) {
    print_usage(stderr, argv[0]);
    return EXIT_FAILURE;
  }

	int argi;
	for (argi = 1; argi < argc - PARAM_OFFSET; ++argi) {
		if (!strcmp(argv[argi], "--version")) {
			print_version();
			return EXIT_SUCCESS;
		}
		else if (!strcmp(argv[argi], "--jaccard"))
			similarityFunc = 1;
		else if (!strcmp(argv[argi], "--cosine"))
			similarityFunc = 2;

		else if (!strcmp(argv[argi], "--host")) {
			if (argi + 1 < argc - PARAM_OFFSET) {
				fputs("--host requires a host name as parameter", stderr);
				return EXIT_FAILURE;
			}

			redisHost = argv[++argi];
		}

		else if (!strcmp(argv[argi], "--port")) {
			if (argi + 1 < argc - PARAM_OFFSET) {
				fputs("--port requires a port number as parameter", stderr);
				return EXIT_FAILURE;
			}

			errno = 0;
			unsigned long port = strtoul(argv[++argi], NULL, 0);
			if (errno) {
				fprintf(stderr, "Invalid port number %s: %s\n",
					argv[argi], strerror(errno));
				return EXIT_FAILURE;
			}

			if (port > USHRT_MAX) {
				fprintf(stderr, "Port number %lu out of range\n", port);
				return EXIT_FAILURE;
			}

			redisPort = port;
		}

		else if (!strcmp(argv[argi], "--help")) {
			print_usage(stdout, argv[0]);
			return EXIT_SUCCESS;
		}

		else {
			fprintf(stderr, "Invalid option: %s\n", argv[argi]);
			return EXIT_FAILURE;
		}
	}

	if (!similarityFunc) {
		fputs("No similarity function specified\n", stderr);
		return EXIT_FAILURE;
	}

  redisPrefix = argv[argi + 1];
  itemID = argv[argi + 2];


  /* connect to redis */
  struct timeval timeout = { 1, 500000 }; 
  c = redisConnectWithTimeout(redisHost, redisPort, timeout); 
  if (!c || c->err) {
    fprintf(stderr, "Connection to redis failed: %s\n",
      c ? c->errstr : "Broken by design");
    return EXIT_FAILURE;
  }
 

  /* get item count */
  reply = redisCommand(c,"HGET %s:items %s", redisPrefix, itemID);    
  itemCount = atoi(reply->str);    
  freeReplyObject(reply);

  if(itemCount == 0){
    fputs("Item count is zero\n", stderr);
    return EXIT_SUCCESS;
  }
  

  /* get all items_ids and the total counts */ 
  all_items = redisCommand(c,"HGETALL %s:items", redisPrefix);

  if (all_items->type != REDIS_REPLY_ARRAY)
    return EXIT_FAILURE;


  /* populate the cc_items array */ 
  size_t cc_items_size = all_items->elements / 2;  
  size_t cc_items_mem = cc_items_size * sizeof(struct cc_item);
  struct cc_item *cc_items = malloc(cc_items_mem);
  cc_items_size--;

  if(!cc_items){    
    fprintf(stderr, "Cannot allocate %lu bytes of memory: %s",
      cc_items_mem, strerror(errno));
    return EXIT_FAILURE;
  }
  
  for (size_t i = 0; i < all_items->elements/2; ++i){                   
    if(strcmp(itemID, all_items->element[i*2]->str) != 0){      
      strncpy(cc_items[i].item_id, all_items->element[i*2]->str, ITEM_ID_SIZE);
      cc_items[i].total_count = atoi(all_items->element[i*2+1]->str);
    }
  }

  freeReplyObject(all_items);


  // batched redis hmgets on the ccmatrix
  cur_batch = malloc(batch_size * (ITEM_ID_SIZE + 4) * 2);

  if(!cur_batch){    
    perror("Cannot allocate memory");
    return EXIT_FAILURE;
  }

  size_t n;
  for (n = cc_items_size; n >= 0; n -= batch_size) {
    cur_batch_size = ((n-1 < batch_size) ? n-1 : batch_size);

    size_t cur_batch_off = 0;

    for (size_t i = 0; i < cur_batch_size; i++) {
      cur_batch_off +=
        item_item_key(cur_batch + cur_batch_off, itemID, cc_items[n-i].item_id);
      cur_batch[cur_batch_off - 1] = ' ';
    }

    redisAppendCommand(c, "HMGET %s:ccmatrix %s", redisPrefix, cur_batch);  
    redisGetReply(c, (void**)&reply);
      
    for (size_t j = 0; j < reply->elements; j++){
      if(reply->element[j]->str){
        cc_items[n-j].coconcurrency_count = atoi(reply->element[j]->str);
      } else {
        cc_items[n-j].coconcurrency_count = 0;
      }   
    }

    freeReplyObject(reply);
  }

  free(cur_batch);



  /* calculate similarities */
  if(similarityFunc == 1)
    calculate_jaccard(itemID, itemCount, cc_items, cc_items_size);

  if(similarityFunc == 2)
    calculate_cosine(itemID, itemCount, cc_items, cc_items_size);

  
  /* find the top x items with simple bubble sort */
  for(size_t i = 0; i < maxItems - 1; ++i){
    n -= batch_size;
    for (size_t j = 0; j < cc_items_size - i - 1; ++j){
      if (cc_items[j].similarity > cc_items[j + 1].similarity){
        struct cc_item tmp = cc_items[j];
        cc_items[j] = cc_items[j + 1];
        cc_items[j + 1] = tmp;
      }
    }
  }


  /* print top k items */
  size_t m = ((cc_items_size < maxItems) ? cc_items_size : maxItems);
  for(size_t j = 0; j < m; j++){
    size_t i = cc_items_size-j-1;
    if(cc_items[i].similarity > 0){
      print_item(cc_items + i);      
    }    
  }


  free(cc_items);
  return EXIT_SUCCESS;
}


