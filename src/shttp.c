#include "shttp.h"

#include <stdio.h>
#include <string.h>

#include "conn.h"

#define PLACEHOLDER_MSG "HTTP/1.1 200 OK\n"
#define REQUEST_LENGTH 500
#define RESPONSE_LENGTH 500

#define MIN(x, y) (((x) > (y)) ? (y) : (x))

char request[REQUEST_LENGTH];
char response[RESPONSE_LENGTH];

void shttp_parse_request(shttp_request *req, const char *msg,
                         shttp_u16 msg_len) {
  (void)req;
  printf("%.*s", (int)msg_len, msg);
}

shttp_u16 shttp_parse_response(char *msg, shttp_u16 msg_len,
                               const shttp_response *res) {
  (void)res;
  strncpy(msg, PLACEHOLDER_MSG, msg_len);
  return MIN(msg_len, sizeof(PLACEHOLDER_MSG));
}

bool shttp_next(shttp_request *req, shttp_u16 timeout) {
  shttp_u16 len;
  req->id = shttp_conn_next(request, &len, REQUEST_LENGTH, timeout);
  if(!shttp_conn_id_valid(req->id)) return true;
  shttp_parse_request(req, request, len);
  return false;
}

bool shttp_next_nblk(shttp_request *req) { return shttp_next(req, 0); }

void shttp_response_to_request(shttp_response *res, const shttp_request *req) {
  res->id = req->id;
}

void shttp_send(const shttp_response *res) {
  shttp_u16 len = shttp_parse_response(response, RESPONSE_LENGTH, res);
  shttp_conn_send(response, len, res->id);
}

bool shttp_init(void) { return shttp_conn_init(); }

void shttp_deinit(void) { shttp_conn_deinit(); }
