#include "shttp.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "compose/compose.h"
#include "conf.h"
#include "parse/parse.h"
#include "sock.h"

shttp_status shttp_next(shttp_request req[static 1],
                        shttp_mut_slice buff[static 1], shttp_u16 timeout) {
  assert(req);
  assert(req->sock);
  assert(buff);
  assert(buff->begin <= buff->end);
  shttp_status status;
  if((status = shttp_sock_next(req->sock, &req->id, buff, timeout)))
    return status;
  if((status = shttp_parse_request(req, (shttp_slice *)buff))) return status;
  return SHTTP_STATUS_OK;
}

shttp_status shttp_next_nblk(shttp_request req[static 1],
                             shttp_mut_slice buff[static 1]) {
  assert(req);
  assert(req->sock);
  assert(buff);
  assert(buff->begin <= buff->end);
  return shttp_next(req, buff, 0);
}

void shttp_response_to_request(shttp_response res[static 1],
                               const shttp_request req[static 1]) {
  assert(res);
  assert(req);
  assert(req->sock);
  res->id = req->id;
  res->code = 200;
  res->version = SHTTP_VERSION_1_1;
  res->sock = req->sock;
}

shttp_status shttp_send(shttp_mut_slice buff[static 1],
                        const shttp_response res[static 1]) {
  assert(res);
  assert(res->sock);
  assert(buff);
  assert(buff->begin <= buff->end);
  shttp_status status;
  shttp_slice s = {.begin = buff->begin};
  if((status = shttp_compose_response(buff, res))) return status;
  s.end = buff->end;
  return shttp_sock_send(res->sock, s, res->id);
}

shttp_status shttp_init(shttp_socket sock[static 1], shttp_u16 port) {
  assert(sock);
  assert(sock->conns);
  return shttp_sock_init(sock, port);
}

shttp_status shttp_deinit(shttp_socket sock[static 1], bool force) {
  return shttp_sock_deinit(sock, force);
}
