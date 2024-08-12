#include "compose.h"

#include <string.h>

#include "../macros.h"

#define PLACEHOLDER_MSG "HTTP/1.1 200 OK\n"

shttp_resi shttp_compose_response(char *msg, shttp_resi msg_len,
                                  const shttp_response *res) {
  (void)res;
  strncpy(msg, PLACEHOLDER_MSG, msg_len);
  return MIN(msg_len, sizeof(PLACEHOLDER_MSG));
}
