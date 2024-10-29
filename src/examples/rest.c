#include <stdio.h>

#define SHTTP_UNUSED_RESULT  // disables required error handling
#include "../shttp.h"

#define MSG_BUFF_LENGTH 500
#define BODY_BUFF_LENGTH 300
#define ARR_CAPACITY 10
#define MAX_CONNS 10
#define PORT 80

static char msg_buff[MSG_BUFF_LENGTH];
static char body_buff[BODY_BUFF_LENGTH];
static shttp_response res;
static shttp_request req;
static shttp_conn conns[MAX_CONNS];
static shttp_socket sock = {.conns = conns, .conn_count = MAX_CONNS};
static shttp_mut_slice msg_slice = SHTTP_MUT_SLICE(msg_buff);
static shttp_mut_slice body_slice = SHTTP_MUT_SLICE(body_buff);

static int32_t arr[ARR_CAPACITY];
static uint32_t arr_size = 0;

static void options(void) {
  shttp_slice_cpy(
    &body_slice,
    SHTTP_SLICE("{\r\n"
                "\t\"requests\": [\r\n"
                "\t\t{\"method\": \"OPTIONS\", \"path\": \"/\"},\r\n"
                "\t\t{\"method\": \"GET\", \"path\": \"/{id}\"},\r\n"
                "\t\t{\"method\": \"DELETE\", \"path\": \"/{id}\"},\r\n"
                "\t\t{\"method\": \"PATCH\", \"path\": \"/{id}:{val}\"},\r\n"
                "\t\t{\"method\": \"PUT\", \"path\": \"/{val}\"}\r\n"
                "\t]\r\n"
                "}"));
}

static shttp_status print_entry(uint32_t id) {
  return shttp_slice_insert_format_delim(
    &body_slice, SHTTP_SLICE("{\"id\": (u32), \"value\": (i32)}"), '(', ')', id,
    arr[id]);
}

static void create(uint16_t val) {
  if(arr_size == ARR_CAPACITY) {
    res.code = SHTTP_CODE_INSUFFICIENT_STORAGE;
    return;
  }
  for(uint32_t i = 0; i < ARR_CAPACITY; i++) {
    if(arr[i] == -1) {
      arr[i] = val;
      arr_size++;
      if(print_entry(i)) res.code = SHTTP_CODE_INTERNAL_SERVER_ERROR;
      return;
    }
  }
  res.code = SHTTP_CODE_INTERNAL_SERVER_ERROR;
  return;
}

static void read(uint32_t id) {
  if(arr_size == 0 || arr[id] == -1) {
    res.code = SHTTP_CODE_BAD_REQUEST;
    return;
  }
  if(print_entry(id)) {
    res.code = SHTTP_CODE_INTERNAL_SERVER_ERROR;
    return;
  }
}

static void update(uint32_t id, uint16_t val) {
  if(arr[id] == -1) {
    res.code = SHTTP_CODE_NOT_FOUND;
    return;
  }
  arr[id] = val;
  if(print_entry(id)) res.code = SHTTP_CODE_BAD_GATEWAY;
}

static void delete(uint32_t id) {
  arr_size -= (arr[id] != -1);
  arr[id] = -1;
}

static void dispatch(void) {
  uint32_t id = 0;
  int32_t val = 0;
  shttp_slice path_slice = req.path;
  res.body = (shttp_slice){0};
  body_slice = SHTTP_MUT_SLICE(body_buff);
  shttp_slice_skip(&path_slice, 1);
  switch(req.method) {
  case SHTTP_METHOD_OPTIONS:
    options();
    break;
  case SHTTP_METHOD_GET:
    shttp_slice_parse_u32(&id, &path_slice);
    read(id);
    break;
  case SHTTP_METHOD_PUT:
    shttp_slice_parse_i32(&val, &path_slice);
    create(val);
    break;
  case SHTTP_METHOD_PATCH:
    shttp_slice_parse_format(&path_slice, SHTTP_SLICE("{u32}:{i32}"), &id,
                             &val);
    update(id, val);
    break;
  case SHTTP_METHOD_DELETE:
    shttp_slice_parse_u32(&id, &path_slice);
    delete(id);
    break;
  default:
    res.code = SHTTP_CODE_BAD_GATEWAY;
    break;
  }
  res.body.begin = body_buff;
  res.body.end = body_slice.begin;
}

int main(void) {
  SHTTP_PROP(shttp_init(&sock, PORT));
  for(uint32_t i = 0; i < ARR_CAPACITY; i++) arr[i] = -1;

  while(true) {
    if(shttp_next_ignore(&req, &msg_slice, &sock, 10)) continue;
    shttp_response_to_request(&res, &req);
    dispatch();
    shttp_send_ignore(&msg_slice, &res);
    shttp_close(&sock, res.id);
  }

  return shttp_deinit(&sock, true);
}
