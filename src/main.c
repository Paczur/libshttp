#include <stdio.h>

#include "http.h"

http_response_t res;
http_request_t req;

int main(void) {
  if(http_init()) return 1;
  while(1) {
    if(http_next(&req, 5000) == -1) continue;
    http_response_to_request(&res, &req);
    http_send(&res);
  }
  http_deinit();
  return 0;
}
