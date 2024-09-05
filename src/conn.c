#include "conn.h"

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

#include "compose/compose.h"
#include "conf.h"
#include "parse/parse.h"
#include "private.h"

#warning "TODO)): Think about removing those variables"
static struct pollfd sockfd = {.events = POLLIN};
static struct pollfd connfds[SHTTP_MAX_CONNS];

shttp_status shttp_conn_close(int fd[static 1]) {
  if(close(*fd)) return SHTTP_STATUS_CONN_FD_CLOSE;
  *fd = -1;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_conn_accept(shttp_u16 timeout) {
  if(!poll(&sockfd, 1, timeout)) return SHTTP_STATUS_TIMEOUT;
  for(shttp_conn_id i = 0; i < SHTTP_MAX_CONNS; i++) {
    if(connfds[i].fd == -1) {
      if(((connfds[i].fd = accept(sockfd.fd, NULL, NULL))) == -1)
        return SHTTP_STATUS_CONN_ACCEPT;
      return SHTTP_STATUS_OK;
    }
  }
  return SHTTP_STATUS_UNKNOWN_ERROR;
}

shttp_status shttp_conn_accept_nblk(void) { return shttp_conn_accept(0); }

#warning "TODO)): Improve this implementation"
shttp_status shttp_conn_next(shttp_conn_id id[static 1],
                             shttp_mut_slice req[static 1], shttp_u16 timeout) {
  assert(id);
  assert(req);
  assert(req->begin <= req->end);
  ssize_t l;
  if(req->begin == req->end) return SHTTP_STATUS_SLICE_END;
  for(shttp_u8 j = 0; j < 2; j++) {
    if(shttp_conn_accept(timeout / 4)) {
    }
    if(poll(connfds, SHTTP_MAX_CONNS, timeout / 4) > 0) {
      for(shttp_conn_id i = 0; i < SHTTP_MAX_CONNS; i++) {
        if(connfds[i].fd && poll(&connfds[i], 1, 0) > 0) {
          l = recv(connfds[i].fd, req->begin, req->end - req->begin, 0);
          req->end = req->begin + (l < 0 ? 0 : l);
          *id = i;
          return SHTTP_STATUS_OK;
        }
      }
    }
  }
  return SHTTP_STATUS_TIMEOUT;
}

shttp_status shttp_conn_next_nblk(shttp_conn_id id[static 1],
                                  shttp_mut_slice req[static 1]) {
  assert(id);
  assert(req);
  assert(req->begin <= req->end);
  ssize_t l;
  if(req->begin == req->end) return SHTTP_STATUS_SLICE_END;
  for(shttp_conn_id i = 0; i < SHTTP_MAX_CONNS; i++) {
    if(connfds[i].fd && poll(&connfds[i], 1, 0) > 0) {
      l = recv(connfds[i].fd, req->begin, req->end - req->begin, 0);
      req->end = req->begin + (l < 0 ? 0 : l);
      *id = i;
      return SHTTP_STATUS_OK;
    }
  }
  return SHTTP_STATUS_TIMEOUT;
}

shttp_status shttp_conn_send(shttp_slice res, shttp_conn_id id) {
  shttp_status status;
  if(send(connfds[id].fd, res.begin, res.end - res.begin - 1, 0) == -1)
    return SHTTP_STATUS_CONN_SEND;
  if(((status = shttp_conn_close(&connfds[id].fd)))) return status;
  if(shttp_conn_accept_nblk()) {
#warning "TODO)): Decide about logging non fatal errors/warnings"
  }
  return SHTTP_STATUS_OK;
}

shttp_status shttp_conn_init(shttp_u16 port) {
  struct sockaddr_in servaddr = {.sin_family = AF_INET};
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(port);

  if(((sockfd.fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0))) == -1)
    return SHTTP_STATUS_SOCK_CREATE;
  if(bind(sockfd.fd, (struct sockaddr *)&servaddr, sizeof(servaddr)))
    return SHTTP_STATUS_SOCK_BIND;
  if(listen(sockfd.fd, SHTTP_SOCKET_BACKLOG_SIZE))
    return SHTTP_STATUS_SOCK_LISTEN;
  for(shttp_conn_id i = 0; i < SHTTP_MAX_CONNS; i++) {
    connfds[i].fd = -1;
    connfds[i].events = POLLIN | POLLOUT;
  }
  return SHTTP_STATUS_OK;
}

shttp_status shttp_conn_deinit(bool force) {
  for(shttp_conn_id i = 0; i < SHTTP_MAX_CONNS; i++) {
    if(connfds[i].fd) {
      if(close(connfds[i].fd) && !force) return SHTTP_STATUS_CONN_FD_CLOSE;
      connfds[i].fd = 0;
    }
  }
  if(close(sockfd.fd)) return SHTTP_STATUS_SOCK_FD_CLOSE;
  sockfd.fd = 0;
  return SHTTP_STATUS_OK;
}
