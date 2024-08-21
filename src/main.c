#include <stdio.h>

#include "shttp.h"

shttp_response res;
shttp_request req;

int main(void) {
  if(shttp_init()) return 1;
  while(true) {
    if(shttp_next(&req, 5000)) continue;
    shttp_response_to_request(&res, &req);
    shttp_send(&res);
  }
  shttp_deinit();
  return 0;
}
