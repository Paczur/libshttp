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

shttp_reqi shttp_parse_token_cpy_until(char *token, const char *msg,
                                       shttp_reqi len, char end) {
  for(shttp_reqi i = 0; i < len; i++) {
    if(!msg[i] || msg[i] == ' ' || msg[i] == '\n' || msg[i] == '\r' ||
       msg[i] == end) {
      token[i] = 0;
      return i;
    }
    token[i] = msg[i];
  }
  token[len - 1] = 0;
  return len - 1;
}

bool shttp_parse_token_cmp_until(const char *token, const char *msg,
                                 shttp_reqi len, char end) {
  for(shttp_reqi i = 0; i < len; i++) {
    if(!token[i] && (!msg[i] || msg[i] == ' ' || msg[i] == '\n' ||
                     msg[i] == '\r' || msg[i] == end)) {
      return true;
    }
    if(msg[i] != token[i]) {
      return false;
    }
  }
  return true;
}

shttp_reqi shttp_parse_token_cpy(char *token, const char *msg, shttp_reqi len) {
  for(shttp_reqi i = 0; i < len; i++) {
    if(!msg[i] || msg[i] == ' ' || msg[i] == '\n' || msg[i] == '\r') {
      token[i] = 0;
      return i;
    }
    token[i] = msg[i];
  }
  token[len - 1] = 0;
  return len - 1;
}

bool shttp_parse_token_cmp(const char *token, const char *msg, shttp_reqi len) {
  for(shttp_reqi i = 0; i < len; i++) {
    if(!token[i] &&
       (!msg[i] || msg[i] == ' ' || msg[i] == '\n' || msg[i] == '\r')) {
      return true;
    }
    if(msg[i] != token[i]) {
      return false;
    }
  }
  return true;
}
