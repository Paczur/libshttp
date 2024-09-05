#include "shttp.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "compose/compose.h"
#include "conf.h"
#include "conn.h"
#include "parse/parse.h"

shttp_status shttp_next(shttp_request req[static 1],
                        shttp_mut_slice buff[static 1], shttp_u16 timeout) {
  assert(req);
  assert(buff);
  assert(buff->begin <= buff->end);
  shttp_status status;
  if((status = shttp_conn_next(&req->id, buff, timeout))) return status;
  if((status = shttp_parse_request(req, (shttp_slice*)buff))) return status;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_next_nblk(shttp_request req[static 1],
                             shttp_mut_slice buff[static 1]) {
  assert(req);
  assert(buff);
  assert(buff->begin <= buff->end);
  return shttp_next(req, buff, 0);
}

void shttp_response_to_request(shttp_response res[static 1],
                               const shttp_request req[static 1]) {
  assert(res);
  assert(req);
  res->id = req->id;
  res->code = 200;
  res->version = SHTTP_VERSION_1_1;
}

shttp_status shttp_send(shttp_mut_slice buff[static 1],
                        const shttp_response res[static 1]) {
  assert(res);
  assert(buff);
  assert(buff->begin <= buff->end);
  shttp_status status;
  shttp_slice s = {.begin = buff->begin};
  if((status = shttp_compose_response(buff, res))) return status;
  s.end = buff->end;
  return shttp_conn_send(s, res->id);
}

shttp_status shttp_init(shttp_u16 port) { return shttp_conn_init(port); }

shttp_status shttp_deinit(bool force) { return shttp_conn_deinit(force); }
