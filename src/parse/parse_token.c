#include "parse_token.h"

#include <stdio.h>

#include "parse_macros.h"

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

shttp_reqi shttp_parse_token_values_weighted(shttp_value_weighted *vals,
                                             shttp_u8 vals_len, const char *msg,
                                             shttp_reqi msg_len) {
  shttp_reqi off = 0;
  shttp_u8 i = 0;
  shttp_u16 n;
  shttp_reqi step;
  while(off < msg_len && msg[off] != '\r') {
    if(msg[off] == ' ' || msg[off] == ',') {
      off++;
      continue;
    }
    step = shttp_parse_token_cpy_until_or(
      vals[i].value, msg + off,
      MIN(LENGTH(vals[0].value), (shttp_reqi)(msg_len - off)), ",;\r");
    if(!step) return 0;
    off += step;
    if(msg[off] == ';' && SHTTP_CMP(msg + off, ";q=0.")) {
      off += sizeof(";q=0.") - 1;
      step = shttp_parse_token_number(&n, msg + off, 3);
      for(shttp_reqi j = 0; j < 3 - step; j++) {
        n *= 10;
      }
      vals[i].weight = SHTTP_WEIGHT_000 + n;
      off += step;
    }
    i++;
    if(i >= vals_len) break;
    while(msg[off] && msg[off] != ',' && msg[off] != '\r') {
      off++;
    }
  }
  while(msg[off] != '\r') {
    off++;
  }
  if(msg[off + 1] != '\n') return 0;
  return off + sizeof("\r\n") - 1;
}

shttp_reqi shttp_parse_token_values(shttp_value *vals, shttp_u8 vals_len,
                                    const char *msg, shttp_reqi msg_len) {
  shttp_reqi off = 0;
  shttp_u8 i = 0;
  shttp_reqi step;
  while(off < msg_len && msg[off] != '\r') {
    if(msg[off] == ' ' || msg[off] == ',') {
      off++;
      continue;
    }
    step = shttp_parse_token_cpy_until_or(
      vals[i].value, msg + off,
      MIN(LENGTH(vals[0].value), (shttp_reqi)(msg_len - off)), ",\r");
    if(!step) return 0;
    off += step;
    i++;
    if(i >= vals_len) break;
  }
  while(msg[off] != '\r') {
    off++;
  }
  if(msg[off + 1] != '\n') return 0;
  return off + sizeof("\r\n") - 1;
}

shttp_reqi shttp_parse_token_value(shttp_value *val, const char *msg,
                                   shttp_reqi msg_len) {
  shttp_reqi off = 0;
  shttp_reqi step;
  step = shttp_parse_token_cpy_until(
    val->value, msg + off, MIN(LENGTH(val->value), (shttp_reqi)(msg_len - off)),
    '\r');
  if(!step) return 0;
  off += step;
  if(msg[off + 1] != '\n') return 0;
  return off + sizeof("\r\n") - 1;
}
