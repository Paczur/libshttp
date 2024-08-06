#ifndef SHTTP_CONN_H
#define SHTTP_CONN_H

#include "types.h"

#define MAX_CONNS 10
typedef shttp_u8 shttp_conn_id;

bool shttp_conn_accept(shttp_u16 timeout);
bool shttp_conn_accept_nblk(void);
bool shttp_conn_id_valid(shttp_conn_id id);
shttp_conn_id shttp_conn_next(char *req, shttp_u16 *len, shttp_u16 max_len,
                              shttp_u16 timeout);
shttp_conn_id shttp_conn_next_nblk(char *req, shttp_u16 *len,
                                   shttp_u16 max_len);
void shttp_conn_send(const char *res, shttp_u16 res_len, shttp_conn_id id);
bool shttp_conn_init(void);
void shttp_conn_deinit(void);

#endif
