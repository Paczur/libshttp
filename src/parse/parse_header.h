#ifndef PARSE_HEADER_H
#define PARSE_HEADER_H

#include "parse_types.h"

shttp_reqi shttp_parse_header(shttp_request *req, const char *msg,
                              shttp_reqi msg_len);

#endif
