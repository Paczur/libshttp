#include "shttp.h"

#include <stdio.h>
#include <string.h>

#include "compose/compose.h"
#include "conf.h"
#include "conn.h"
#include "parse/parse.h"

char request[SHTTP_REQUEST_LENGTH];
char response[SHTTP_RESPONSE_LENGTH];

bool shttp_next(shttp_request req[static 1], shttp_u16 timeout) {
  shttp_reqi len;
  req->id = shttp_conn_next(request, &len, SHTTP_REQUEST_LENGTH, timeout);
  if(!shttp_conn_id_valid(req->id)) return true;
  shttp_parse_request(req, &(shttp_slice){request, request + len});
  return false;
}

bool shttp_next_nblk(shttp_request req[static 1]) { return shttp_next(req, 0); }

void shttp_response_to_request(shttp_response res[static 1],
                               const shttp_request req[static 1]) {
  res->id = req->id;
  res->code = 200;
  res->version = SHTTP_VERSION_1_1;
}

void shttp_send(const shttp_response res[static 1]) {
  shttp_u16 len = shttp_compose_response(response, SHTTP_RESPONSE_LENGTH, res);
  shttp_conn_send(response, len, res->id);
}

bool shttp_init(void) { return shttp_conn_init(); }

void shttp_deinit(void) { shttp_conn_deinit(); }
