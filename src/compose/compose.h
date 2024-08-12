#ifndef COMPOSE_H
#define COMPOSE_H

#include "../types.h"

typedef shttp_u32 shttp_resi;

shttp_resi shttp_compose_response(char *msg, shttp_resi msg_len,
                                  const shttp_response *res);

#endif
