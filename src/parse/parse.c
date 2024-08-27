#include "parse.h"

#include <stdio.h>
#include <string.h>

#include "../conf.h"
#include "parse_header.h"
#include "parse_macros.h"
#include "parse_slice.h"

static shttp_status shttp_parse_method(shttp_request req[static 1],
                                       shttp_slice msg[static 1]) {
#define X(x)                                                       \
  else if(SHTTP_CMP(STRINGIFY(x), *msg)) {                         \
    if((size_t)(msg->end - msg->begin) < sizeof(STRINGIFY(x)) - 1) \
      return SHTTP_STATUS_SLICE_END;                               \
    msg->begin += sizeof(STRINGIFY(x)) - 1;                        \
    req->method = SHTTP_METHOD_##x;                                \
    return SHTTP_STATUS_OK;                                        \
  }

  req->method = SHTTP_METHOD_LENGTH;
  if(0) {
  }
  SHTTP_X_METHODS
  return SHTTP_STATUS_VALUE_INVALID;

#undef X
}

static shttp_status shttp_parse_path(shttp_request req[static 1],
                                     shttp_slice msg[static 1]) {
  shttp_status status;
  if(msg->begin == msg->end) return SHTTP_STATUS_SLICE_END;
  req->path.begin = msg->begin;
  if((status = shttp_parse_slice_skip_until_space(msg))) return status;
  req->path.end = msg->begin;
  return SHTTP_STATUS_OK;
}

static shttp_status shttp_parse_version(shttp_request req[static 1],
                                        shttp_slice msg[static 1]) {
  if((size_t)(msg->end - msg->begin) < sizeof("HTTP/1.0") - 1)
    return SHTTP_STATUS_SLICE_END;
  if(!SHTTP_CMP("HTTP/", *msg)) return SHTTP_STATUS_PREFIX_INVALID;
  msg->begin += sizeof("HTTP/") - 1;

  if(SHTTP_CMP("1.0", *msg)) {
    req->version = SHTTP_VERSION_1_0;
  } else if(SHTTP_CMP("1.1", *msg)) {
    req->version = SHTTP_VERSION_1_1;
  } else if(SHTTP_CMP("2.0", *msg)) {
    req->version = SHTTP_VERSION_2_0;
  } else if(SHTTP_CMP("3.0", *msg)) {
    req->version = SHTTP_VERSION_3_0;
  } else {
    req->version = SHTTP_VERSION_LENGTH;
    return SHTTP_STATUS_VALUE_INVALID;
  }
  msg->begin += sizeof("1.0") - 1;
  return SHTTP_STATUS_OK;
}

static shttp_status shttp_parse_start_line(shttp_request req[static 1],
                                           shttp_slice msg[static 1]) {
  shttp_status status;
  if((status = shttp_parse_method(req, msg))) return status;
  if((status = shttp_parse_slice_space(msg))) return status;
  if((status = shttp_parse_path(req, msg))) return status;
  if((status = shttp_parse_slice_space(msg))) return status;
  if((status = shttp_parse_version(req, msg))) return status;
  if((status = shttp_parse_slice_newline(msg))) return status;
  return SHTTP_STATUS_OK;
}

static shttp_status shttp_parse_slices(shttp_request req[static 1],
                                       shttp_slice msg[static 1]) {
  shttp_status status;
  const char *t;
  if(msg->begin == msg->end) return SHTTP_STATUS_SLICE_END;
  req->headers.begin = msg->begin;
  while(1) {
    if((status = shttp_parse_slice_skip_past_newline(msg))) return status;
    t = msg->begin;
    if(t == msg->end) return SHTTP_STATUS_SLICE_END;
    if(!(status = shttp_parse_slice_newline(msg))) break;
  }
  req->headers.end = t;
  if(msg->begin == msg->end) return SHTTP_STATUS_OK;
  t = msg->begin;
  if((status = shttp_parse_slice_skip_until_newline(msg))) return status;
  msg->end = msg->begin;
  msg->begin = t;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_parse_request(shttp_request req[static 1],
                                 shttp_slice msg[static 1]) {
  shttp_status status;
  if((status = shttp_parse_start_line(req, msg))) return status;
  if((status = shttp_parse_slices(req, msg))) return status;
  return SHTTP_STATUS_OK;
}

