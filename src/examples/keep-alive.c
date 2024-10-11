#include <stdio.h>

#include "../shttp.h"

#define BUFF_LENGTH 500
#define HEADER_BUFF_LENGTH 100
#define MAX_CONNS 10

static char buff[BUFF_LENGTH];
static char header_buff[HEADER_BUFF_LENGTH];
static shttp_response res;
static shttp_request req;
static int conns[MAX_CONNS];
static shttp_socket sock = {.conns = conns, .conn_count = MAX_CONNS};

static shttp_status keep_alive(void) {
  res.headers.begin = header_buff;
  res.headers.end = header_buff + HEADER_BUFF_LENGTH;
  SHTTP_PROP(
    shttp_compose_slice_cpy((shttp_mut_slice *)&res.headers,
                            SHTTP_SLICE("Keep-Alive: timeout=1, max=1")));
  SHTTP_PROP(shttp_compose_slice_newline((shttp_mut_slice *)&res.headers));
  SHTTP_PROP(shttp_compose_slice_cpy((shttp_mut_slice *)&res.headers,
                                     SHTTP_SLICE("Content-Length: 0")));
  SHTTP_PROP(shttp_compose_slice_newline((shttp_mut_slice *)&res.headers));
  return SHTTP_STATUS_OK;
}

static shttp_status headers(void) {
  shttp_status status;
  if((status = keep_alive())) {
    puts("Couldn't add keep-alive to the header");
    res.headers.end = header_buff;
  } else {
    res.headers.end = res.headers.begin;
  }
  res.headers.begin = header_buff;
  return status;
}

static shttp_status get(shttp_mut_slice *sbuff, const shttp_mut_slice csbuff) {
  shttp_status status = shttp_next(&req, sbuff, 10);
  *sbuff = csbuff;
  switch(status) {
  case SHTTP_STATUS_SLICE_END:
    puts("Request too large for slice provided");
    break;
  case SHTTP_STATUS_PREFIX_INVALID:
  case SHTTP_STATUS_VALUE_INVALID:
  case SHTTP_STATUS_NEWLINE_EXPECTED:
  case SHTTP_STATUS_SPACE_EXPECTED:
    puts("Malformed request");
    break;
  default:
    break;
  }
  return status;
}

static shttp_status send(shttp_mut_slice *sbuff, const shttp_mut_slice csbuff) {
  shttp_status status = shttp_send(sbuff, &res);
  *sbuff = csbuff;
  switch(status) {
  case SHTTP_STATUS_CONN_SEND:
    puts("Couldn't send message");
    break;
  case SHTTP_STATUS_TIMEOUT:
    puts("Timeout reached waiting for connection");
    break;
  case SHTTP_STATUS_CONN_ACCEPT:
    puts("Error when accepting connection");
    break;
  case SHTTP_STATUS_VALUE_INVALID:
    puts("Invalid value passed in response struct");
    break;
  case SHTTP_STATUS_SLICE_END:
    puts("Buffer too small for response");
    break;
  default:
    break;
  }
  return status;
}

static shttp_status init(void) {
  shttp_status status;
  for(shttp_u16 port = 1337; port < 65535; port++) {
    status = shttp_init(&sock, port);
    switch(status) {
    case SHTTP_STATUS_SOCK_CREATE:
      puts("Error creating socket");
      break;
    case SHTTP_STATUS_SOCK_BIND:
      printf("Error binding socket to port: %u\n", port);
      break;
    case SHTTP_STATUS_SOCK_LISTEN:
      puts("Error setting socket to listen mode");
      break;
    case SHTTP_STATUS_OK:
      printf("Connected to port: %u\n", port);
      return SHTTP_STATUS_OK;
    default:
      break;
    }
    if(shttp_deinit(&sock, false)) {
    }
  }
  return status;
}

static shttp_status deinit(void) {
  shttp_status status;
  if((status = shttp_deinit(&sock, false))) {
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
    if((status = shttp_deinit(&sock, true))) {
      return status;
    }
  }
}

int main(void) {
  const shttp_mut_slice csbuff = SHTTP_MUT_SLICE(buff);
  shttp_mut_slice sbuff = SHTTP_MUT_SLICE(buff);
  SHTTP_PROP(init());
  req.sock = &sock;
  while(true) {
    if(get(&sbuff, csbuff)) continue;
    shttp_response_to_request(&res, &req);
    headers();
    send(&sbuff, csbuff);
  }
  return deinit();
}
