#ifndef PARSE_H
#define PARSE_H

#include "types.h"

shttp_u16 shttp_parse_request(shttp_request *req, const char *msg,
                              shttp_u16 msg_len);

shttp_u16 shttp_parse_response(char *msg, shttp_u16 msg_len,
                               const shttp_response *res);

#endif
