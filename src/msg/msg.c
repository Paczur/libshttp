#include "msg.h"

#include <assert.h>

#include "../slice/slice.h"
#include "../types.h"

#define SHTTP_STARTS_WITH(m, t) \
  shttp_slice_starts_with((m), (shttp_slice){(t), (t) + sizeof(t) - 1})
#define SHTTP_CPY(m, t) \
  shttp_slice_cpy((m), (shttp_slice){(t), (t) + sizeof(t) - 1})

SHTTP_UNUSED_RESULT static shttp_status method(shttp_request req[static 1],
                                               shttp_slice msg[static 1]) {
  SHTTP_ASSERT(req != NULL);
  SHTTP_ASSERT(msg != NULL);
  SHTTP_ASSERT(msg->begin <= msg->end);
#define X(x)                                             \
  else if(SHTTP_STARTS_WITH(*msg, #x)) {                 \
    if((size_t)(msg->end - msg->begin) < sizeof(#x) - 1) \
      return SHTTP_STATUS_SLICE_END;                     \
    msg->begin += sizeof(#x) - 1;                        \
    req->method = SHTTP_METHOD_##x;                      \
    return SHTTP_STATUS_OK;                              \
  }

  req->method = SHTTP_METHOD_LENGTH;
  if(0) {
  }
  SHTTP_X_METHODS
  return SHTTP_STATUS_VALUE_INVALID;

#undef X
}

SHTTP_UNUSED_RESULT static shttp_status path(shttp_request req[static 1],
                                             shttp_slice msg[static 1]) {
  SHTTP_ASSERT(req != NULL);
  SHTTP_ASSERT(msg != NULL);
  SHTTP_ASSERT(msg->begin <= msg->end);
  if(msg->begin == msg->end) return SHTTP_STATUS_SLICE_END;
  req->path.begin = msg->begin;
  SHTTP_PROP(shttp_slice_skip_until_space(msg));
  req->path.end = msg->begin;
  return SHTTP_STATUS_OK;
}

SHTTP_UNUSED_RESULT static shttp_status parse_version(
  shttp_request req[static 1], shttp_slice msg[static 1]) {
  SHTTP_ASSERT(req != NULL);
  SHTTP_ASSERT(msg != NULL);
  SHTTP_ASSERT(msg->begin <= msg->end);
  if((size_t)(msg->end - msg->begin) < sizeof("HTTP/1.0") - 1)
    return SHTTP_STATUS_SLICE_END;
  if(!SHTTP_STARTS_WITH(*msg, "HTTP/")) return SHTTP_STATUS_PREFIX_INVALID;
  msg->begin += sizeof("HTTP/") - 1;

  if(SHTTP_STARTS_WITH(*msg, "1.0")) {
    req->version = SHTTP_VERSION_1_0;
  } else if(SHTTP_STARTS_WITH(*msg, "1.1")) {
    req->version = SHTTP_VERSION_1_1;
  } else if(SHTTP_STARTS_WITH(*msg, "2.0")) {
    req->version = SHTTP_VERSION_2_0;
  } else if(SHTTP_STARTS_WITH(*msg, "3.0")) {
    req->version = SHTTP_VERSION_3_0;
  } else {
    req->version = SHTTP_VERSION_LENGTH;
    return SHTTP_STATUS_VALUE_INVALID;
  }
  msg->begin += sizeof("1.0") - 1;
  return SHTTP_STATUS_OK;
}

SHTTP_UNUSED_RESULT static shttp_status request_line(
  shttp_request req[static 1], shttp_slice msg[static 1]) {
  SHTTP_ASSERT(req != NULL);
  SHTTP_ASSERT(msg != NULL);
  SHTTP_ASSERT(msg->begin <= msg->end);
  SHTTP_PROP(method(req, msg));
  SHTTP_PROP(shttp_slice_parse_space(msg));
  SHTTP_PROP(path(req, msg));
  SHTTP_PROP(shttp_slice_parse_space(msg));
  SHTTP_PROP(parse_version(req, msg));
  return shttp_slice_parse_newline(msg);
}

SHTTP_UNUSED_RESULT static shttp_status slices(shttp_request req[static 1],
                                               shttp_slice msg[static 1]) {
  SHTTP_ASSERT(req != NULL);
  SHTTP_ASSERT(msg != NULL);
  SHTTP_ASSERT(msg->begin <= msg->end);
  shttp_status status;
  const char *t;
  if(msg->begin == msg->end) return SHTTP_STATUS_SLICE_END;
  req->headers.begin = msg->begin;
  while(1) {
    SHTTP_PROP(shttp_slice_skip_past_newline(msg));
    t = msg->begin;
    if(t == msg->end) return SHTTP_STATUS_SLICE_END;
    if(!(status = shttp_slice_parse_newline(msg))) break;
  }
  req->headers.end = t;
  if(msg->begin == msg->end) return SHTTP_STATUS_OK;
  t = msg->begin;
  SHTTP_PROP(shttp_slice_skip_until_newline(msg));
  msg->end = msg->begin;
  msg->begin = t;
  return SHTTP_STATUS_OK;
}

SHTTP_UNUSED_RESULT static shttp_status insert_version(
  shttp_mut_slice msg[static 1], const shttp_response res[static 1]) {
  SHTTP_ASSERT(res != NULL);
  SHTTP_ASSERT(msg != NULL);
  SHTTP_ASSERT(msg->begin <= msg->end);
  SHTTP_PROP(SHTTP_CPY(msg, "HTTP/"));
  switch(res->version) {
  case SHTTP_VERSION_1_0:
    SHTTP_PROP(SHTTP_CPY(msg, "1.0"));
    break;
  case SHTTP_VERSION_1_1:
    SHTTP_PROP(SHTTP_CPY(msg, "1.1"));
    break;
  case SHTTP_VERSION_2_0:
    SHTTP_PROP(SHTTP_CPY(msg, "2.0"));
    break;
  case SHTTP_VERSION_3_0:
    SHTTP_PROP(SHTTP_CPY(msg, "3.0"));
    break;
  default:
    return SHTTP_STATUS_VALUE_INVALID;
  }
  return SHTTP_STATUS_OK;
}

SHTTP_UNUSED_RESULT static shttp_status code(
  shttp_mut_slice msg[static 1], const shttp_response res[static 1]) {
  SHTTP_ASSERT(msg != NULL);
  SHTTP_ASSERT(res != NULL);
  SHTTP_ASSERT(msg->begin <= msg->end);
  char *restrict beginm = msg->begin;
  char *restrict const endm = msg->end;
  shttp_code num = res->code;

  if(endm - beginm < 3) {
    for(uint32_t i = endm - beginm; i < 3; i++) num /= 10;
    for(uint32_t i = endm - beginm; i > 0; i--) {
      beginm[i] = num % 10 + '0';
      num /= 10;
    }
    msg->begin = endm;
    return SHTTP_STATUS_SLICE_END;
  }

  for(char *restrict runm = beginm + 2; runm != beginm; runm--) {
    *runm = num % 10 + '0';
    num /= 10;
  }
  *beginm = num % 10 + '0';
  beginm += 3;
  if(beginm == endm) return SHTTP_STATUS_SLICE_END;
  *(beginm++) = ' ';
  msg->begin = beginm;

#define X(x)                        \
  case SHTTP_CODE_##x:              \
    SHTTP_PROP(SHTTP_CPY(msg, #x)); \
    break;
  switch(res->code) { SHTTP_X_CODES }
#undef X

  return SHTTP_STATUS_OK;
}

SHTTP_UNUSED_RESULT static shttp_resi status_line(
  shttp_mut_slice msg[static 1], const shttp_response res[static 1]) {
  SHTTP_ASSERT(msg != NULL);
  SHTTP_ASSERT(res != NULL);
  SHTTP_ASSERT(msg->begin <= msg->end);
  SHTTP_PROP(insert_version(msg, res));
  if(msg->begin == msg->end) return SHTTP_STATUS_SLICE_END;
  *(msg->begin++) = ' ';
  SHTTP_PROP(code(msg, res));
  SHTTP_PROP(shttp_slice_insert_newline(msg));
  return SHTTP_STATUS_OK;
}

shttp_status shttp_msg_request(shttp_request req[static 1],
                               shttp_slice msg[static 1]) {
  SHTTP_ASSERT(req != NULL);
  SHTTP_ASSERT(msg != NULL);
  SHTTP_ASSERT(msg->begin <= msg->end);
  SHTTP_PROP(request_line(req, msg));
  SHTTP_PROP(slices(req, msg));
  return SHTTP_STATUS_OK;
}

shttp_status shttp_msg_response(shttp_mut_slice msg[static 1],
                                const shttp_response res[static 1]) {
  char *restrict const beginm = msg->begin;
  SHTTP_ASSERT(msg->begin <= msg->end);
  SHTTP_PROP(status_line(msg, res));
  SHTTP_PROP(shttp_slice_cpy(msg, *(shttp_slice *)&res->headers));
  SHTTP_PROP(shttp_slice_insert_newline(msg));
  SHTTP_PROP(shttp_slice_cpy(msg, *(shttp_slice *)&res->body));
  msg->end = msg->begin;
  msg->begin = beginm;
  return SHTTP_STATUS_OK;
}
