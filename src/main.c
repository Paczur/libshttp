

#include "conn.h"

typedef unsigned int uint;

int main(void) {
  if(conn_init()) return 1;
  conn_loop();
  conn_deinit();
  return 0;
}
