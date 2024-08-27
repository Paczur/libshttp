#ifndef PARSE_MACROS_H
#define PARSE_MACROS_H

#include "../private.h"
#define SHTTP_CMP(t, m) \
  shttp_parse_slice_cmp((shttp_slice){(t), (t) + sizeof(t) - 1}, (m))

#endif
