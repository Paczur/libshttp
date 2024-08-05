#include "conn.h"

#include <arpa/inet.h>
#include <errno.h>
#include <poll.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

#define MSG_MAX 500
#define PORT 1337
#define BACKLOG_SIZE 2
#define RESPONSE_MSG "HTTP/1.1 204 No Content\n"
#define MAX_CONNS 10

struct pollfd sockfd = {.events = POLLIN};
struct pollfd connfds[MAX_CONNS];

char buff[MSG_MAX];

void conn_close(int *fd) {
  close(*fd);
  *fd = 0;
}

void conn_loop(void) {
  int poll_count;
  int counter;
  while(1) {
    poll_count = poll(&sockfd, 1, 0);
    counter = 0;
    for(size_t i = 0; i < MAX_CONNS && counter < poll_count; i++) {
      if(!connfds[i].fd) {
        connfds[i].fd = accept(sockfd.fd, NULL, NULL);
        counter++;
      }
    }

    for(size_t i = 0; i < MAX_CONNS; i++) {
      if(connfds[i].fd && poll(&connfds[i], 1, 0) > 0) {
        if(connfds[i].revents & POLLIN) {
          recv(connfds[i].fd, buff, sizeof(buff), 0);
          printf("%s\n", buff);

          send(connfds[i].fd, RESPONSE_MSG, sizeof(RESPONSE_MSG), 0);
          conn_close(&connfds[i].fd);
          puts(RESPONSE_MSG);
        }
      }
    }
  }
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
