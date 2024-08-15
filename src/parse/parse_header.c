#include "parse_header.h"

#include <stdio.h>

#include "parse_macros.h"
#include "parse_token.h"

static shttp_reqi shttp_parse_header_host(shttp_request *req, const char *msg,
                                          shttp_reqi msg_len) {
  shttp_reqi off;
  shttp_reqi step;
  shttp_u16 port;
  if(msg_len < sizeof("Host: ") || !SHTTP_CMP(msg, "Host: ")) return 0;
  off = sizeof("Host: ") - 1;

  step = shttp_parse_token_cpy_until_or(req->host_domain, msg + off,
                                        msg_len - off, ":\r");
  if(!step) return 0;
  if(msg[off + step] != '\r') {
    if(msg[off + step] != ':' || !msg[off + step + 1]) return 0;
    step++;
    off += step;
    step = shttp_parse_token_number(&port, msg + off, msg_len - off);
    ;
    req->host_port = port;
    req->has_host = true;
    return off + step;
  }
  req->host_port = 80;
  req->has_host = true;
  return off + step;
}

static shttp_reqi shttp_parse_header_accept_encoding(shttp_request *req,
                                                     const char *msg,
                                                     shttp_reqi msg_len) {
  shttp_reqi off;
  if(msg_len < sizeof("Accept-Encoding: ") - 1 ||
     !SHTTP_CMP(msg, "Accept-Encoding: "))
    return 0;
  off = sizeof("Accept-Encoding: ") - 1;
  off += shttp_parse_token_value_weighted(req->accept_encoding,
                                          LENGTH(req->accept_encoding),
                                          msg + off, msg_len - off);
  return off;
}

shttp_reqi shttp_parse_header(shttp_request *req, const char *msg,
                              shttp_reqi msg_len) {
  shttp_reqi step;
  shttp_reqi off = 0;
  while(msg[off] != '\r' && msg[off + 1] != '\n') {
    if((!req->has_host &&
        (step = shttp_parse_header_host(req, msg + off, msg_len - off))) ||
       (step =
          shttp_parse_header_accept_encoding(req, msg + off, msg_len - off))) {
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
