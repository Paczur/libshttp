#ifndef SHTTP_COMPOSE_H
#define SHTTP_COMPOSE_H

#include "../types.h"

// returns: SLICE_END
SHTTP_UNUSED_RESULT shttp_status
shttp_compose_slice_newline(shttp_mut_slice msg[static 1]);

// returns: SLICE_END
SHTTP_UNUSED_RESULT shttp_status
shttp_compose_slice_cpy(shttp_mut_slice msg[static 1], shttp_slice slice);

// returns: SLICE_END
SHTTP_UNUSED_RESULT shttp_status
shttp_compose_slice_cpy_char(shttp_mut_slice msg[static 1], char c);

// returns: VALUE_INVALID, SLICE_END
SHTTP_UNUSED_RESULT shttp_status shttp_compose_response(
  shttp_mut_slice msg[static 1], const shttp_response res[static 1]);

#endif
