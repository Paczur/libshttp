#include "parse.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../conf.h"
#include "../private.h"

#define SHTTP_CMP(t, m) \
  shttp_parse_slice_cmp((shttp_slice){(t), (t) + sizeof(t) - 1}, (m))

SHTTP_UNUSED_RESULT static shttp_status shttp_parse_method(
  shttp_request req[static 1], shttp_slice msg[static 1]) {
  assert(req);
  assert(msg);
  assert(msg->begin <= msg->end);
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

SHTTP_UNUSED_RESULT static shttp_status shttp_parse_path(
  shttp_request req[static 1], shttp_slice msg[static 1]) {
  assert(req);
  assert(msg);
  assert(msg->begin <= msg->end);
  shttp_status status;
  if(msg->begin == msg->end) return SHTTP_STATUS_SLICE_END;
  req->path.begin = msg->begin;
  if((status = shttp_parse_slice_skip_until_space(msg))) return status;
  req->path.end = msg->begin;
  return SHTTP_STATUS_OK;
}

SHTTP_UNUSED_RESULT static shttp_status shttp_parse_version(
  shttp_request req[static 1], shttp_slice msg[static 1]) {
  assert(req);
  assert(msg);
  assert(msg->begin <= msg->end);
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

SHTTP_UNUSED_RESULT static shttp_status shttp_parse_start_line(
  shttp_request req[static 1], shttp_slice msg[static 1]) {
  assert(req);
  assert(msg);
  assert(msg->begin <= msg->end);
  shttp_status status;
  if((status = shttp_parse_method(req, msg))) return status;
  if((status = shttp_parse_slice_space(msg))) return status;
  if((status = shttp_parse_path(req, msg))) return status;
  if((status = shttp_parse_slice_space(msg))) return status;
  if((status = shttp_parse_version(req, msg))) return status;
  if((status = shttp_parse_slice_newline(msg))) return status;
  return SHTTP_STATUS_OK;
}

SHTTP_UNUSED_RESULT static shttp_status shttp_parse_slices(
  shttp_request req[static 1], shttp_slice msg[static 1]) {
  assert(req);
  assert(msg);
  assert(msg->begin <= msg->end);
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

shttp_status shttp_parse_slice_newline(shttp_slice s[static 1]) {
  assert(s);
  assert(s->begin <= s->end);
  const char *restrict begin = s->begin;
  const char *restrict const end = s->end;
  if(begin == end) return SHTTP_STATUS_SLICE_END;
  if(*begin == '\r') begin++;
  if(begin == end) {
    s->begin = begin;
    return SHTTP_STATUS_SLICE_END;
  }
  if(*begin != '\n') {
    s->begin = begin;
    return SHTTP_STATUS_NEWLINE_EXPECTED;
  }
  begin++;
  s->begin = begin;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_parse_slice_space(shttp_slice s[static 1]) {
  assert(s);
  assert(s->begin <= s->end);
  const char *restrict const begin = s->begin;
  const char *restrict const end = s->end;
  if(begin == end) return SHTTP_STATUS_SLICE_END;
  if(*begin != ' ') return SHTTP_STATUS_SPACE_EXPECTED;
  s->begin++;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_parse_slice_space_optional(shttp_slice s[static 1]) {
  assert(s);
  assert(s->begin <= s->end);
  const char *restrict const begin = s->begin;
  const char *restrict const end = s->end;
  if(begin == end) return SHTTP_STATUS_SLICE_END;
  if(*begin == ' ') s->begin++;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_parse_slice_number(shttp_u16 out[static 1],
                                      shttp_slice s[static 1]) {
  assert(out);
  assert(s);
  assert(s->begin <= s->end);
  const char *restrict begin = s->begin;
  const char *restrict const end = s->end;
  if(begin == end) return SHTTP_STATUS_SLICE_END;
  if(*begin > '9' || *begin < '0') return SHTTP_STATUS_VALUE_INVALID;
  shttp_u16 n = 0;
  while(begin != end && *begin <= '9' && *begin >= '0') {
    n *= 10;
    n += *begin - '0';
    begin++;
  }
  s->begin = begin;
  *out = n;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_parse_slice_skip(shttp_slice s[static 1], shttp_u16 x) {
  assert(s);
  assert(s->begin <= s->end);
  const char *restrict const begin = s->begin;
  const char *restrict const end = s->end;
  if(begin + x > end) return SHTTP_STATUS_SLICE_END;
  s->begin += x;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_parse_slice_skip_until(shttp_slice s[static 1], char mark) {
  assert(s);
  assert(s->begin <= s->end);
  const char *restrict begin = s->begin;
  const char *restrict const end = s->end;
  if(begin == end) return SHTTP_STATUS_SLICE_END;
  while(begin != end) {
    if(*begin == mark) {
      s->begin = begin;
      return SHTTP_STATUS_OK;
    }
    begin++;
  }
  s->begin = begin;
  return SHTTP_STATUS_SLICE_END;
}

shttp_status shttp_parse_slice_skip_until_or(shttp_slice s[static 1],
                                             char marks[static 3]) {
  assert(s);
  assert(s->begin <= s->end);
  const char *restrict begin = s->begin;
  const char *restrict const end = s->end;
  if(begin == end) return SHTTP_STATUS_SLICE_END;
  while(begin != end) {
    for(const char *mr = marks; *mr != 0; mr++) {
      if(*mr == *begin) {
        s->begin = begin;
        return SHTTP_STATUS_OK;
      }
    }
    begin++;
  }
  s->begin = begin;
  return SHTTP_STATUS_SLICE_END;
}

shttp_status shttp_parse_slice_skip_until_newline(shttp_slice s[static 1]) {
  assert(s);
  assert(s->begin <= s->end);
  shttp_status status;
  const char *const t = s->begin;
  if((status = shttp_parse_slice_skip_until(s, '\n')))
    return SHTTP_STATUS_NEWLINE_EXPECTED;
  const char *const begin = s->begin;
  if(begin - t > 0 && begin[-1] == '\r') {
    s->begin--;
  }
  return SHTTP_STATUS_OK;
}

shttp_status shttp_parse_slice_skip_until_space(shttp_slice s[static 1]) {
  assert(s);
  assert(s->begin <= s->end);
  shttp_status status;
  if((status = shttp_parse_slice_skip_until(s, ' ')))
    return SHTTP_STATUS_SPACE_EXPECTED;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_parse_slice_skip_past(shttp_slice s[static 1], char end) {
  assert(s);
  assert(s->begin <= s->end);
  shttp_status status;
  if((status = shttp_parse_slice_skip_until(s, end))) return status;
  s->begin++;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_parse_slice_skip_past_or(shttp_slice s[static 1],
                                            char end[static 3]) {
  assert(s);
  assert(end);
  assert(s->begin <= s->end);
  shttp_status status;
  if((status = shttp_parse_slice_skip_until_or(s, end))) return status;
  s->begin++;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_parse_slice_skip_past_newline(shttp_slice s[static 1]) {
  assert(s);
  assert(s->begin <= s->end);
  shttp_status status;
  if((status = shttp_parse_slice_skip_past(s, '\n')))
    return SHTTP_STATUS_NEWLINE_EXPECTED;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_parse_slice_skip_past_space(shttp_slice s[static 1]) {
  assert(s);
  assert(s->begin <= s->end);
  shttp_status status;
  if((status = shttp_parse_slice_skip_past(s, ' ')))
    return SHTTP_STATUS_SPACE_EXPECTED;
  return SHTTP_STATUS_OK;
}

bool shttp_parse_slice_cmp(shttp_slice s1, shttp_slice s2) {
  assert(s1.begin <= s1.end);
  assert(s2.begin <= s2.end);
  const char *restrict begin1 = s1.begin;
  const char *restrict const end1 = s1.end;
  const char *restrict begin2 = s2.begin;
  const char *restrict const end2 = s2.end;
  if(end1 - begin1 < end2 - begin2) {
    while(begin1 != end1) {
      if(*begin1 != *begin2) return false;
      begin1++;
      begin2++;
    }
  } else {
    while(begin2 != end2) {
      if(*begin1 != *begin2) return false;
      begin1++;
      begin2++;
    }
  }
  return true;
}

bool shttp_parse_slice_cmp_until(shttp_slice s1, shttp_slice s2, char mark) {
  assert(s2.begin <= s2.end);
  assert(s1.begin <= s1.end);
  const char *restrict begin1 = s1.begin;
  const char *restrict const end1 = s1.end;
  const char *restrict begin2 = s2.begin;
  const char *restrict const end2 = s2.end;
  if(end1 - begin1 < end2 - begin2) {
    while(begin1 != end1 && *begin1 != mark && *begin2 != mark) {
      if(*begin1 != *begin2) return false;
    }
  } else {
    while(begin2 != end2 && *begin1 != mark && *begin2 != mark) {
      if(*begin1 != *begin2) return false;
    }
  }
  return true;
}

shttp_status shttp_parse_header_next(shttp_slice out[static 1],
                                     shttp_slice s[static 1]) {
  assert(out);
  assert(s);
  assert(out->begin <= out->end);
  assert(s->begin <= s->end);
  shttp_status status;
  const char *restrict const begin = s->begin;
  const char *restrict const end = s->end;
  if(begin == end) return SHTTP_STATUS_SLICE_END;
  out->begin = begin;
  if((status = shttp_parse_slice_skip_until_newline(s))) return status;
  out->end = s->begin;
  if((status = shttp_parse_slice_newline(s))) return status;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_parse_header_key(shttp_slice out[static 1],
                                    shttp_slice s[static 1]) {
  assert(s);
  assert(out);
  assert(out->begin <= out->end);
  assert(s->begin <= s->end);
  shttp_status status;
  const char *restrict const begin = s->begin;
  const char *restrict const end = s->end;
  if(begin == end) return SHTTP_STATUS_SLICE_END;
  out->begin = begin;
  if((status = shttp_parse_slice_skip_until(s, ':'))) return status;
  out->end = s->begin;
  s->begin++;
  if((status = shttp_parse_slice_space_optional(s))) return status;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_parse_header_value(shttp_slice out[static 1],
                                      shttp_slice s[static 1]) {
  assert(out);
  assert(s);
  assert(out->begin <= out->end);
  assert(s->begin <= s->end);
  const char *t;
  shttp_status status;
  const char *restrict begin = s->begin;
  const char *restrict const end = s->end;
  if(begin == end) return SHTTP_STATUS_SLICE_END;
  out->begin = begin;
  t = begin;
  if((status = shttp_parse_slice_skip_until_or(s, " \n,"))) return status;
  begin = s->begin;
  if(begin - t > 0 && begin[-1] == '\r') {
    out->end = begin - 1;
  } else {
    out->end = begin;
  }
  if(*begin == ',' || *begin == '\n') begin++;
  s->begin = begin;
  if((status = shttp_parse_slice_space_optional(s)) != SHTTP_STATUS_SLICE_END)
    return status;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_parse_request(shttp_request req[static 1],
                                 shttp_slice msg[static 1]) {
  assert(req);
  assert(msg);
  assert(msg->begin <= msg->end);
  shttp_status status;
  if((status = shttp_parse_start_line(req, msg))) return status;
  if((status = shttp_parse_slices(req, msg))) return status;
  return SHTTP_STATUS_OK;
}

