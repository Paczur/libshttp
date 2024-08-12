#include "parse.h"

#include <stdio.h>
#include <string.h>

#include "conf.h"
#include "macros.h"

#define PLACEHOLDER_MSG "HTTP/1.1 200 OK\n"

static shttp_u16 shttp_string_number(shttp_u16 *n, const char *msg,
                                     shttp_u16 len) {
  shttp_u16 res = 0;
  for(shttp_u16 i = 0; i < len; i++) {
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

static shttp_u16 shttp_token_cpy_until(char *token, const char *msg,
                                       shttp_u16 len, char end) {
  for(shttp_u16 i = 0; i < len; i++) {
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

static bool shttp_token_cmp_until(const char *token, const char *msg,
                                  shttp_u16 len, char end) {
  for(shttp_u16 i = 0; i < len; i++) {
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

static shttp_u16 shttp_parse_request_start_line(shttp_request *req,
                                                const char *msg,
                                                shttp_u16 msg_len) {
  shttp_u16 off;
  shttp_u16 step;
  step = shttp_parse_method(req, msg, msg_len);
  if(!step || !msg[step]) {
    puts("Malformed request: Method parse error");
    return 0;
  }
  step++;

  off = step;
  step = shttp_parse_path(req, msg + off, msg_len - off);
  if(!step || !msg[step]) {
    puts("Malformed request: Path parse error");
    return 0;
  }
  step++;

  off += step;
  step = shttp_parse_version(req, msg + off, msg_len - off);
  if(!step) {
    puts("Malformed request: Version parse error");
    return 0;
  }
  if(!msg[step] || !msg[step + 1]) return 0;
  return off + step + 2;
}

static shttp_u16 shttp_parse_header_host(shttp_request *req, const char *msg,
                                         shttp_u16 msg_len) {
  shttp_u16 off;
  shttp_u16 step;
  shttp_u16 port;
  if(!shttp_token_cmp("Host:", msg, msg_len)) return 0;

  off = sizeof("Host:") - 1;
  if(!msg[off]) return 0;
  off++;

  step = shttp_token_cpy_until(req->host_domain, msg + off, msg_len - off, ':');
  if(!step) return 0;
  if(msg[off + step] != '\r') {
    if(msg[off + step] != ':' || !msg[off + step + 1]) return 0;
    step++;
    off += step;
    step = shttp_string_number(&port, msg + off, msg_len - off);
    if(!step) return 0;
    req->host_port = port;
    req->has_host = true;
    return off + step;
  }
  req->host_port = 80;
  req->has_host = true;
  return off + step;
}

static shttp_u16 shttp_parse_request_header(shttp_request *req, const char *msg,
                                            shttp_u16 msg_len) {
  shttp_u16 step;
  shttp_u16 off = 0;
  while(msg[off] != '\r' && msg[off + 1] != '\n') {
    if(!req->has_host &&
       (step = shttp_parse_header_host(req, msg + off, msg_len - off))) {
      off += step;
      if(!msg[off] || !msg[off + 1]) {
        puts("Malformed request: Header parse error");
        return 0;
      }
      off += 2;
    } else {
      return off;
    }
  }
  return off + 2;
}

shttp_u16 shttp_parse_request(shttp_request *req, const char *msg,
                              shttp_u16 msg_len) {
  shttp_u16 off = 0;
  shttp_u16 step;
  step = shttp_parse_request_start_line(req, msg, msg_len);
  if(!step) {
    puts("Malformed Request: Error parsing start_line");
    return 0;
  }
  off += step;
  if(!msg[off]) return off;

  step = shttp_parse_request_header(req, msg + off, msg_len - off);
  if(!step) {
    puts("Malformed Request: Error parsing header");
    return 0;
  }
  off += step;
  if(!msg[step]) return off;
  return off;
}

shttp_u16 shttp_parse_response(char *msg, shttp_u16 msg_len,
                               const shttp_response *res) {
  (void)res;
  strncpy(msg, PLACEHOLDER_MSG, msg_len);
  return MIN(msg_len, sizeof(PLACEHOLDER_MSG));
}
