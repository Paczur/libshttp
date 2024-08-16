#include "parse_header.h"

#include <stdio.h>

#include "parse_macros.h"
#include "parse_token.h"

#define SHTTP_VALUES_WEIGHTED(vals, msg, msg_len, token)                      \
  shttp_parse_header_values_weighted(vals, LENGTH(vals), msg, msg_len, token, \
                                     sizeof(token) - 1)

#define SHTTP_VALUES(vals, msg, msg_len, token)                      \
  shttp_parse_header_values(vals, LENGTH(vals), msg, msg_len, token, \
                            sizeof(token) - 1)

#define SHTTP_VALUE(val, msg, msg_len, token) \
  shttp_parse_header_value(val, msg, msg_len, token, sizeof(token) - 1)

#define SHTTP_PARSE_HEADER(name) \
  (step = shttp_parse_header_##name(req, msg + off, msg_len - off))

static shttp_reqi shttp_parse_header_host(shttp_request *req, const char *msg,
                                          shttp_reqi msg_len) {
  shttp_reqi off;
  shttp_reqi step;
  shttp_u16 port;
  if(msg_len < sizeof("Host: ") || !SHTTP_CMP(msg, "Host: ")) return 0;
  off = sizeof("Host: ") - 1;

  step = shttp_parse_token_cpy_until_or(req->host_domain, msg + off,
                                        msg_len - off, ":\r\n");
  if(!step) return 0;
  if(msg[off + step] != '\r' && msg[off + step] != '\n') {
    if(msg[off + step] != ':' || !msg[off + step + 1]) return 0;
    step++;
    off += step;
    step = shttp_parse_token_number(&port, msg + off, msg_len - off);
    req->host_port = port;
    if(msg[off + step] == '\r') step++;
    if(msg[off + step] != '\n') return 0;
    return off + step + 1;
  }
  req->host_port = 80;
  if(msg[off + step] == '\r') step++;
  if(msg[off + step] != '\n') return 0;
  return off + step + 1;
}

static shttp_reqi shttp_parse_header_values_weighted(
  shttp_value_weighted *vals, shttp_u8 vals_len, const char *msg,
  shttp_reqi msg_len, const char *token, shttp_reqi token_len) {
  shttp_reqi step;
  if(msg_len < token_len || !shttp_parse_token_cmp(msg, token, token_len))
    return 0;
  step = shttp_parse_token_values_weighted(vals, vals_len, msg + token_len,
                                           msg_len - token_len);
  if(!step) return 0;
  return token_len + step;
}

static shttp_reqi shttp_parse_header_values(shttp_value *vals,
                                            shttp_u8 vals_len, const char *msg,
                                            shttp_reqi msg_len,
                                            const char *token,
                                            shttp_reqi token_len) {
  shttp_reqi step;
  if(msg_len < token_len || !shttp_parse_token_cmp(msg, token, token_len))
    return 0;
  step = shttp_parse_token_values(vals, vals_len, msg + token_len,
                                  msg_len - token_len);
  if(!step) return 0;
  return token_len + step;
}

static shttp_reqi shttp_parse_header_value(shttp_value *val, const char *msg,
                                           shttp_reqi msg_len,
                                           const char *token,
                                           shttp_reqi token_len) {
  shttp_reqi step;
  if(msg_len < token_len || !shttp_parse_token_cmp(msg, token, token_len))
    return 0;
  step = shttp_parse_token_value(val, msg + token_len, msg_len - token_len);
  if(!step) return 0;
  return token_len + step;
}

#define X(name, token)                                            \
  static shttp_reqi shttp_parse_header_##name(                    \
    shttp_request *req, const char *msg, shttp_reqi msg_len) {    \
    return SHTTP_VALUES_WEIGHTED(req->name, msg, msg_len, token); \
  }
SHTTP_X_REQUEST_HEADERS_VALUES_WEIGHTED
#undef X

#define X(name, token)                                         \
  static shttp_reqi shttp_parse_header_##name(                 \
    shttp_request *req, const char *msg, shttp_reqi msg_len) { \
    return SHTTP_VALUES(req->name, msg, msg_len, token);       \
  }
SHTTP_X_REQUEST_HEADERS_VALUES
#undef X

#define X(name, token)                                         \
  static shttp_reqi shttp_parse_header_##name(                 \
    shttp_request *req, const char *msg, shttp_reqi msg_len) { \
    return SHTTP_VALUE(&req->name, msg, msg_len, token);       \
  }
SHTTP_X_REQUEST_HEADERS_VALUE
#undef X

#define X(name, token) || SHTTP_PARSE_HEADER(name)
shttp_reqi shttp_parse_header(shttp_request *req, const char *msg,
                              shttp_reqi msg_len) {
  shttp_reqi step;
  shttp_reqi off = 0;
  while(msg[off] != '\r' && msg[off] != '\n') {
    if(SHTTP_PARSE_HEADER(host)
         SHTTP_X_REQUEST_HEADERS_VALUES_WEIGHTED SHTTP_X_REQUEST_HEADERS_VALUES
           SHTTP_X_REQUEST_HEADERS_VALUE) {
      off += step;
    } else {
      while(msg[off] != '\n') off++;
      off++;
    }
  }
  if(msg[off] == '\r') off++;
  if(msg[off] != '\n') return 0;
  return off + 1;
}
#undef X
