#ifndef SHTTP_SOCK_H
#define SHTTP_SOCK_H

#include <poll.h>

#include "types.h"

// returns: TIMEOUT, CONN_ACCEPT
SHTTP_UNUSED_RESULT shttp_status
shttp_sock_accept(shttp_socket sock[restrict 1], shttp_u16 timeout);

// returns: TIMEOUT, CONN_ACCEPT
SHTTP_UNUSED_RESULT shttp_status
shttp_sock_accept_nblk(shttp_socket sock[restrict 1]);

// returns: TIMEOUT, SLICE_END
SHTTP_UNUSED_RESULT shttp_status shttp_sock_next(shttp_socket sock[restrict 1],
                                                 shttp_conn_id id[static 1],
                                                 shttp_mut_slice req[static 1],
                                                 shttp_u16 timeout);

// returns: TIMEOUT, SLICE_END
shttp_status shttp_sock_next_nblk(shttp_socket sock[restrict 1],
                                  shttp_conn_id id[static 1],
                                  shttp_mut_slice req[static 1]);

// returns: CONN_SEND, CONN_FD_CLOSE, TIMEOUT, CONN_ACCEPT
SHTTP_UNUSED_RESULT shttp_status shttp_sock_send(shttp_socket sock[restrict 1],
                                                 shttp_slice res,
                                                 shttp_conn_id id);

// returns: SOCKET_CREATE, SOCKET_BIND, SOCKET_LISTEN
SHTTP_UNUSED_RESULT shttp_status shttp_sock_init(shttp_socket sock[restrict 1],
                                                 shttp_u16 port);

// returns: CONN_FD_CLOSE, SOCK_FD_CLOSE
SHTTP_UNUSED_RESULT shttp_status
shttp_sock_deinit(shttp_socket sock[restrict 1], bool force);

#endif