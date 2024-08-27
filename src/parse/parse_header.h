#ifndef PARSE_HEADER_H
#define PARSE_HEADER_H

#include "../types.h"

shttp_status shttp_parse_header_next(shttp_slice out[static 1],
                                     shttp_slice s[static 1]);

shttp_status shttp_parse_header_key(shttp_slice out[static 1],
                                    shttp_slice s[static 1]);

shttp_status shttp_parse_header_value(shttp_slice out[static 1],
                                      shttp_slice s[static 1]);

#endif
