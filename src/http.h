#ifndef HTTP_H
#define HTTP_H

#include <stddef.h>

typedef struct {
  size_t id;
} http_request_t;

typedef struct {
  size_t id;
} http_response_t;

void http_parse_request(http_request_t *req, const char *msg, size_t msg_len);
size_t http_parse_response(char *msg, size_t msg_len,
                           const http_response_t *res);
void http_response_to_request(http_response_t *res, const http_request_t *req);
int http_next(http_request_t *req, int timeout);
int http_next_nblk(http_request_t *req);
void http_send(const http_response_t *res);

void http_loop(void);
int http_init(void);
void http_deinit(void);

#endif
