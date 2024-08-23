#ifndef PARSE_TOKEN_H
#define PARSE_TOKEN_H

#include "parse_types.h"

shttp_reqi shttp_parse_token_number(shttp_u16 *n, const char *msg,
                                    shttp_reqi len);

shttp_reqi shttp_parse_token_cpy_until(char *restrict token,
                                       const char *restrict msg, shttp_reqi len,
                                       char end);

shttp_reqi shttp_parse_token_cpy_until_or(char *restrict token,
                                          const char *restrict msg,
                                          shttp_reqi len, const char *end);

bool shttp_parse_token_cmp_until(const char *restrict token,
                                 const char *restrict msg, shttp_reqi len,
                                 char end);

shttp_reqi shttp_parse_token_cpy(char *restrict token, const char *restrict msg,
                                 shttp_reqi len);

bool shttp_parse_token_cmp(const char *restrict token, const char *restrict msg,
                           shttp_reqi len);

shttp_reqi shttp_parse_token_values_weighted(shttp_value_weighted *vals,
                                             shttp_u8 vals_length,
                                             const char *msg,
                                             shttp_reqi msg_len);

shttp_reqi shttp_parse_token_values(shttp_value *vals, shttp_u8 vals_length,
                                    const char *msg, shttp_reqi msg_len);

shttp_reqi shttp_parse_token_value(shttp_value *val, const char *msg,
                                   shttp_reqi msg_len);

shttp_reqi shttp_parse_token_datetime(shttp_datetime *datetime, const char *msg,
                                      shttp_reqi msg_len);

#endif
