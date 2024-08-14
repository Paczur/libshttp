#ifndef PARSE_TOKEN_H
#define PARSE_TOKEN_H

#include "parse_types.h"

shttp_reqi shttp_parse_token_number(shttp_u16 *n, const char *msg,
                                    shttp_reqi len);

shttp_reqi shttp_parse_token_cpy_until(char *token, const char *msg,
                                       shttp_reqi len, char end);

bool shttp_parse_token_cmp_until(const char *token, const char *msg,
                                 shttp_reqi len, char end);

shttp_reqi shttp_parse_token_cpy(char *token, const char *msg, shttp_reqi len);

bool shttp_parse_token_cmp(const char *token, const char *msg, shttp_reqi len);

#endif