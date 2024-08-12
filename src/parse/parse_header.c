#include "parse_header.h"

#include <stdio.h>

#include "parse_token.h"

static shttp_reqi shttp_parse_header_host(shttp_request *req, const char *msg,
                                          shttp_reqi msg_len) {
  shttp_reqi off;
  shttp_reqi step;
  shttp_u16 port;
  if(!shttp_parse_token_cmp("Host:", msg, msg_len)) return 0;

  off = sizeof("Host:") - 1;
  if(!msg[off]) return 0;
  off++;

  step = shttp_parse_token_cpy_until(req->host_domain, msg + off, msg_len - off,
                                     ':');
  if(!step) return 0;
  if(msg[off + step] != '\r') {
    if(msg[off + step] != ':' || !msg[off + step + 1]) return 0;
    step++;
    off += step;
    step = shttp_parse_token_number(&port, msg + off, msg_len - off);
    if(!step) return 0;
    req->host_port = port;
    req->has_host = true;
    return off + step;
  }
  req->host_port = 80;
  req->has_host = true;
  return off + step;
}

shttp_reqi shttp_parse_header(shttp_request *req, const char *msg,
                              shttp_reqi msg_len) {
  shttp_reqi step;
  shttp_reqi off = 0;
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
