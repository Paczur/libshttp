#include "compose_token.h"

shttp_resi shttp_compose_token_cpy(char *dest, const char *token,
                                   shttp_resi token_len) {
  shttp_resi i = 0;
  for(; i < token_len; i++) {
    dest[i] = token[i];
  }
  return i - 1;
}

