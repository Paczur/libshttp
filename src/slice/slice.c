#include "slice.h"

#include <assert.h>
#include <string.h>

#include "../private.h"

shttp_status shttp_slice_parse_newline(shttp_slice s[static 1]) {
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

shttp_status shttp_slice_parse_space(shttp_slice s[static 1]) {
  assert(s);
  assert(s->begin <= s->end);
  const char *restrict const begin = s->begin;
  const char *restrict const end = s->end;
  if(begin == end) return SHTTP_STATUS_SLICE_END;
  if(*begin != ' ') return SHTTP_STATUS_SPACE_EXPECTED;
  s->begin++;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_slice_parse_space_optional(shttp_slice s[static 1]) {
  assert(s);
  assert(s->begin <= s->end);
  const char *restrict const begin = s->begin;
  const char *restrict const end = s->end;
  if(begin == end) return SHTTP_STATUS_SLICE_END;
  if(*begin == ' ') s->begin++;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_slice_parse_number(shttp_u16 out[static 1],
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

shttp_status shttp_slice_skip(shttp_slice s[static 1], shttp_u16 x) {
  assert(s);
  assert(s->begin <= s->end);
  const char *restrict const begin = s->begin;
  const char *restrict const end = s->end;
  if(begin + x > end) return SHTTP_STATUS_SLICE_END;
  s->begin += x;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_slice_skip_until(shttp_slice s[static 1], char mark) {
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

shttp_status shttp_slice_skip_until_or(shttp_slice s[static 1],
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

shttp_status shttp_slice_skip_until_newline(shttp_slice s[static 1]) {
  assert(s);
  assert(s->begin <= s->end);
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
  assert(s);
  assert(s->begin <= s->end);
  shttp_status status;
  if((status = shttp_slice_skip_until(s, ' ')))
    return SHTTP_STATUS_SPACE_EXPECTED;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_slice_skip_past(shttp_slice s[static 1], char end) {
  assert(s);
  assert(s->begin <= s->end);
  SHTTP_PROP(shttp_slice_skip_until(s, end));
  s->begin++;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_slice_skip_past_or(shttp_slice s[static 1],
                                      char end[static 3]) {
  assert(s);
  assert(end);
  assert(s->begin <= s->end);
  SHTTP_PROP(shttp_slice_skip_until_or(s, end));
  s->begin++;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_slice_skip_past_newline(shttp_slice s[static 1]) {
  assert(s);
  assert(s->begin <= s->end);
  shttp_status status;
  if((status = shttp_slice_skip_past(s, '\n')))
    return SHTTP_STATUS_NEWLINE_EXPECTED;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_slice_skip_past_space(shttp_slice s[static 1]) {
  assert(s);
  assert(s->begin <= s->end);
  shttp_status status;
  if((status = shttp_slice_skip_past(s, ' ')))
    return SHTTP_STATUS_SPACE_EXPECTED;
  return SHTTP_STATUS_OK;
}

bool shttp_slice_eq(shttp_slice s1, shttp_slice s2) {
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

bool shttp_slice_eq_until(shttp_slice s1, shttp_slice s2, char mark) {
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

shttp_status shttp_header_next(shttp_slice out[static 1],
                               shttp_slice s[static 1]) {
  assert(out);
  assert(s);
  assert(out->begin <= out->end);
  assert(s->begin <= s->end);
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
  assert(s);
  assert(out);
  assert(out->begin <= out->end);
  assert(s->begin <= s->end);
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

shttp_status shttp_slice_cpy(shttp_mut_slice msg[static 1], shttp_slice slice) {
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

shttp_status shttp_slice_cpy_char(shttp_mut_slice msg[static 1], char c) {
  assert(msg);
  assert(msg->begin <= msg->end);
  char *restrict beginm = msg->begin;
  char *restrict const endm = msg->end;
  if(endm == beginm) return SHTTP_STATUS_SLICE_END;
  *(beginm++) = c;
  return SHTTP_STATUS_OK;
}
