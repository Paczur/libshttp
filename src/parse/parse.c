#include "parse.h"

#include <stdio.h>
#include <string.h>

#include "../conf.h"
#include "parse_header.h"
#include "parse_macros.h"
#include "parse_token.h"

static shttp_reqi shttp_parse_method(shttp_request *req, const char *msg,
                                     shttp_reqi msg_len) {
#define X(x)                                               \
  else if(SHTTP_CMP(msg, SHTTP_STRINGIFY(x))) {            \
    if(msg_len < sizeof(SHTTP_STRINGIFY(x)) - 1) return 0; \
    req->method = SHTTP_METHOD_##x;                        \
    return sizeof(SHTTP_STRINGIFY(x)) - 1;                 \
  }

  req->method = SHTTP_METHOD_LENGTH;
  if(0) {
  }
  SHTTP_X_METHODS
  return 0;

#undef X
}

static shttp_reqi shttp_parse_path(shttp_request *req, const char *msg,
                                   shttp_reqi msg_len) {
  return shttp_parse_token_cpy_until(req->path, msg, msg_len, ' ');
}

static shttp_reqi shttp_parse_version(shttp_request *req, const char *msg,
                                      shttp_reqi msg_len) {
  if(msg_len < sizeof("HTTP/1.0") - 1 || SHTTP_CMP("HTTP/", msg)) return 0;
  msg += sizeof("HTTP/") - 1;
  msg_len -= sizeof("HTTP/") - 1;

  if(SHTTP_CMP(msg, "1.0")) {
    req->version = SHTTP_VERSION_1_0;
  } else if(SHTTP_CMP(msg, "1.1")) {
    req->version = SHTTP_VERSION_1_1;
  } else if(SHTTP_CMP(msg, "2.0")) {
    req->version = SHTTP_VERSION_2_0;
  } else if(SHTTP_CMP(msg, "3.0")) {
    req->version = SHTTP_VERSION_3_0;
  } else {
    req->version = SHTTP_VERSION_LENGTH;
    return 0;
  }
  return sizeof("HTTP/1.0") - 1;
}

static shttp_reqi shttp_parse_start_line(shttp_request *req, const char *msg,
                                         shttp_reqi msg_len) {
  shttp_reqi off;
  shttp_reqi step;
  step = shttp_parse_method(req, msg, msg_len);
  if(!step || !msg[step]) return 0;
  step++;

  off = step;
  step = shttp_parse_path(req, msg + off, msg_len - off);
  if(!step || !msg[step]) return 0;
  step++;

  off += step;
  step = shttp_parse_version(req, msg + off, msg_len - off);
  if(!step) return 0;
  if(!msg[step] || !msg[step + 1]) return 0;
  return off + step + 2;
}

shttp_reqi shttp_parse_request(shttp_request *req, const char *msg,
                               shttp_reqi msg_len) {
  shttp_reqi off = 0;
  shttp_reqi step;
  step = shttp_parse_start_line(req, msg, msg_len);
  if(!step) {
    puts("Malformed Request: Error parsing start_line");
    return 0;
  }
  off += step;
  if(!msg[off]) return off;

  step = shttp_parse_header(req, msg + off, msg_len - off);
  if(!step) {
    puts("Malformed Request: Error parsing header");
    return 0;
  }
  off += step;
  if(!msg[step]) return off;
  return off;
}

