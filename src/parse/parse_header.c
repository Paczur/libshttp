#include "parse_header.h"

#include <stdio.h>

#include "parse_token.h"

#define SHTTP_CMP(m, t) shttp_parse_token_cmp((m), (t), sizeof(t) - 1)

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
  shttp_accept_encoding last;
  if(msg_len < sizeof("Accept-Encoding: ") - 1 ||
     !SHTTP_CMP(msg, "Accept-Encoding: "))
    return 0;
  off = sizeof("Accept-Encoding: ") - 1;
  while(msg[off] != '\r') {
    if(msg[off] == ',') {
      off++;
      if(msg[off] == ' ') off++;
      continue;
    } else if(SHTTP_CMP(msg + off, "gzip")) {
      last = SHTTP_ACCEPT_ENCODING_GZIP;
      off += sizeof("gzip") - 1;
      if(SHTTP_CMP(msg + off, ";q=")) {
        off += sizeof(";q=0.") - 1;
        req->accept_encoding_weight_gzip = msg[off] - '0';
        off++;
      }
    } else if(SHTTP_CMP(msg + off, "compress")) {
      last = SHTTP_ACCEPT_ENCODING_COMPRESS;
      off += sizeof("compress") - 1;
      if(SHTTP_CMP(msg + off, ";q=")) {
        off += sizeof(";q=0.") - 1;
        req->accept_encoding_weight_compress = msg[off] - '0';
        off++;
      }
    } else if(SHTTP_CMP(msg + off, "deflate")) {
      last = SHTTP_ACCEPT_ENCODING_DEFLATE;
      off += sizeof("deflate") - 1;
      if(SHTTP_CMP(msg + off, ";q=")) {
        off += sizeof(";q=0.") - 1;
        req->accept_encoding_weight_deflate = msg[off] - '0';
        off++;
      }
    } else if(SHTTP_CMP(msg + off, "br")) {
      last = SHTTP_ACCEPT_ENCODING_BR;
      off += sizeof("br") - 1;
      if(SHTTP_CMP(msg + off, ";q=")) {
        off += sizeof(";q=0.") - 1;
        req->accept_encoding_weight_br = msg[off] - '0';
        off++;
      }
    } else if(SHTTP_CMP(msg + off, "zstd")) {
      last = SHTTP_ACCEPT_ENCODING_ZSTD;
      off += sizeof("zstd") - 1;
      if(SHTTP_CMP(msg + off, ";q=")) {
        off += sizeof(";q=0.") - 1;
        req->accept_encoding_weight_zstd = msg[off] - '0';
        off++;
      }
    } else if(SHTTP_CMP(msg + off, "identity")) {
      last = SHTTP_ACCEPT_ENCODING_IDENTITY;
      off += sizeof("identity") - 1;
      if(SHTTP_CMP(msg + off, ";q=")) {
        off += sizeof(";q=0.") - 1;
        req->accept_encoding_weight_identity = msg[off] - '0';
        off++;
      }
    } else if(SHTTP_CMP(msg + off, "*")) {
      last = SHTTP_ACCEPT_ENCODING_STAR;
      off += sizeof("*") - 1;
      if(SHTTP_CMP(msg + off, ";q=")) {
        off += sizeof(";q=0.") - 1;
        req->accept_encoding_weight_star = msg[off] - '0';
        off++;
      }
    } else {
      puts("Malformed message: accept-encoding parse error");
      off++;
      continue;
    }
    req->accept_encoding |= last;
  }
  if(msg[off + 1] != '\n') return 0;
  off += sizeof("\r\n") - 1;
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
