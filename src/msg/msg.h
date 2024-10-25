#ifndef SHTTP_MSG_H
#define SHTTP_MSG_H

#include "../conf.h"
#include "../types.h"

/* Fills request and makes "s" point to request body
 * returns: SLICE_END, PREFIX_INVALID, VALUE_INVALID, NEWLINE_EXPECTED,
 * SPACE_EXPECTED
 */
SHTTP_UNUSED_RESULT shttp_status shttp_msg_request(shttp_request req[static 1],
                                                   shttp_slice msg[static 1]);

// returns: VALUE_INVALID, SLICE_END
SHTTP_UNUSED_RESULT shttp_status shttp_msg_response(
  shttp_mut_slice msg[static 1], const shttp_response res[static 1]);

#endif
