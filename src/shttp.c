#include "shttp.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "conf.h"
#include "msg/msg.h"
#include "slice/slice.h"
#include "sock/sock.h"
#include "timer/timer.h"

shttp_status shttp_signal_handler(shttp_socket sock[static 1]) {
  SHTTP_ASSERT(sock);
  SHTTP_ASSERT(sock->conns);
  SHTTP_ASSERT(sock->conn_count > 0);
  SHTTP_ASSERT(sock->conn_timers);
  SHTTP_ASSERT(sock->conn_timers[0].time != -1);
  shttp_conn_id id = sock->conn_timers[0].conn;
  shttp_timer_next(sock->timer, sock->conn_timers, sock->conn_count);
  return shttp_sock_close(sock, id);
}

shttp_status shttp_next(shttp_request req[static 1],
                        shttp_mut_slice buff[static 1],
                        shttp_socket sock[static 1], uint32_t timeout) {
  SHTTP_ASSERT(req);
  SHTTP_ASSERT(buff);
  SHTTP_ASSERT(buff->begin <= buff->end);
  SHTTP_PROP(shttp_sock_next(sock, &req->id, buff, timeout));
  req->sock = sock;
  SHTTP_PROP(shttp_msg_request(req, (shttp_slice *)buff));
  if(sock->conn_timers) {
    shttp_timer_stop(req->id, sock->timer, sock->conn_timers, sock->conn_count);
  }
  return SHTTP_STATUS_OK;
}

shttp_status shttp_next_ignore(shttp_request req[static 1],
                               shttp_mut_slice buff[static 1],
                               shttp_socket sock[static 1], uint32_t timeout) {
  const shttp_mut_slice c = *buff;
  shttp_status status = shttp_next(req, buff, sock, timeout);
  *buff = c;
  return status;
}

shttp_status shttp_next_nblk(shttp_request req[static 1],
                             shttp_mut_slice buff[static 1],
                             shttp_socket sock[static 1]) {
  return shttp_next(req, buff, sock, 0);
}

shttp_status shttp_next_nblk_ignore(shttp_request req[static 1],
                                    shttp_mut_slice buff[static 1],
                                    shttp_socket sock[static 1]) {
  const shttp_mut_slice c = *buff;
  shttp_status status = shttp_next_nblk(req, buff, sock);
  *buff = c;
  return status;
}

void shttp_response_to_request(shttp_response res[static 1],
                               const shttp_request req[static 1]) {
  SHTTP_ASSERT(res);
  SHTTP_ASSERT(req);
  SHTTP_ASSERT(req->sock);
  res->id = req->id;
  res->code = 200;
  res->version = SHTTP_VERSION_1_1;
  res->sock = req->sock;
}

shttp_status shttp_send(shttp_mut_slice buff[static 1],
                        const shttp_response res[static 1]) {
  SHTTP_ASSERT(res);
  SHTTP_ASSERT(res->sock);
  SHTTP_ASSERT(buff);
  SHTTP_ASSERT(buff->begin <= buff->end);
  shttp_socket *sock = res->sock;
  shttp_slice s = {.begin = buff->begin};
  SHTTP_PROP(shttp_msg_response(buff, res));
  s.end = buff->end;
  SHTTP_PROP(shttp_sock_send(res->sock, s, res->id));
  if(sock->conn_timers != NULL) {
    return shttp_timer_start(res->id, sock->timer, sock->conn_timers,
                             sock->conn_count);
  }
  return SHTTP_STATUS_OK;
}

shttp_status shttp_send_ignore(shttp_mut_slice buff[static 1],
                               const shttp_response res[static 1]) {
  const shttp_mut_slice c = *buff;
  shttp_status status = shttp_send(buff, res);
  *buff = c;
  return status;
}

shttp_status shttp_close(shttp_socket sock[static 1], shttp_conn_id id) {
  SHTTP_ASSERT(sock);
  SHTTP_ASSERT(sock->conns);
  SHTTP_ASSERT(sock->conn_count > 0);
  SHTTP_ASSERT(id < sock->conn_count);
  if(sock->conn_timers)
    shttp_timer_stop(id, sock->timer, sock->conn_timers, sock->conn_count);
  return shttp_sock_close(sock, id);
}

shttp_status shttp_init(shttp_socket sock[static 1], uint32_t port) {
  SHTTP_ASSERT(sock);
  SHTTP_ASSERT(sock->conns);
  SHTTP_PROP(shttp_sock_init(sock, port));
  if(sock->conn_timers) {
    return shttp_timer_init(&sock->timer, sock->conn_timers, sock->conn_count);
  }
  return SHTTP_STATUS_OK;
}

shttp_status shttp_deinit(shttp_socket sock[static 1], bool force) {
  if(sock->conn_timers) shttp_timer_deinit(sock->timer);
  return shttp_sock_deinit(sock, force);
}
