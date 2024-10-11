#include "sock.h"

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#include "compose/compose.h"
#include "conf.h"
#include "parse/parse.h"
#include "private.h"

shttp_status shttp_sock_accept(shttp_socket sock[static 1], shttp_u16 timeout) {
  assert(sock);
  assert(sock->conns);
  assert(sock->conn_count > 0);
  struct pollfd sockfd = {.fd = sock->sock, .events = POLLIN | POLLOUT};
  if(!poll(&sockfd, 1, timeout)) return SHTTP_STATUS_TIMEOUT;
  for(shttp_conn_id i = 0; i < sock->conn_count; i++) {
    if(sock->conns[i] == -1) {
      if(((sock->conns[i] = accept(sock->sock, NULL, NULL))) == -1)
        return SHTTP_STATUS_CONN_ACCEPT;
      return SHTTP_STATUS_OK;
    }
  }
  return SHTTP_STATUS_UNKNOWN_ERROR;
}

shttp_status shttp_sock_accept_nblk(shttp_socket sock[static 1]) {
  assert(sock);
  assert(sock->conns);
  assert(sock->conn_count > 0);
  return shttp_sock_accept(sock, 0);
}

shttp_status shttp_sock_next(shttp_socket sock[static 1],
                             shttp_conn_id id[static 1],
                             shttp_mut_slice req[static 1], shttp_u16 timeout) {
  assert(id);
  assert(req);
  assert(req->begin <= req->end);
  assert(sock);
  assert(sock->conns);
  assert(sock->conn_count > 0);
  assert(sock->conn_count <= 255);
  struct pollfd connfds[SHTTP_CONN_ID_MAX];
  for(shttp_conn_id i = 0; i < sock->conn_count; i++) {
    connfds[i].fd = sock->conns[i];
    connfds[i].events = POLLIN;
  }
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
      for(shttp_conn_id i = 0; i < sock->conn_count; i++)
        connfds[i].fd = sock->conns[i];
    }
    if(!shttp_sock_accept_nblk(sock)) {
      for(shttp_conn_id i = 0; i < sock->conn_count; i++) {
        connfds[i].fd = sock->conns[i];
      }
    }
    for(shttp_conn_id i = 0; i < sock->conn_count; i++) {
      if(sock->conns[i] && poll(connfds + i, 1, 0) > 0) {
        if(connfds[i].revents & (POLLERR | POLLHUP | POLLPRI)) {
          if(shttp_sock_close(sock, i)) {
            sock->conns[i] = -1;
          }
          connfds[i].fd = -1;
          connfds[i].events = 0;
          continue;
        }
        connfds[i].revents = 0;
        l = recv(sock->conns[i], req->begin, req->end - req->begin, 0);
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
  assert(id);
  assert(req);
  assert(req->begin <= req->end);
  assert(sock);
  assert(sock->conns);
  assert(sock->conn_count > 0);
  assert(sock->conn_count <= SHTTP_CONN_ID_MAX);
  struct pollfd connfds[SHTTP_CONN_ID_MAX];
  for(shttp_conn_id i = 0; i < sock->conn_count; i++) {
    connfds[i].fd = sock->conns[i];
    connfds[i].events = POLLIN;
  }
  ssize_t l;
  if(req->begin == req->end) return SHTTP_STATUS_SLICE_END;
  for(shttp_conn_id i = 0; i < sock->conn_count; i++) {
    if(sock->conns[i] && poll(connfds + i, 1, 0) > 0) {
      if(connfds[i].revents & (POLLERR | POLLHUP | POLLPRI)) {
        if(shttp_sock_close(sock, i)) {
          sock->conns[i] = -1;
        }
        connfds[i].fd = -1;
        connfds[i].events = 0;
        continue;
      }
      connfds[i].revents = 0;
      l = recv(sock->conns[i], req->begin, req->end - req->begin, 0);
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
  assert(sock);
  assert(sock->conns);
  assert(sock->conn_count > 0);
  assert(id < sock->conn_count);
  if(send(sock->conns[id], res.begin, res.end - res.begin, 0) == -1)
    return SHTTP_STATUS_CONN_SEND;
  if(shttp_sock_accept_nblk(sock)) {
    /* Failing here isn't fatal and happens often
     * don't need to notify user */
  }
  return SHTTP_STATUS_OK;
}

shttp_status shttp_sock_close(shttp_socket sock[static 1], shttp_conn_id id) {
  assert(sock);
  assert(sock->conns);
  assert(sock->conn_count > 0);
  assert(id < sock->conn_count);
  assert(sock->conns[id] >= 0);
  if(shutdown(sock->conns[id], SHUT_RDWR)) return SHTTP_STATUS_SOCK_SHUTDOWN;
  if(close(sock->conns[id])) return SHTTP_STATUS_CONN_FD_CLOSE;
  sock->conns[id] = -1;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_sock_init(shttp_socket sock[static 1], shttp_u16 port) {
  assert(sock);
  assert(sock->conns);
  assert(sock->conn_count > 0);
  int flag = 1;
  struct sockaddr_in servaddr = {.sin_family = AF_INET};
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(port);

  for(shttp_conn_id i = 0; i < sock->conn_count; i++) sock->conns[i] = -1;
  if(((sock->sock = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0))) < 0)
    return SHTTP_STATUS_SOCK_CREATE;
  if(setsockopt(sock->sock, IPPROTO_TCP, TCP_NODELAY, (char *)&flag,
                sizeof(flag)))
    return SHTTP_STATUS_SOCK_OPT;
  if(bind(sock->sock, (struct sockaddr *)&servaddr, sizeof(servaddr)))
    return SHTTP_STATUS_SOCK_BIND;
  if(listen(sock->sock, SHTTP_SOCKET_BACKLOG_SIZE))
    return SHTTP_STATUS_SOCK_LISTEN;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_sock_deinit(shttp_socket sock[restrict 1], bool force) {
  assert(sock);
  if(sock->conns) {
    for(shttp_conn_id i = 0; i < sock->conn_count; i++) {
      if(sock->conns[i] < 0) {
        if(close(sock->conns[i]) && !force) return SHTTP_STATUS_CONN_FD_CLOSE;
        sock->conns[i] = -1;
      }
    }
    sock->conns = NULL;
  }
  sock->conn_count = 0;
  if(sock->sock >= 0 && close(sock->sock)) return SHTTP_STATUS_SOCK_FD_CLOSE;
  sock->sock = -1;
  return SHTTP_STATUS_OK;
}
