#include "http.h"

#include <stdio.h>
#include <string.h>

#include "conn.h"

#define PLACEHOLDER_MSG "HTTP/1.1 200 OK\n"
#define REQUEST_LENGTH 500
#define RESPONSE_LENGTH 500

#define MIN(x, y) (((x) > (y)) ? (y) : (x))

char request[REQUEST_LENGTH];
char response[RESPONSE_LENGTH];

void http_parse_request(http_request_t *req, const char *msg, size_t msg_len) {
  (void)req;
  printf("%.*s", (int)msg_len, msg);
}

size_t http_parse_response(char *msg, size_t msg_len,
                           const http_response_t *res) {
  (void)res;
  strncpy(msg, PLACEHOLDER_MSG, msg_len);
  return MIN(msg_len, sizeof(PLACEHOLDER_MSG));
}

int http_next(http_request_t *req, int timeout) {
  ssize_t len;
  req->id = conn_next(request, &len, REQUEST_LENGTH, timeout);
  if(!conn_id_valid(req->id)) return -1;
  http_parse_request(req, request, len);
  return 0;
}

int http_next_nblk(http_request_t *req) { return http_next(req, 0); }

void http_response_to_request(http_response_t *res, const http_request_t *req) {
  res->id = req->id;
}

void http_send(const http_response_t *res) {
  size_t len = http_parse_response(response, RESPONSE_LENGTH, res);
  conn_send(response, len, res->id);
}

int http_init(void) { return conn_init(); }

void http_deinit(void) { conn_deinit(); }
