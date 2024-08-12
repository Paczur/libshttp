#include "shttp.h"

#include <stdio.h>
#include <string.h>

#include "compose/compose.h"
#include "conf.h"
#include "conn.h"
#include "parse/parse.h"

char request[REQUEST_LENGTH];
char response[RESPONSE_LENGTH];

bool shttp_next(shttp_request *req, shttp_u16 timeout) {
  shttp_u16 len;
  req->id = shttp_conn_next(request, &len, REQUEST_LENGTH, timeout);
  if(!shttp_conn_id_valid(req->id)) return true;
  printf("%.*s\n", len, request);
  shttp_parse_request(req, request, len);
  return false;
}

bool shttp_next_nblk(shttp_request *req) { return shttp_next(req, 0); }

void shttp_response_to_request(shttp_response *res, const shttp_request *req) {
  res->id = req->id;
}

void shttp_send(const shttp_response *res) {
  shttp_u16 len = shttp_compose_response(response, RESPONSE_LENGTH, res);
  shttp_conn_send(response, len, res->id);
}

bool shttp_init(void) { return shttp_conn_init(); }

void shttp_deinit(void) { shttp_conn_deinit(); }
