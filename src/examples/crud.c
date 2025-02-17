#include <stdio.h>

#define SHTTP_UNUSED_RESULT  // disables required error handling
#include "../shttp.h"

#define MSG_BUFF_LENGTH 1000
#define BODY_BUFF_LENGTH 700
#define HEADERS_BUFF_LENGTH 50
#define ARR_CAPACITY 10
#define MAX_CONNS 10
#define PORT 1400

static char msg_buff[MSG_BUFF_LENGTH];
static char body_buff[BODY_BUFF_LENGTH];
static char headers_buff[HEADERS_BUFF_LENGTH];
static shttp_response res;
static shttp_request req;
static shttp_conn conns[MAX_CONNS];
static shttp_socket sock = {.conns = conns, .conn_count = MAX_CONNS};
static shttp_mut_slice msg_slice = SHTTP_MUT_SLICE(msg_buff);
static shttp_mut_slice body_slice = SHTTP_MUT_SLICE(body_buff);
static shttp_mut_slice headers_slice = SHTTP_MUT_SLICE(headers_buff);

static int32_t arr[ARR_CAPACITY];
static uint32_t arr_size = 0;

static void main_page(void) {
  shttp_slice_cpy(&body_slice, SHTTP_SLICE("<!DOCTYPE html>"
                                           "<html>"
                                           "<head>"
                                           "</head>"
                                           "<body>"

                                           "<form action=\"create\">"
                                           "<label>"
                                           "value: "
                                           "</label>"
                                           "<input name=\"value\"></input>"
                                           "<button type=\"submit\">"
                                           "create"
                                           "</button>"
                                           "</form>"

                                           "<form action=\"update\">"
                                           "<label>"
                                           "id: "
                                           "</label>"
                                           "<input name=\"id\"></input>"
                                           "<label>"
                                           "value: "
                                           "</label>"
                                           "<input name=\"value\"></input>"
                                           "<button type=\"submit\">"
                                           "update"
                                           "</button>"
                                           "</form>"

                                           "<form action=\"delete\">"
                                           "<label>"
                                           "id: "
                                           "</label>"
                                           "<input name=\"id\"></input>"
                                           "<button type=\"submit\">"
                                           "delete"
                                           "</button>"
                                           "</form>"

                                           "<table>"
                                           "<tr>"
                                           "<th>id</th>"
                                           "<th>value</th>"
                                           "</tr>"));
  for(uint32_t i = 0; i < arr_size; i++) {
    shttp_slice_insert_format(&body_slice,
                              SHTTP_SLICE("<tr>"
                                          "<td>{u32}</td>"
                                          "<td>{u16}</td>"
                                          "</tr>"),
                              i, arr[i]);
  }
  shttp_slice_cpy(&body_slice, SHTTP_SLICE("</table>"
                                           "</body>"
                                           "</html>"));
}

static void list(void) {
  uint32_t i;
  if(arr_size == 0) return;
  for(i = 0; i < ARR_CAPACITY; i++) {
    if(arr[i] != -1) {
      if(shttp_slice_insert_format(&body_slice, SHTTP_SLICE("{u32}:{u16}"), i,
                                   arr[i])) {
        res.code = SHTTP_CODE_INTERNAL_SERVER_ERROR;
        return;
      }
      break;
    }
  }
  i++;
  for(; i < ARR_CAPACITY; i++) {
    if(arr[i] != -1) {
      if(shttp_slice_insert_format(&body_slice, SHTTP_SLICE(" {u32}:{i32}"), i,
                                   arr[i])) {
        res.code = SHTTP_CODE_INTERNAL_SERVER_ERROR;
        return;
      }
    }
  }
}

static void create(uint16_t val) {
  if(arr_size == ARR_CAPACITY) {
    res.code = SHTTP_CODE_INSUFFICIENT_STORAGE;
    return;
  }
  for(uint32_t i = 0; i < ARR_CAPACITY; i++) {
    if(arr[i] == -1) {
      if(shttp_slice_insert_u32(&body_slice, i)) {
        res.code = SHTTP_CODE_INTERNAL_SERVER_ERROR;
        return;
      }
      arr[i] = val;
      arr_size++;
      printf("creating %u with value %u\n", i, val);
      return;
    }
  }
  res.code = SHTTP_CODE_INTERNAL_SERVER_ERROR;
}

static void read(uint32_t id) {
  if(arr_size == 0 || arr[id] == -1) {
    res.code = SHTTP_CODE_BAD_REQUEST;
    return;
  }
  if(shttp_slice_insert_i32(&body_slice, arr[id])) {
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
  printf("updating %u with value %u\n", id, val);
}

static void delete(uint32_t id) {
  arr_size -= (arr[id] != -1);
  arr[id] = -1;
  printf("deleting: %u\n", id);
}

static void redirect(void) {
  res.code = SHTTP_CODE_PERMANENT_REDIRECT;
  shttp_slice_cpy(&headers_slice, SHTTP_SLICE("Location: /"));
}

static void path_dispatch(void) {
  uint32_t id = 0;
  int32_t val = 0;
  shttp_slice path_slice = req.path;
  res.body = (shttp_slice){0};
  body_slice = SHTTP_MUT_SLICE(body_buff);
  headers_slice = SHTTP_MUT_SLICE(headers_buff);
  if(req.method != SHTTP_METHOD_GET) {
    res.code = SHTTP_CODE_METHOD_NOT_ALLOWED;
  } else {
    if(shttp_slice_eq(path_slice, SHTTP_SLICE("/"))) {
      main_page();
    } else if(!shttp_slice_parse_format(&path_slice, SHTTP_SLICE("/list"))) {
      list();
    } else if(!shttp_slice_parse_format(
                &path_slice, SHTTP_SLICE("/create?value={u16}"), &val)) {
      create(val);
      redirect();
    } else if(!shttp_slice_parse_format(&path_slice, SHTTP_SLICE("/read/{u32}"),
                                        &id)) {
      read(id);
    } else if(!shttp_slice_parse_format(
                &path_slice, SHTTP_SLICE("/update?id={u32}&value={u16}"), &id,
                &val)) {
      update(id, val);
      redirect();
    } else if(!shttp_slice_parse_format(&path_slice,
                                        SHTTP_SLICE("/delete?id={u32}"), &id)) {
      delete(id);
      redirect();
    } else {
      redirect();
    }
  }
  res.body = (shttp_slice){.begin = body_buff, .end = body_slice.begin};
  res.headers =
    (shttp_slice){.begin = headers_buff, .end = headers_slice.begin};
}

int main(void) {
  SHTTP_PROP(shttp_init(&sock, PORT));
  for(uint32_t i = 0; i < ARR_CAPACITY; i++) arr[i] = -1;

  while(true) {
    if(shttp_next_ignore(&req, &msg_slice, &sock, 10)) continue;
    shttp_response_to_request(&res, &req);
    path_dispatch();
    shttp_send_ignore(&msg_slice, &res);
    shttp_close(&sock, res.id);
  }

  return shttp_deinit(&sock, true);
}
