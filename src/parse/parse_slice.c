#include "parse_slice.h"

#include <stdio.h>

#include "parse_macros.h"

shttp_status shttp_parse_slice_newline(shttp_slice s[static 1]) {
  if(s->begin == s->end) return SHTTP_STATUS_SLICE_END;
  if(*s->begin == '\r') s->begin++;
  if(s->begin == s->end) return SHTTP_STATUS_SLICE_END;
  if(*s->begin != '\n') return SHTTP_STATUS_NEWLINE_EXPECTED;
  s->begin++;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_parse_slice_space(shttp_slice s[static 1]) {
  if(s->begin == s->end) return SHTTP_STATUS_SLICE_END;
  if(*s->begin != ' ') return SHTTP_STATUS_SPACE_EXPECTED;
  s->begin++;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_parse_slice_space_optional(shttp_slice s[static 1]) {
  if(s->begin == s->end) return SHTTP_STATUS_SLICE_END;
  if(*s->begin == ' ') s->begin++;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_parse_slice_number(shttp_u16 out[static 1],
                                      shttp_slice s[static 1]) {
  if(s->begin == s->end) return SHTTP_STATUS_SLICE_END;
  if(*s->begin > '9' || *s->begin < '0') return SHTTP_STATUS_VALUE_INVALID;
  shttp_u16 n = 0;
  while(s->begin != s->end && *s->begin <= '9' && *s->begin >= '0') {
    n *= 10;
    n += *s->begin - '0';
    s->begin++;
  }
  *out = n;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_parse_slice_skip(shttp_slice s[static 1], shttp_u16 x) {
  if(s->begin + x > s->end) return SHTTP_STATUS_SLICE_END;
  s->begin += x;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_parse_slice_skip_until(shttp_slice s[static 1], char end) {
  if(s->begin == s->end) return SHTTP_STATUS_SLICE_END;
  while(*s->begin != end) {
    if(s->begin == s->end) return SHTTP_STATUS_SLICE_END;
    s->begin++;
  }
  return SHTTP_STATUS_OK;
}

shttp_status shttp_parse_slice_skip_until_or(shttp_slice s[static 1],
                                             char end[static 3]) {
  if(s->begin == s->end) return SHTTP_STATUS_SLICE_END;
  while(true) {
    for(const char *er = end; *er != 0; er++) {
      if(*er == *s->begin) return SHTTP_STATUS_OK;
    }
    if(s->begin == s->end) return SHTTP_STATUS_SLICE_END;
    s->begin++;
  }
}

shttp_status shttp_parse_slice_skip_until_newline(shttp_slice s[static 1]) {
  shttp_status status;
  const char *const t = s->begin;
  if((status = shttp_parse_slice_skip_until(s, '\n')))
    return SHTTP_STATUS_NEWLINE_EXPECTED;
  if(s->begin - t > 0 && s->begin[-1] == '\r') {
    s->begin--;
  }
  return SHTTP_STATUS_OK;
}

shttp_status shttp_parse_slice_skip_until_space(shttp_slice s[static 1]) {
  shttp_status status;
  if((status = shttp_parse_slice_skip_until(s, ' ')))
    return SHTTP_STATUS_SPACE_EXPECTED;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_parse_slice_skip_past(shttp_slice s[static 1], char end) {
  shttp_status status;
  if((status = shttp_parse_slice_skip_until(s, end))) return status;
  s->begin++;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_parse_slice_skip_past_or(shttp_slice s[static 1],
                                            char end[static 3]) {
  shttp_status status;
  if((status = shttp_parse_slice_skip_until_or(s, end))) return status;
  s->begin++;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_parse_slice_skip_past_newline(shttp_slice s[static 1]) {
  shttp_status status;
  if((status = shttp_parse_slice_skip_past(s, '\n')))
    return SHTTP_STATUS_NEWLINE_EXPECTED;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_parse_slice_skip_past_space(shttp_slice s[static 1]) {
  shttp_status status;
  if((status = shttp_parse_slice_skip_past(s, ' ')))
    return SHTTP_STATUS_SPACE_EXPECTED;
  return SHTTP_STATUS_OK;
}

SHTTP_PURE bool shttp_parse_slice_cmp(shttp_slice s1, shttp_slice s2) {
  while(s1.begin != s1.end && s2.begin != s2.end) {
    if(*s1.begin != *s2.begin) return false;
    s1.begin++;
    s2.begin++;
  }
  return true;
}

SHTTP_PURE bool shttp_parse_slice_cmp_until(shttp_slice s1, shttp_slice s2,
                                            char end) {
  while(s1.begin != s1.end && s2.begin != s2.end && *s1.begin != end &&
        *s2.begin != end) {
    if(*s1.begin != *s2.begin) return false;
  }
  return true;
}
