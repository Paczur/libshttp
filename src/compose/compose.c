#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../private.h"
#include "../shttp.h"

#define SHTTP_CPY(m, t) \
  shttp_compose_slice_cpy((m), (shttp_slice){(t), (t) + sizeof(t) - 1})

SHTTP_UNUSED_RESULT static shttp_status shttp_compose_version(
  shttp_mut_slice msg[static 1], const shttp_response res[static 1]) {
  assert(res);
  assert(msg);
  assert(msg->begin <= msg->end);
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

SHTTP_UNUSED_RESULT static shttp_status shttp_compose_code(
  shttp_mut_slice msg[static 1], const shttp_response res[static 1]) {
  assert(msg);
  assert(res);
  assert(msg->begin <= msg->end);
  char *restrict beginm = msg->begin;
  char *restrict const endm = msg->end;
  shttp_code num = res->code;

  if(endm - beginm < 3) {
    for(shttp_u8 i = endm - beginm; i < 3; i++) num /= 10;
    for(shttp_u8 i = endm - beginm; i > 0; i--) {
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

SHTTP_UNUSED_RESULT static shttp_resi shttp_compose_start_line(
  shttp_mut_slice msg[static 1], const shttp_response res[static 1]) {
  assert(msg);
  assert(res);
  assert(msg->begin <= msg->end);
  SHTTP_PROP(shttp_compose_version(msg, res));
  if(msg->begin == msg->end) return SHTTP_STATUS_SLICE_END;
  *(msg->begin++) = ' ';
  SHTTP_PROP(shttp_compose_code(msg, res));
  SHTTP_PROP(shttp_compose_slice_newline(msg));
  return SHTTP_STATUS_OK;
}

shttp_status shttp_compose_slice_newline(shttp_mut_slice msg[static 1]) {
  assert(msg);
  assert(msg->begin <= msg->end);
  char *restrict beginm = msg->begin;
  char *restrict const endm = msg->end;
  if(beginm == endm) return SHTTP_STATUS_SLICE_END;
  *(beginm++) = '\r';
  if(beginm == endm) {
    msg->begin = beginm;
    return SHTTP_STATUS_SLICE_END;
  }
  *beginm = '\n';
  msg->begin = beginm + 1;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_compose_slice_cpy(shttp_mut_slice msg[static 1],
                                     shttp_slice slice) {
  assert(msg);
  assert(msg->begin <= msg->end);
  assert(slice.begin <= slice.end);
  char *restrict beginm = msg->begin;
  char *restrict const endm = msg->end;
  const char *restrict begint = slice.begin;
  const char *restrict const endt = slice.end;
  if(endm == beginm && endt != begint) return SHTTP_STATUS_SLICE_END;

  if(endm - beginm < endt - begint) {
    while(beginm != endm) {
      *beginm = *begint;
      beginm++;
      begint++;
    }
    msg->begin = beginm;
    return SHTTP_STATUS_SLICE_END;
  } else {
    while(begint != endt) {
      *beginm = *begint;
      beginm++;
      begint++;
    }
    msg->begin = beginm;
    return SHTTP_STATUS_OK;
  }
}

shttp_status shttp_compose_slice_cpy_char(shttp_mut_slice msg[static 1],
                                          char c) {
  assert(msg);
  assert(msg->begin <= msg->end);
  char *restrict beginm = msg->begin;
  char *restrict const endm = msg->end;
  if(endm == beginm) return SHTTP_STATUS_SLICE_END;
  *(beginm++) = c;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_compose_response(shttp_mut_slice msg[static 1],
                                    const shttp_response res[static 1]) {
  char *restrict const beginm = msg->begin;
  assert(msg->begin <= msg->end);
  SHTTP_PROP(shttp_compose_start_line(msg, res));
  SHTTP_PROP(shttp_compose_slice_cpy(msg, *(shttp_slice *)&res->headers));
  if(res->headers.begin != res->headers.end)
    SHTTP_PROP(shttp_compose_slice_newline(msg));
  SHTTP_PROP(shttp_compose_slice_cpy(msg, *(shttp_slice *)&res->body));
  msg->end = msg->begin;
  msg->begin = beginm;
  return SHTTP_STATUS_OK;
}
