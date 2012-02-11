#include <stdio.h>
#include <stdio.h>
#include <hiredis/hiredis.h>

using namespace std;

int main(){
  redisContext *c;
  // redisReply *reply;

  struct timeval timeout = { 1, 500000 }; // 1.5 seconds  
  c = redisConnectWithTimeout("127.0.0.2", 6379, timeout);

  if (c->err) {
    printf("connection to redis failed: %s\n", c->errstr);
    return 1;
  }

  return 0;
}
