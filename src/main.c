#include <stdio.h>

#include "shttp.h"

#define BUFF_LENGTH 500
#define MAX_CONNS 10
static shttp_response res;
static shttp_request req;
static char buff[BUFF_LENGTH];
static struct pollfd conns[MAX_CONNS];

int main(void) {
  const shttp_mut_slice csbuff = SHTTP_SLICE(buff);
  shttp_mut_slice sbuff = SHTTP_SLICE(buff);
  shttp_status status = SHTTP_STATUS_OK;

  for(shttp_u16 port = 1337; port < 65535; port++) {
    status = shttp_init(port, conns, MAX_CONNS);
    switch(status) {
    case SHTTP_STATUS_SOCK_CREATE:
      puts("Error creating socket");
      break;
    case SHTTP_STATUS_SOCK_BIND:
      puts("Error binding socket to address");
      break;
    case SHTTP_STATUS_SOCK_LISTEN:
      puts("Error setting socket to listen mode");
      break;
    case SHTTP_STATUS_OK:
      printf("Connected to port: %u\n", port);
      goto success;
    default:
      break;
    }
    status = shttp_deinit(false);
  }
  return status;
success:
  while(true) {
    status = shttp_next(&req, &sbuff, 5000);
    sbuff = csbuff;
    switch(status) {
    case SHTTP_STATUS_SLICE_END:
      puts("Request too large for slice provided");
      continue;
    case SHTTP_STATUS_PREFIX_INVALID:
    case SHTTP_STATUS_VALUE_INVALID:
    case SHTTP_STATUS_NEWLINE_EXPECTED:
    case SHTTP_STATUS_SPACE_EXPECTED:
      puts("Malformed request");
      continue;
    case SHTTP_STATUS_OK:
      break;
    default:
      continue;
    }

    shttp_response_to_request(&res, &req);

    status = shttp_send(&sbuff, &res);
    sbuff = csbuff;
    switch(status) {
    case SHTTP_STATUS_CONN_SEND:
      puts("Couldn't send message");
      break;
    case SHTTP_STATUS_CONN_FD_CLOSE:
      puts("Couldn't close file descriptor used for connection");
      break;
    case SHTTP_STATUS_TIMEOUT:
      puts("Timeout reached waiting for connection");
      break;
    case SHTTP_STATUS_CONN_ACCEPT:
      puts("Error when accepting connection");
      break;
    default:
      break;
    }
  }
  if((status = shttp_deinit(false))) {
    switch(status) {
    case SHTTP_STATUS_CONN_FD_CLOSE:
      puts("Couldn't close connection");
      break;
    case SHTTP_STATUS_SOCK_FD_CLOSE:
      puts("Couldn't close socket");
      break;
    default:
      break;
    }
    if((status = shttp_deinit(true))) {
      return status;
    }
  }
  return SHTTP_STATUS_OK;
}
