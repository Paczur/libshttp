#include <stdio.h>

#define SHTTP_UNUSED_RESULT  // disables required error handling
#include "../shttp.h"

#define MSG_BUFF_LENGTH 500
#define BODY_BUFF_LENGTH 300
#define ARR_CAPACITY 10
#define MAX_CONNS 10
#define PORT 1338

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
  shttp_slice_cpy(&body_slice, SHTTP_SLICE("/options"));
  shttp_slice_insert_newline(&body_slice);
  shttp_slice_cpy(&body_slice, SHTTP_SLICE("/list"));
  shttp_slice_insert_newline(&body_slice);
  shttp_slice_cpy(&body_slice, SHTTP_SLICE("/create/[num]"));
  shttp_slice_insert_newline(&body_slice);
  shttp_slice_cpy(&body_slice, SHTTP_SLICE("/read/[id]"));
  shttp_slice_insert_newline(&body_slice);
  shttp_slice_cpy(&body_slice, SHTTP_SLICE("/update/[id]:[num]"));
  shttp_slice_insert_newline(&body_slice);
  shttp_slice_cpy(&body_slice, SHTTP_SLICE("/delete/[id]"));
}

static void list(void) {
  uint32_t i;
  if(arr_size == 0) return;
  for(i = 0; i < ARR_CAPACITY; i++) {
    if(arr[i] != -1) {
      if(shttp_slice_insert_format(&body_slice, SHTTP_SLICE("{u32}:{i32}"), i,
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

static void create(uint32_t val) {
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
  if(shttp_slice_insert_i32(&body_slice, arr[id])) {
    res.code = SHTTP_CODE_INTERNAL_SERVER_ERROR;
    return;
  }
}

static void update(uint32_t id, uint32_t val) {
  if(arr[id] == -1) {
    res.code = SHTTP_CODE_NOT_FOUND;
    return;
  }
  arr[id] = val;
}

static void delete(uint32_t id) {
  arr_size -= (arr[id] != -1);
  arr[id] = -1;
}

static void path_dispatch(void) {
  uint32_t id = 0;
  int32_t val = 0;
  shttp_slice path_slice = req.path;
  res.body = (shttp_slice){0};
  body_slice = SHTTP_MUT_SLICE(body_buff);
  if(shttp_slice_eq(path_slice, SHTTP_SLICE("/"))) {
    options();
  } else if(!shttp_slice_parse_format(&path_slice, SHTTP_SLICE("/options"))) {
    options();
  } else if(!shttp_slice_parse_format(&path_slice, SHTTP_SLICE("/list"))) {
    list();
  } else if(!shttp_slice_parse_format(&path_slice, SHTTP_SLICE("/create/{i32}"),
                                      &val)) {
    create(val);
  } else if(!shttp_slice_parse_format(&path_slice, SHTTP_SLICE("/read/{u32}"),
                                      &id)) {
    read(id);
  } else if(!shttp_slice_parse_format(
              &path_slice, SHTTP_SLICE("/update/{u32}:{i32}"), &id, &val)) {
    update(id, val);
  } else if(!shttp_slice_parse_format(&path_slice, SHTTP_SLICE("/delete/{u32}"),
                                      &id)) {
    delete(id);
  } else {
    res.code = SHTTP_CODE_BAD_REQUEST;
    return;
  }
  res.body = (shttp_slice){.begin = body_buff, .end = body_slice.begin};
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
