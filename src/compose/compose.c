#include "compose.h"

#include <stdio.h>
#include <string.h>

#include "../macros.h"
#include "compose_token.h"

shttp_resi shttp_compose_version(char *msg, shttp_resi msg_len,
                                 const shttp_response *res) {
  if(msg_len < sizeof("HTTP/1.1")) return 0;
  shttp_compose_token_cpy(msg, "HTTP/", sizeof("HTTP/") - 1);
  switch(res->version) {
  case SHTTP_VERSION_1_0:
    shttp_compose_token_cpy(msg + sizeof("HTTP/") - 1, "1.0", sizeof("1.0"));
    break;
  case SHTTP_VERSION_1_1:
    shttp_compose_token_cpy(msg + sizeof("HTTP/") - 1, "1.1", sizeof("1.1"));
    break;
  case SHTTP_VERSION_2_0:
    shttp_compose_token_cpy(msg + sizeof("HTTP/") - 1, "2.0", sizeof("2.0"));
    break;
  case SHTTP_VERSION_3_0:
    shttp_compose_token_cpy(msg + sizeof("HTTP/") - 1, "3.0", sizeof("3.0"));
    break;
  default:
    return 0;
    break;
  }
  return sizeof("HTTP/1.1") - 1;
}

shttp_resi shttp_compose_code(char *msg, shttp_resi msg_len,
                              const shttp_response *res) {
  shttp_resi off = 0;
  shttp_u16 num = res->code;
  if(msg_len < 4) return 0;  // status code

  for(shttp_resi i = 2; i > 0; i--) {
    msg[i] = num % 10 + '0';
    num /= 10;
  }
  msg[0] = num % 10 + '0';
  off += 3;
  msg[off++] = ' ';

#define X(x)                                                   \
  case SHTTP_CODE_##x:                                         \
    if(sizeof(#x) > msg_len - off) {                           \
      printf("%lu %u\n", sizeof(#x), msg_len - off);           \
      return 0;                                                \
    }                                                          \
    off += shttp_compose_token_cpy(msg + off, #x, sizeof(#x)); \
    break;
  switch(res->code) { SHTTP_X_CODES }
#undef X

  return off;
}

shttp_resi shttp_compose_start_line(char *msg, shttp_resi msg_len,
                                    const shttp_response *res) {
  shttp_resi off = 0;
  shttp_resi step;
  step = shttp_compose_version(msg, msg_len, res);
  if(!step) return 0;
  off += step;
  if(off + 1 >= msg_len) return 0;
  msg[off++] = ' ';
  step = shttp_compose_code(msg + off, msg_len - off, res);
  if(!step) return 0;
  off += step;
  if(off + 2 >= msg_len) return 0;
  msg[off++] = '\r';
  msg[off++] = '\n';
  msg[off] = 0;
  return off;
}

shttp_resi shttp_compose_response(char *msg, shttp_resi msg_len,
                                  const shttp_response *res) {
  return shttp_compose_start_line(msg, msg_len, res);
}
