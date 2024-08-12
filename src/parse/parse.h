#ifndef PARSE_H
#define PARSE_H

#include "parse_types.h"

shttp_reqi shttp_parse_request(shttp_request *req, const char *msg,
                               shttp_reqi msg_len);

#endif
