#ifndef SHTTP_CONN_H
#define SHTTP_CONN_H

#include "types.h"

// returns: CONN_FD_CLOSE
SHTTP_UNUSED_RESULT shttp_status shttp_conn_close(int fd[static 1]);

// returns: TIMEOUT, CONN_ACCEPT
SHTTP_UNUSED_RESULT shttp_status shttp_conn_accept(shttp_u16 timeout);

// returns: TIMEOUT, CONN_ACCEPT
SHTTP_UNUSED_RESULT shttp_status shttp_conn_accept_nblk(void);

// returns: TIMEOUT, SLICE_END
SHTTP_UNUSED_RESULT shttp_status shttp_conn_next(shttp_conn_id id[static 1],
                                                 shttp_mut_slice req[static 1],
                                                 shttp_u16 timeout);

// returns: TIMEOUT, SLICE_END
shttp_status shttp_conn_next_nblk(shttp_conn_id id[static 1],
                                  shttp_mut_slice req[static 1]);

// returns: CONN_SEND, CONN_FD_CLOSE, TIMEOUT, CONN_ACCEPT
SHTTP_UNUSED_RESULT shttp_status shttp_conn_send(shttp_slice res,
                                                 shttp_conn_id id);

// returns: SOCKET_CREATE, SOCKET_BIND, SOCKET_LISTEN
SHTTP_UNUSED_RESULT shttp_status shttp_conn_init(shttp_u16 port);

// returns: CONN_FD_CLOSE, SOCK_FD_CLOSE
SHTTP_UNUSED_RESULT shttp_status shttp_conn_deinit(bool force);

#endif
