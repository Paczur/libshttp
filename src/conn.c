#include "conn.h"

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#include "compose/compose.h"
#include "conf.h"
#include "parse/parse.h"
#include "private.h"

static struct pollfd sockfd = {.events = POLLIN};
static struct pollfd *connfds;
static shttp_u16 connfd_count;

shttp_status shttp_conn_close(int fd[static 1]) {
  if(close(*fd)) return SHTTP_STATUS_CONN_FD_CLOSE;
  *fd = -1;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_conn_accept(shttp_u16 timeout) {
  if(!poll(&sockfd, 1, timeout)) return SHTTP_STATUS_TIMEOUT;
  for(shttp_conn_id i = 0; i < connfd_count; i++) {
    if(connfds[i].fd == -1) {
      if(((connfds[i].fd = accept(sockfd.fd, NULL, NULL))) == -1)
        return SHTTP_STATUS_CONN_ACCEPT;
      return SHTTP_STATUS_OK;
    }
  }
  return SHTTP_STATUS_UNKNOWN_ERROR;
}

shttp_status shttp_conn_accept_nblk(void) { return shttp_conn_accept(0); }

shttp_status shttp_conn_next(shttp_conn_id id[static 1],
                             shttp_mut_slice req[static 1], shttp_u16 timeout) {
  assert(id);
  assert(req);
  assert(req->begin <= req->end);
  ssize_t l;
  const struct timespec reqt = {
    .tv_sec = 0, .tv_nsec = 1000000000 / SHTTP_CONN_CHECKS_PER_SECOND};
  struct timespec rem;
  struct timespec rem2;
  if(req->begin == req->end) return SHTTP_STATUS_SLICE_END;
  for(shttp_u32 j = 0; j < timeout * SHTTP_CONN_CHECKS_PER_SECOND; j++) {
    if(nanosleep(&reqt, &rem) == -1) {
      while(1) {
        if(nanosleep(&rem, &rem2) != -1) break;
        if(nanosleep(&rem2, &rem) != -1) break;
      }
    }
    if(shttp_conn_accept_nblk()) {
    }
    for(shttp_conn_id i = 0; i < connfd_count; i++) {
      if(connfds[i].fd && poll(&connfds[i], 1, 0) > 0) {
        l = recv(connfds[i].fd, req->begin, req->end - req->begin, 0);
        req->end = req->begin + (l < 0 ? 0 : l);
        *id = i;
        return SHTTP_STATUS_OK;
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
  for(shttp_conn_id i = 0; i < connfd_count; i++) {
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

void shttp_conn_fd_set(struct pollfd conns[static 1], shttp_u16 conn_count) {
  assert(conns);
  connfd_count = conn_count;
  connfds = conns;
}

shttp_status shttp_conn_init(shttp_u16 port, struct pollfd conns[static 1],
                             shttp_u16 conn_count) {
  assert(conns);
  struct sockaddr_in servaddr = {.sin_family = AF_INET};
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(port);
  connfd_count = conn_count;
  connfds = conns;

  if(((sockfd.fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0))) == -1)
    return SHTTP_STATUS_SOCK_CREATE;
  if(bind(sockfd.fd, (struct sockaddr *)&servaddr, sizeof(servaddr)))
    return SHTTP_STATUS_SOCK_BIND;
  if(listen(sockfd.fd, SHTTP_SOCKET_BACKLOG_SIZE))
    return SHTTP_STATUS_SOCK_LISTEN;
  for(shttp_conn_id i = 0; i < conn_count; i++) {
    connfds[i].fd = -1;
    connfds[i].events = POLLIN | POLLOUT;
  }
  return SHTTP_STATUS_OK;
}

shttp_status shttp_conn_deinit(bool force) {
  if(connfds) {
    for(shttp_conn_id i = 0; i < connfd_count; i++) {
      if(connfds[i].fd) {
        if(close(connfds[i].fd) && !force) return SHTTP_STATUS_CONN_FD_CLOSE;
        connfds[i].fd = 0;
      }
    }
    connfds = NULL;
  }
  connfd_count = 0;
  if(close(sockfd.fd)) return SHTTP_STATUS_SOCK_FD_CLOSE;
  sockfd.fd = 0;
  return SHTTP_STATUS_OK;
}
