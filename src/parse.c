#include "parse.h"

#include <stdio.h>
#include <string.h>

#include "conf.h"
#include "macros.h"

#define PLACEHOLDER_MSG "HTTP/1.1 200 OK\n"

static shttp_u16 shttp_token_cpy(char *token, const char *msg, shttp_u16 len) {
  for(shttp_u16 i = 0; i < len; i++) {
    if(!msg[i] || msg[i] == ' ' || msg[i] == '\n' || msg[i] == '\r') {
      token[i] = 0;
      return i;
    }
    token[i] = msg[i];
  }
  token[len - 1] = 0;
  return len - 1;
}

static bool shttp_token_cmp(const char *token, const char *msg, shttp_u16 len) {
  for(shttp_u16 i = 0; i < len; i++) {
    if(!token[i] && (!msg[i] || msg[i] == ' ' || msg[i] == '\n') ||
       msg[i] == '\r') {
      return true;
    }
    if(msg[i] != token[i]) {
      return false;
    }
  }
  return true;
}

static shttp_method shttp_str2method(const char *buff) {
#define X(x)                                               \
  else if(!shttp_token_cmp(SHTTP_STRINGIFY(x), buff, 8)) { \
    return SHTTP_METHOD_##x;                               \
  }
  if(0) {
  }
  SHTTP_X_METHODS
#undef X
  return SHTTP_METHOD_LENGTH;
}

static void shttp_method2str(char *buff, shttp_method m) {
#define X(x)                              \
  case SHTTP_METHOD_##x:                  \
    strncpy(buff, SHTTP_STRINGIFY(x), 8); \
    break;
  switch(m) {
    SHTTP_X_METHODS
  default:
    break;
  }
#undef X
}

static shttp_u16 shttp_parse_method(shttp_request *req, const char *msg,
                                    shttp_u16 msg_len) {
#define X(x)                                                           \
  else if(shttp_token_cmp(SHTTP_STRINGIFY(x), msg,                     \
                          MIN(sizeof(SHTTP_STRINGIFY(x)), msg_len))) { \
    req->method = SHTTP_METHOD_##x;                                    \
    return sizeof(SHTTP_STRINGIFY(x)) - 1;                             \
  }

  req->method = SHTTP_METHOD_LENGTH;
  if(0) {
  }
  SHTTP_X_METHODS
  return 0;

#undef X
}

static shttp_u16 shttp_parse_path(shttp_request *req, const char *msg,
                                  shttp_u16 msg_len) {
  return shttp_token_cpy(req->path, msg, MIN(sizeof(req->path), msg_len));
}

static shttp_u16 shttp_parse_version(shttp_request *req, const char *msg,
                                     shttp_u16 msg_len) {
  if(strncmp("HTTP/", msg, MIN(sizeof("HTTP/") - 1, msg_len)) ||
     !msg[sizeof("HTTP/") - 1])
    return 0;
  msg += sizeof("HTTP/") - 1;
  msg_len -= sizeof("HTTP/") - 1;

  if(shttp_token_cmp("1.0", msg, MIN(sizeof("1.0"), msg_len))) {
    req->version = SHTTP_VERSION_1_0;
  } else if(shttp_token_cmp("1.1", msg, MIN(sizeof("1.1"), msg_len))) {
    req->version = SHTTP_VERSION_1_1;
  } else if(shttp_token_cmp("2.0", msg, MIN(sizeof("2.0"), msg_len))) {
    req->version = SHTTP_VERSION_2_0;
  } else if(shttp_token_cmp("3.0", msg, MIN(sizeof("3.0"), msg_len))) {
    req->version = SHTTP_VERSION_3_0;
  } else {
    req->version = SHTTP_VERSION_LENGTH;
    return 0;
  }
  return sizeof("HTTP/1.0") - 1;
}

void shttp_parse_request(shttp_request *req, const char *msg,
                         shttp_u16 msg_len) {
  shttp_u16 step;

  printf("%.*s", (int)msg_len, msg);
  step = shttp_parse_method(req, msg, msg_len);
  if(!step || !msg[step]) {
    puts("Malformed request: Method parse error");
    return;
  }
  step++;

  msg += step;
  msg_len -= step;
  step = shttp_parse_path(req, msg, msg_len);
  if(!step || !msg[step]) {
    puts("Malformed request: Path parse error");
    return;
  }
  step++;

  msg += step;
  msg_len -= step;
  step = shttp_parse_version(req, msg, msg_len);
  if(!step) {
    puts("Malformed request: Version parse error");
    return;
  }
  if(!msg[step] || !msg[step + 1]) return;
  step++;

  printf("%u %s %u\n", req->method, req->path, req->version);
}

shttp_u16 shttp_parse_response(char *msg, shttp_u16 msg_len,
                               const shttp_response *res) {
  (void)res;
  strncpy(msg, PLACEHOLDER_MSG, msg_len);
  return MIN(msg_len, sizeof(PLACEHOLDER_MSG));
}
