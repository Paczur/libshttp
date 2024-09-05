#ifndef COMPOSE_H
#define COMPOSE_H

#include "../types.h"

SHTTP_UNUSED_RESULT shttp_status
shttp_compose_slice_newline(shttp_mut_slice msg[static 1]);

SHTTP_UNUSED_RESULT shttp_status
shttp_compose_slice_cpy(shttp_mut_slice msg[static 1], shttp_slice slice);

SHTTP_UNUSED_RESULT shttp_status shttp_compose_response(
  shttp_mut_slice msg[static 1], const shttp_response res[static 1]);

#endif
