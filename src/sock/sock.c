#include "sock.h"

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#include "../conf.h"

shttp_status shttp_sock_accept(shttp_socket sock[static 1], uint32_t timeout) {
  SHTTP_ASSERT(sock);
  SHTTP_ASSERT(sock->conns);
  SHTTP_ASSERT(sock->conn_count > 0);
  struct pollfd sockfd = {.fd = sock->sock, .events = POLLIN | POLLOUT};
  if(!poll(&sockfd, 1, timeout)) return SHTTP_STATUS_TIMEOUT;
  for(shttp_conn_id i = 0; i < sock->conn_count; i++) {
    if(sock->conns[i].fd == -1) {
      if(((sock->conns[i].fd = accept(sock->sock, NULL, NULL))) == -1)
        return SHTTP_STATUS_CONN_ACCEPT;
      return SHTTP_STATUS_OK;
    }
  }
  return SHTTP_STATUS_UNKNOWN_ERROR;
}

shttp_status shttp_sock_accept_nblk(shttp_socket sock[static 1]) {
  SHTTP_ASSERT(sock);
  SHTTP_ASSERT(sock->conns);
  SHTTP_ASSERT(sock->conn_count > 0);
  return shttp_sock_accept(sock, 0);
}

shttp_status shttp_sock_next(shttp_socket sock[static 1],
                             shttp_conn_id id[static 1],
                             shttp_mut_slice req[static 1], uint32_t timeout) {
  SHTTP_ASSERT(id);
  SHTTP_ASSERT(req);
  SHTTP_ASSERT(req->begin <= req->end);
  SHTTP_ASSERT(sock);
  SHTTP_ASSERT(sock->conns);
  ssize_t l;
  const struct timespec reqt = {
    .tv_sec = 0, .tv_nsec = 1000000000 / SHTTP_CONN_CHECKS_PER_SECOND};
  struct timespec rem;
  struct timespec rem2;
  if(req->begin == req->end) return SHTTP_STATUS_SLICE_END;
  for(uint32_t j = 0; j < timeout * SHTTP_CONN_CHECKS_PER_SECOND; j++) {
    if(nanosleep(&reqt, &rem) == -1) {
      while(1) {
        if(nanosleep(&rem, &rem2) != -1) break;
        if(nanosleep(&rem2, &rem) != -1) break;
      }
    }
    if(!shttp_sock_accept_nblk(sock)) {
    }
    for(shttp_conn_id i = 0; i < sock->conn_count; i++) {
      if(sock->conns[i].fd && poll(&sock->conns[i], 1, 0) > 0) {
        if(sock->conns[i].revents & (POLLERR | POLLHUP | POLLPRI)) {
          if(shttp_sock_close(sock, i)) {
            sock->conns[i].fd = -1;
          }
          sock->conns[i].fd = -1;
          sock->conns[i].events = 0;
          continue;
        }
        sock->conns[i].revents = 0;
        l = recv(sock->conns[i].fd, req->begin, req->end - req->begin, 0);
        if(l < 1) continue;
        req->end = req->begin + (l < 0 ? 0 : l);
        *id = i;
        return SHTTP_STATUS_OK;
      }
    }
  }
  return SHTTP_STATUS_TIMEOUT;
}

shttp_status shttp_sock_next_nblk(shttp_socket sock[static 1],
                                  shttp_conn_id id[static 1],
                                  shttp_mut_slice req[static 1]) {
  SHTTP_ASSERT(id);
  SHTTP_ASSERT(req);
  SHTTP_ASSERT(req->begin <= req->end);
  SHTTP_ASSERT(sock);
  SHTTP_ASSERT(sock->conns);
  SHTTP_ASSERT(sock->conn_count > 0);
  ssize_t l;
  if(req->begin == req->end) return SHTTP_STATUS_SLICE_END;
  for(shttp_conn_id i = 0; i < sock->conn_count; i++) {
    if(sock->conns[i].fd && poll(&sock->conns[i], 1, 0) > 0) {
      if(sock->conns[i].revents & (POLLERR | POLLHUP | POLLPRI)) {
        if(shttp_sock_close(sock, i)) {
          sock->conns[i].fd = -1;
        }
        sock->conns[i].fd = -1;
        sock->conns[i].events = 0;
        continue;
      }
      sock->conns[i].revents = 0;
      l = recv(sock->conns[i].fd, req->begin, req->end - req->begin, 0);
      if(l < 1) continue;
      req->end = req->begin + (l < 0 ? 0 : l);
      *id = i;
      return SHTTP_STATUS_OK;
    }
  }
  return SHTTP_STATUS_TIMEOUT;
}

shttp_status shttp_sock_send(shttp_socket sock[static 1], shttp_slice res,
                             shttp_conn_id id) {
  SHTTP_ASSERT(sock);
  SHTTP_ASSERT(sock->conns);
  SHTTP_ASSERT(sock->conn_count > 0);
  SHTTP_ASSERT(id < sock->conn_count);
  if(send(sock->conns[id].fd, res.begin, res.end - res.begin, 0) == -1)
    return SHTTP_STATUS_CONN_SEND;
  if(shttp_sock_accept_nblk(sock)) {
    /* Failing here isn't fatal and happens often
     * don't need to notify user */
  }
  return SHTTP_STATUS_OK;
}

shttp_status shttp_sock_close(shttp_socket sock[static 1], shttp_conn_id id) {
  SHTTP_ASSERT(sock);
  SHTTP_ASSERT(sock->conns);
  SHTTP_ASSERT(sock->conn_count > 0);
  SHTTP_ASSERT(id < sock->conn_count);
  SHTTP_ASSERT(sock->conns[id].fd >= 0);
  if(shutdown(sock->conns[id].fd, SHUT_RDWR)) SHTTP_ASSERT(errno == ENOTCONN);
  if(close(sock->conns[id].fd)) return SHTTP_STATUS_CONN_FD_CLOSE;
  sock->conns[id].fd = -1;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_sock_init(shttp_socket sock[static 1], uint32_t port) {
  SHTTP_ASSERT(sock);
  SHTTP_ASSERT(sock->conns);
  SHTTP_ASSERT(sock->conn_count > 0);
  int flag = 1;
  struct sockaddr_in servaddr = {.sin_family = AF_INET};
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(port);

  for(shttp_conn_id i = 0; i < sock->conn_count; i++) {
    sock->conns[i].fd = -1;
    sock->conns[i].events = POLLIN;
  }
  if(((sock->sock = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0))) < 0)
    return SHTTP_STATUS_SOCK_CREATE;
#ifdef NDEBUG
  setsockopt(sock->sock, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag));
#else
  SHTTP_ASSERT(!setsockopt(sock->sock, IPPROTO_TCP, TCP_NODELAY, (char *)&flag,
                           sizeof(flag)));
#endif
  if(bind(sock->sock, (struct sockaddr *)&servaddr, sizeof(servaddr)))
    return SHTTP_STATUS_SOCK_BIND;
  if(listen(sock->sock, SHTTP_SOCKET_BACKLOG_SIZE))
    return SHTTP_STATUS_SOCK_LISTEN;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_sock_deinit(shttp_socket sock[restrict 1], bool force) {
  SHTTP_ASSERT(sock);
  if(sock->conns) {
    for(shttp_conn_id i = 0; i < sock->conn_count; i++) {
      if(sock->conns[i].fd < 0) {
        if(close(sock->conns[i].fd) && !force)
          return SHTTP_STATUS_CONN_FD_CLOSE;
        sock->conns[i].fd = -1;
      }
    }
    sock->conns = NULL;
  }
  sock->conn_count = 0;
  if(sock->sock >= 0 && close(sock->sock)) return SHTTP_STATUS_SOCK_FD_CLOSE;
  sock->sock = -1;
  return SHTTP_STATUS_OK;
}
