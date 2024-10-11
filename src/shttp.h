#ifndef SHTTP_H
#define SHTTP_H

#include "compose/compose.h"
#include "parse/parse.h"
#include "types.h"

#define SHTTP_PROP(x)               \
  do {                              \
    const shttp_status _status = x; \
    if(_status) return _status;     \
  } while(0)

void shttp_response_to_request(shttp_response res[static 1],
                               const shttp_request req[static 1]);

/* returns: TIMEOUT, SLICE_END, PREFIX_INVALID, VALUE_INVALID, NEWLINE_EXPECTED,
 * SPACE_EXPECTED
 */
SHTTP_UNUSED_RESULT shttp_status shttp_next(shttp_request req[static 1],
                                            shttp_mut_slice buff[static 1],
                                            shttp_u16 timeout);

/* returns: TIMEOUT, SLICE_END, PREFIX_INVALID, VALUE_INVALID, NEWLINE_EXPECTED,
 * SPACE_EXPECTED
 */
SHTTP_UNUSED_RESULT shttp_status
shttp_next_nblk(shttp_request req[static 1], shttp_mut_slice buff[static 1]);

// returns: CONN_SEND, TIMEOUT, CONN_ACCEPT, VALUE_INVALID, SLICE_END
SHTTP_UNUSED_RESULT shttp_status shttp_send(shttp_mut_slice buff[static 1],
                                            const shttp_response res[static 1]);

// returns: CONN_FD_CLOSE
SHTTP_UNUSED_RESULT shttp_status shttp_close(shttp_socket sock[static 1],
                                             shttp_conn_id id);

// returns: SOCK_CREATE, SOCK_BIND, SOCK_LISTEN
SHTTP_UNUSED_RESULT shttp_status shttp_init(shttp_socket sock[static 1],
                                            shttp_u16 port);

// returns: CONN_FD_CLOSE, SOCK_FD_CLOSE
SHTTP_UNUSED_RESULT shttp_status shttp_deinit(shttp_socket sock[static 1],
                                              bool force);

#endif
