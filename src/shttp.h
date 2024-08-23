#ifndef SHTTP_H
#define SHTTP_H

#include "types.h"
void shttp_response_to_request(shttp_response *res, const shttp_request *req);
bool shttp_next(shttp_request *req, shttp_u16 timeout);
bool shttp_next_nblk(shttp_request *req);
void shttp_send(const shttp_response *res);

bool shttp_init(void);
void shttp_deinit(void);

#endif
