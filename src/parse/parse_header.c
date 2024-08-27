#include "parse_header.h"

#include <stdio.h>

#include "parse_macros.h"
#include "parse_slice.h"

shttp_status shttp_parse_header_next(shttp_slice out[static 1],
                                     shttp_slice s[static 1]) {
  shttp_status status;
  if(s->begin == s->end) return SHTTP_STATUS_SLICE_END;
  out->begin = s->begin;
  if((status = shttp_parse_slice_skip_until_newline(s))) return status;
  out->end = s->begin;
  if((status = shttp_parse_slice_newline(s))) return status;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_parse_header_key(shttp_slice out[static 1],
                                    shttp_slice s[static 1]) {
  shttp_status status;
  if(s->begin == s->end) return SHTTP_STATUS_SLICE_END;
  out->begin = s->begin;
  if((status = shttp_parse_slice_skip_until(s, ':'))) return status;
  out->end = s->begin;
  s->begin++;
  if((status = shttp_parse_slice_space_optional(s))) return status;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_parse_header_value(shttp_slice out[static 1],
                                      shttp_slice s[static 1]) {
  const char* t;
  shttp_status status;
  if(s->begin == s->end) return SHTTP_STATUS_SLICE_END;
  out->begin = s->begin;
  t = s->begin;
  if((status = shttp_parse_slice_skip_until_or(s, " \n,"))) return status;
  if(s->begin - t > 0 && s->begin[-1] == '\r') s->begin--;
  out->end = s->begin;
  if(*s->begin == ',') s->begin++;
  if((status = shttp_parse_slice_space_optional(s))) return status;
  return SHTTP_STATUS_OK;
}
