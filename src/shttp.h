#ifndef SHTTP_H
#define SHTTP_H

#include "conn.h"
#include "types.h"

typedef struct shttp_request {
  shttp_conn_id id;
} shttp_request;

typedef struct shttp_response {
  shttp_conn_id id;
} shttp_response;

void shttp_parse_request(shttp_request *req, const char *msg,
                         shttp_u16 msg_len);
shttp_u16 shttp_parse_response(char *msg, shttp_u16 msg_len,
                               const shttp_response *res);
void shttp_response_to_request(shttp_response *res, const shttp_request *req);
bool shttp_next(shttp_request *req, shttp_u16 timeout);
bool shttp_next_nblk(shttp_request *req);
void shttp_send(const shttp_response *res);

bool shttp_init(void);
void shttp_deinit(void);

#endif
