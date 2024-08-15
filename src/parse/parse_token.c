#include "parse_token.h"

shttp_reqi shttp_parse_token_number(shttp_u16 *n, const char *msg,
                                    shttp_reqi len) {
  shttp_reqi res = 0;
  for(shttp_reqi i = 0; i < len; i++) {
    if(msg[i] < '0' || msg[i] > '9') {
      *n = res;
      return i;
    }
    res *= 10;
    res += msg[i] - '0';
  }
  *n = res;
  return len - 1;
}

shttp_reqi shttp_parse_token_cpy(char *token, const char *msg, shttp_reqi len) {
  shttp_reqi i = 0;
  for(; i < len; i++) {
    token[i] = msg[i];
  }
  return i;
}

shttp_reqi shttp_parse_token_cpy_until(char *token, const char *msg,
                                       shttp_reqi len, char end) {
  for(shttp_reqi i = 0; i < len; i++) {
    if(!msg[i] || msg[i] == end) {
      token[i] = 0;
      return i;
    }
    token[i] = msg[i];
  }
  token[len] = 0;
  return len;
}

shttp_reqi shttp_parse_token_cpy_until_or(char *token, const char *msg,
                                          shttp_reqi len, const char *end) {
  for(shttp_reqi i = 0; i < len; i++) {
    if(!msg[i]) {
      token[i] = 0;
      return i;
    }
    for(shttp_reqi j = 0; end[j]; j++) {
      if(msg[i] == end[j]) {
        token[i] = 0;
        return i;
      }
    }
    token[i] = msg[i];
  }
  token[len] = 0;
  return len;
}

bool shttp_parse_token_cmp(const char *msg, const char *token, shttp_reqi len) {
  for(shttp_reqi i = 0; i < len; i++) {
    if(msg[i] != token[i]) return false;
  }
  return true;
}

bool shttp_parse_token_cmp_until(const char *token, const char *msg,
                                 shttp_reqi len, char end) {
  for(shttp_reqi i = 0; i < len; i++) {
    if(!token[i] && (!msg[i] || msg[i] == end)) return true;
    if(msg[i] != token[i]) return false;
  }
  return true;
}
