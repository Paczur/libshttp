#ifndef SHTTP_H
#define SHTTP_H

#include "conf.h"
#include "keep_alive.h"
#include "slice/slice.h"
#include "types.h"

/* Currently only used for timer management
 * returns: TIMER_SET, CONN_FD_CLOSE
 */
SHTTP_UNUSED_RESULT shttp_status
shttp_signal_handler(shttp_socket sock[static 1]);

void shttp_response_to_request(shttp_response res[static 1],
                               const shttp_request req[static 1]);

/* returns: TIMEOUT, SLICE_END, PREFIX_INVALID, VALUE_INVALID, NEWLINE_EXPECTED,
 * SPACE_EXPECTED
 */
SHTTP_UNUSED_RESULT shttp_status shttp_next(shttp_request req[static 1],
                                            shttp_mut_slice buff[static 1],
                                            shttp_socket sock[static 1],
                                            uint32_t timeout);

// Same as shttp_next, but ignores errors
shttp_status shttp_next_ignore(shttp_request req[static 1],
                               shttp_mut_slice buff[static 1],
                               shttp_socket sock[static 1], uint32_t timeout);

// Same as shttp_next with timeout = 0
SHTTP_UNUSED_RESULT shttp_status shttp_next_nblk(shttp_request req[static 1],
                                                 shttp_mut_slice buff[static 1],
                                                 shttp_socket sock[static 1]);

// Same as shttp_next_nblk, but ignores errors
shttp_status shttp_next_nblk_ignore(shttp_request req[static 1],
                                    shttp_mut_slice buff[static 1],
                                    shttp_socket sock[static 1]);

// returns: CONN_SEND, TIMEOUT, CONN_ACCEPT, VALUE_INVALID, SLICE_END
SHTTP_UNUSED_RESULT shttp_status shttp_send(shttp_mut_slice buff[static 1],
                                            const shttp_response res[static 1]);

// Same as shttp_send, but ignores errors
shttp_status shttp_send_ignore(shttp_mut_slice buff[static 1],
                               const shttp_response res[static 1]);

// returns: CONN_FD_CLOSE
SHTTP_UNUSED_RESULT shttp_status shttp_close(shttp_socket sock[static 1],
                                             shttp_conn_id id);

// returns: SOCK_CREATE, SOCK_BIND, SOCK_LISTEN
SHTTP_UNUSED_RESULT shttp_status shttp_init(shttp_socket sock[static 1],
                                            uint32_t port);

// returns: CONN_FD_CLOSE, SOCK_FD_CLOSE
SHTTP_UNUSED_RESULT shttp_status shttp_deinit(shttp_socket sock[static 1],
                                              bool force);

#ifdef SHTTP_KEEP_ALIVE_ENABLE
#define shttp_next(req, buf, sock, timeout) \
  shttp_keep_alive_next(req, buf, sock, timeout)
#define shttp_next_nblk(req, buf, sock) \
  shttp_keep_alive_next_nblk(req, buf, sock)
#define shttp_next_ignore(req, buf, sock, timeout) \
  shttp_keep_alive_next_ignore(req, buf, sock, timeout)
#define shttp_next_nblk_ignore(req, buf, sock) \
  shttp_keep_alive_next_nblk_ignore(req, buf, sock)
#define shttp_send(buff, res) shttp_keep_alive_send(buff, res)
#define shttp_send_ignore(buff, res) shttp_keep_alive_send_ignore(buff, res)
#define shttp_close(sock, id) shttp_keep_alive_close(sock, id)
#define shttp_init(sock, port) shttp_keep_alive_init(sock, port)
#define shttp_deinit(sock, force) shttp_keep_alive_deinit(sock, force)
#endif
#endif
