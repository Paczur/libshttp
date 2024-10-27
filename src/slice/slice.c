#include "slice.h"

#include <assert.h>
#include <string.h>

SHTTP_PURE uint32_t shttp_slice_length(shttp_slice s[static 1]) {
  SHTTP_ASSERT(s);
  SHTTP_ASSERT(s->end > s->begin);
  return s->end - s->begin;
}

SHTTP_PURE bool shttp_slice_empty(shttp_slice s[static 1]) {
  SHTTP_ASSERT(s);
  return s->end <= s->begin;
}

shttp_status shttp_slice_parse_newline(shttp_slice s[static 1]) {
  SHTTP_ASSERT(s);
  SHTTP_ASSERT(s->begin <= s->end);
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

shttp_status shttp_slice_parse_space(shttp_slice s[static 1]) {
  SHTTP_ASSERT(s);
  SHTTP_ASSERT(s->begin <= s->end);
  const char *restrict const begin = s->begin;
  const char *restrict const end = s->end;
  if(begin == end) return SHTTP_STATUS_SLICE_END;
  if(*begin != ' ') return SHTTP_STATUS_SPACE_EXPECTED;
  s->begin++;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_slice_parse_space_optional(shttp_slice s[static 1]) {
  SHTTP_ASSERT(s);
  SHTTP_ASSERT(s->begin <= s->end);
  const char *restrict const begin = s->begin;
  const char *restrict const end = s->end;
  if(begin == end) return SHTTP_STATUS_SLICE_END;
  if(*begin == ' ') s->begin++;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_slice_parse_i32(int32_t out[static 1],
                                   shttp_slice s[static 1]) {
  SHTTP_ASSERT(out);
  SHTTP_ASSERT(s);
  SHTTP_ASSERT(s->begin <= s->end);
  bool minus_flag = false;
  const char *restrict begin = s->begin;
  const char *restrict const end = s->end;
  if(begin == end) return SHTTP_STATUS_SLICE_END;
  if(*begin == '-') {
    minus_flag = true;
    begin++;
  }
  if(*begin > '9' || *begin < '0') return SHTTP_STATUS_VALUE_INVALID;
  int32_t n = 0;
  while(begin != end && *begin <= '9' && *begin >= '0') {
    n *= 10;
    n += *begin - '0';
    begin++;
  }
  s->begin = begin;
  if(minus_flag) n = -n;
  *out = n;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_slice_parse_i16(int16_t out[static 1],
                                   shttp_slice s[static 1]) {
  SHTTP_ASSERT(out);
  SHTTP_ASSERT(s);
  SHTTP_ASSERT(s->begin <= s->end);
  bool minus_flag = false;
  const char *restrict begin = s->begin;
  const char *restrict const end = s->end;
  if(begin == end) return SHTTP_STATUS_SLICE_END;
  if(*begin == '-') {
    minus_flag = true;
    begin++;
  }
  if(*begin > '9' || *begin < '0') return SHTTP_STATUS_VALUE_INVALID;
  int16_t n = 0;
  while(begin != end && *begin <= '9' && *begin >= '0') {
    n *= 10;
    n += *begin - '0';
    begin++;
  }
  s->begin = begin;
  if(minus_flag) n = -n;
  *out = n;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_slice_parse_i8(int8_t out[static 1],
                                  shttp_slice s[static 1]) {
  SHTTP_ASSERT(out);
  SHTTP_ASSERT(s);
  SHTTP_ASSERT(s->begin <= s->end);
  bool minus_flag = false;
  const char *restrict begin = s->begin;
  const char *restrict const end = s->end;
  if(begin == end) return SHTTP_STATUS_SLICE_END;
  if(*begin == '-') {
    minus_flag = true;
    begin++;
  }
  if(*begin > '9' || *begin < '0') return SHTTP_STATUS_VALUE_INVALID;
  uint8_t n = 0;
  while(begin != end && *begin <= '9' && *begin >= '0') {
    n *= 10;
    n += *begin - '0';
    begin++;
  }
  s->begin = begin;
  if(minus_flag) n = -n;
  *out = n;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_slice_parse_u32(uint32_t out[static 1],
                                   shttp_slice s[static 1]) {
  SHTTP_ASSERT(out);
  SHTTP_ASSERT(s);
  SHTTP_ASSERT(s->begin <= s->end);
  const char *restrict begin = s->begin;
  const char *restrict const end = s->end;
  if(begin == end) return SHTTP_STATUS_SLICE_END;
  if(*begin > '9' || *begin < '0') return SHTTP_STATUS_VALUE_INVALID;
  uint32_t n = 0;
  while(begin != end && *begin <= '9' && *begin >= '0') {
    n *= 10;
    n += *begin - '0';
    begin++;
  }
  s->begin = begin;
  *out = n;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_slice_parse_u16(uint16_t out[static 1],
                                   shttp_slice s[static 1]) {
  SHTTP_ASSERT(out);
  SHTTP_ASSERT(s);
  SHTTP_ASSERT(s->begin <= s->end);
  const char *restrict begin = s->begin;
  const char *restrict const end = s->end;
  if(begin == end) return SHTTP_STATUS_SLICE_END;
  if(*begin > '9' || *begin < '0') return SHTTP_STATUS_VALUE_INVALID;
  uint16_t n = 0;
  while(begin != end && *begin <= '9' && *begin >= '0') {
    n *= 10;
    n += *begin - '0';
    begin++;
  }
  s->begin = begin;
  *out = n;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_slice_parse_u8(uint8_t out[static 1],
                                  shttp_slice s[static 1]) {
  SHTTP_ASSERT(out);
  SHTTP_ASSERT(s);
  SHTTP_ASSERT(s->begin <= s->end);
  const char *restrict begin = s->begin;
  const char *restrict const end = s->end;
  if(begin == end) return SHTTP_STATUS_SLICE_END;
  if(*begin > '9' || *begin < '0') return SHTTP_STATUS_VALUE_INVALID;
  uint8_t n = 0;
  while(begin != end && *begin <= '9' && *begin >= '0') {
    n *= 10;
    n += *begin - '0';
    begin++;
  }
  s->begin = begin;
  *out = n;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_slice_skip(shttp_slice s[static 1], uint32_t x) {
  SHTTP_ASSERT(s);
  SHTTP_ASSERT(s->begin <= s->end);
  const char *restrict const begin = s->begin;
  const char *restrict const end = s->end;
  if(begin + x > end) return SHTTP_STATUS_SLICE_END;
  s->begin += x;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_slice_skip_until(shttp_slice s[static 1], char mark) {
  SHTTP_ASSERT(s);
  SHTTP_ASSERT(s->begin <= s->end);
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

shttp_status shttp_slice_skip_until_or(shttp_slice s[static 1],
                                       char marks[static 3]) {
  SHTTP_ASSERT(s);
  SHTTP_ASSERT(s->begin <= s->end);
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

shttp_status shttp_slice_skip_until_newline(shttp_slice s[static 1]) {
  SHTTP_ASSERT(s);
  SHTTP_ASSERT(s->begin <= s->end);
  shttp_status status;
  const char *const t = s->begin;
  if((status = shttp_slice_skip_until(s, '\n')))
    return SHTTP_STATUS_NEWLINE_EXPECTED;
  const char *const begin = s->begin;
  if(begin - t > 0 && begin[-1] == '\r') {
    s->begin--;
  }
  return SHTTP_STATUS_OK;
}

shttp_status shttp_slice_skip_until_space(shttp_slice s[static 1]) {
  SHTTP_ASSERT(s);
  SHTTP_ASSERT(s->begin <= s->end);
  shttp_status status;
  if((status = shttp_slice_skip_until(s, ' ')))
    return SHTTP_STATUS_SPACE_EXPECTED;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_slice_skip_past(shttp_slice s[static 1], char end) {
  SHTTP_ASSERT(s);
  SHTTP_ASSERT(s->begin <= s->end);
  SHTTP_PROP(shttp_slice_skip_until(s, end));
  s->begin++;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_slice_skip_past_or(shttp_slice s[static 1],
                                      char end[static 3]) {
  SHTTP_ASSERT(s);
  SHTTP_ASSERT(end);
  SHTTP_ASSERT(s->begin <= s->end);
  SHTTP_PROP(shttp_slice_skip_until_or(s, end));
  s->begin++;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_slice_skip_past_newline(shttp_slice s[static 1]) {
  SHTTP_ASSERT(s);
  SHTTP_ASSERT(s->begin <= s->end);
  shttp_status status;
  if((status = shttp_slice_skip_past(s, '\n')))
    return SHTTP_STATUS_NEWLINE_EXPECTED;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_slice_skip_past_space(shttp_slice s[static 1]) {
  SHTTP_ASSERT(s);
  SHTTP_ASSERT(s->begin <= s->end);
  shttp_status status;
  if((status = shttp_slice_skip_past(s, ' ')))
    return SHTTP_STATUS_SPACE_EXPECTED;
  return SHTTP_STATUS_OK;
}

bool shttp_slice_starts_with(shttp_slice s1, shttp_slice s2) {
  SHTTP_ASSERT(s1.begin <= s1.end);
  SHTTP_ASSERT(s2.begin <= s2.end);
  const char *restrict begin1 = s1.begin;
  const char *restrict begin2 = s2.begin;
  const char *restrict const end2 = s2.end;
  if(s1.end - begin1 < end2 - begin2) return false;
  while(begin2 != end2) {
    if(*begin1 != *begin2) return false;
    begin1++;
    begin2++;
  }
  return true;
}

bool shttp_slice_eq(shttp_slice s1, shttp_slice s2) {
  SHTTP_ASSERT(s1.begin <= s1.end);
  SHTTP_ASSERT(s2.begin <= s2.end);
  const char *restrict begin1 = s1.begin;
  const char *restrict begin2 = s2.begin;
  const char *restrict const end2 = s2.end;
  if(s1.end - begin1 != end2 - begin2) return false;
  while(begin2 != end2) {
    if(*begin1 != *begin2) return false;
    begin1++;
    begin2++;
  }
  return true;
}

bool shttp_slice_eq_until(shttp_slice s1, shttp_slice s2, char mark) {
  SHTTP_ASSERT(s2.begin <= s2.end);
  SHTTP_ASSERT(s1.begin <= s1.end);
  const char *restrict begin1 = s1.begin;
  const char *restrict const end1 = s1.end;
  const char *restrict begin2 = s2.begin;
  const char *restrict const end2 = s2.end;
  if(end1 - begin1 < end2 - begin2) {
    while(begin1 != end1 && *begin1 != mark && *begin2 != mark) {
      if(*begin1 != *begin2) return false;
      begin2++;
      begin1++;
    }
  } else {
    while(begin2 != end2 && *begin1 != mark && *begin2 != mark) {
      if(*begin1 != *begin2) return false;
      begin2++;
      begin1++;
    }
  }
  return true;
}

shttp_status shttp_header_next(shttp_slice out[static 1],
                               shttp_slice s[static 1]) {
  SHTTP_ASSERT(out);
  SHTTP_ASSERT(s);
  SHTTP_ASSERT(out->begin <= out->end);
  SHTTP_ASSERT(s->begin <= s->end);
  const char *restrict const begin = s->begin;
  const char *restrict const end = s->end;
  if(begin == end) return SHTTP_STATUS_SLICE_END;
  out->begin = begin;
  SHTTP_PROP(shttp_slice_skip_until_newline(s));
  out->end = s->begin;
  return shttp_slice_parse_newline(s);
}

shttp_status shttp_header_key(shttp_slice out[static 1],
                              shttp_slice s[static 1]) {
  SHTTP_ASSERT(s);
  SHTTP_ASSERT(out);
  SHTTP_ASSERT(out->begin <= out->end);
  SHTTP_ASSERT(s->begin <= s->end);
  const char *restrict const begin = s->begin;
  const char *restrict const end = s->end;
  if(begin == end) return SHTTP_STATUS_SLICE_END;
  out->begin = begin;
  SHTTP_PROP(shttp_slice_skip_until(s, ':'));
  out->end = s->begin;
  s->begin++;
  return shttp_slice_parse_space_optional(s);
}

shttp_status shttp_header_value(shttp_slice out[static 1],
                                shttp_slice s[static 1]) {
  SHTTP_ASSERT(out);
  SHTTP_ASSERT(s);
  SHTTP_ASSERT(out->begin <= out->end);
  SHTTP_ASSERT(s->begin <= s->end);
  const char *t;
  shttp_status status;
  const char *restrict begin = s->begin;
  const char *restrict const end = s->end;
  if(begin == end) return SHTTP_STATUS_SLICE_END;
  out->begin = begin;
  t = begin;
  SHTTP_PROP(shttp_slice_skip_until_or(s, " \n,"));
  begin = s->begin;
  if(begin - t > 0 && begin[-1] == '\r') {
    out->end = begin - 1;
  } else {
    out->end = begin;
  }
  if(*begin == ',' || *begin == '\n') begin++;
  s->begin = begin;
  if((status = shttp_slice_parse_space_optional(s)) != SHTTP_STATUS_SLICE_END)
    return status;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_slice_insert_newline(shttp_mut_slice msg[static 1]) {
  SHTTP_ASSERT(msg);
  SHTTP_ASSERT(msg->begin <= msg->end);
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

shttp_status shttp_slice_cpy(shttp_mut_slice msg[static 1], shttp_slice slice) {
  SHTTP_ASSERT(msg);
  SHTTP_ASSERT(msg->begin <= msg->end);
  SHTTP_ASSERT(slice.begin <= slice.end);
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

shttp_status shttp_slice_cpy_char(shttp_mut_slice msg[static 1], char c) {
  SHTTP_ASSERT(msg);
  SHTTP_ASSERT(msg->begin <= msg->end);
  char *restrict beginm = msg->begin;
  char *restrict const endm = msg->end;
  if(endm == beginm) return SHTTP_STATUS_SLICE_END;
  *(beginm++) = c;
  return SHTTP_STATUS_OK;
}
