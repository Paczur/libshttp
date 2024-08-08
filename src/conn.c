#include "conn.h"

#include <arpa/inet.h>
#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

#include "conf.h"

struct pollfd sockfd = {.events = POLLIN};
struct pollfd connfds[MAX_CONNS];

void shttp_conn_close(int *fd) {
  close(*fd);
  *fd = -1;
}

bool shttp_conn_accept(shttp_u16 timeout) {
  if(!poll(&sockfd, 1, timeout)) return false;
  for(shttp_conn_id i = 0; i < MAX_CONNS; i++) {
    if(connfds[i].fd == -1) {
      connfds[i].fd = accept(sockfd.fd, NULL, NULL);
      return true;
    }
  }
  return false;
}

bool shttp_conn_accept_nblk(void) { return shttp_conn_accept(0); }

bool shttp_conn_id_valid(shttp_conn_id id) { return id < MAX_CONNS; }

shttp_conn_id shttp_conn_next(char *req, shttp_u16 *len, shttp_u16 max_len,
                              shttp_u16 timeout) {
  shttp_s32 l;
  for(shttp_u8 j = 0; j < 2; j++) {
    shttp_conn_accept(timeout / 4);
    if(poll(connfds, MAX_CONNS, timeout / 4) > 0) {
      for(shttp_conn_id i = 0; i < MAX_CONNS; i++) {
        if(connfds[i].fd && poll(&connfds[i], 1, 0) > 0) {
          l = recv(connfds[i].fd, req, max_len, 0);
          *len = l < 0 ? 0 : l;
          return i;
        }
      }
    }
  }
  *len = 0;
  return MAX_CONNS + 1;
}

shttp_conn_id shttp_conn_next_nblk(char *req, shttp_u16 *len,
                                   shttp_u16 max_len) {
  for(shttp_conn_id i = 0; i < MAX_CONNS; i++) {
    if(connfds[i].fd && poll(&connfds[i], 1, 0) > 0) {
      *len = recv(connfds[i].fd, req, max_len, 0);
      return i;
    }
  }
  *len = 0;
  return MAX_CONNS + 1;
}

void shttp_conn_send(const char *res, shttp_u16 res_len, shttp_conn_id id) {
  send(connfds[id].fd, res, res_len, 0);
  shttp_conn_close(&connfds[id].fd);
  shttp_conn_accept_nblk();
}

bool shttp_conn_init(void) {
  struct sockaddr_in servaddr = {.sin_family = AF_INET};
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(PORT);

  if((sockfd.fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) == -1) {
    printf("Socket creation failed, errno: %u\n", errno);
    return true;
  }
  if((bind(sockfd.fd, (struct sockaddr *)&servaddr, sizeof(servaddr))) == -1) {
    printf("Socket bind failed, errno: %u\n", errno);
    return true;
  }
  listen(sockfd.fd, SOCKET_BACKLOG_SIZE);
  for(shttp_conn_id i = 0; i < MAX_CONNS; i++) {
    connfds[i].fd = -1;
    connfds[i].events = POLLIN | POLLOUT;
  }
  return false;
}

void shttp_conn_deinit(void) {
  for(shttp_conn_id i = 0; i < MAX_CONNS; i++) {
    if(connfds[i].fd) {
      close(connfds[i].fd);
      connfds[i].fd = 0;
    }
  }
  close(sockfd.fd);
  sockfd.fd = 0;
}
