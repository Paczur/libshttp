#include "conn.h"

#include <arpa/inet.h>
#include <errno.h>
#include <poll.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 69
#define BACKLOG_SIZE 2
#define MAX_CONNS 10

struct pollfd sockfd = {.events = POLLIN};
struct pollfd connfds[MAX_CONNS];

void conn_close(int *fd) {
  close(*fd);
  *fd = -1;
}

bool conn_accept(int timeout) {
  if(!poll(&sockfd, 1, timeout)) return false;
  for(size_t i = 0; i < MAX_CONNS; i++) {
    if(connfds[i].fd == -1) {
      connfds[i].fd = accept(sockfd.fd, NULL, NULL);
      return true;
    }
  }
  return false;
}

bool conn_accept_nblk(void) { return conn_accept(0); }

bool conn_id_valid(size_t id) { return id < MAX_CONNS; }

size_t conn_next(char *req, ssize_t *len, size_t max_len, int timeout) {
  for(size_t j = 0; j < 2; j++) {
    conn_accept(timeout / 4);
    if(poll(connfds, MAX_CONNS, timeout / 4) > 0) {
      for(size_t i = 0; i < MAX_CONNS; i++) {
        if(connfds[i].fd && poll(&connfds[i], 1, 0) > 0) {
          *len = recv(connfds[i].fd, req, max_len, 0);
          return i;
        }
      }
    }
  }
  *len = -1;
  return -1;
}

size_t conn_next_nblk(char *req, ssize_t *len, size_t max_len) {
  for(size_t i = 0; i < MAX_CONNS; i++) {
    if(connfds[i].fd && poll(&connfds[i], 1, 0) > 0) {
      *len = recv(connfds[i].fd, req, max_len, 0);
      return i;
    }
  }
  *len = -1;
  return -1;
}

void conn_send(const char *res, size_t res_len, size_t id) {
  send(connfds[id].fd, res, res_len, 0);
  conn_close(&connfds[id].fd);
  conn_accept_nblk();
}

int conn_init(void) {
  struct sockaddr_in servaddr = {.sin_family = AF_INET};
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(PORT);

  if((sockfd.fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) == -1) {
    printf("Socket creation failed, errno: %u\n", errno);
    return 1;
  }
  if((bind(sockfd.fd, (struct sockaddr *)&servaddr, sizeof(servaddr))) == -1) {
    printf("Socket bind failed, errno: %u\n", errno);
    return 1;
  }
  listen(sockfd.fd, BACKLOG_SIZE);
  for(size_t i = 0; i < MAX_CONNS; i++) {
    connfds[i].fd = -1;
    connfds[i].events = POLLIN | POLLOUT;
  }
  return 0;
}

void conn_deinit(void) {
  for(size_t i = 0; i < MAX_CONNS; i++) {
    if(connfds[i].fd) {
      close(connfds[i].fd);
      connfds[i].fd = 0;
    }
  }
  close(sockfd.fd);
  sockfd.fd = 0;
}
