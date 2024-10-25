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

static shttp_s32 arr[ARR_CAPACITY];
static shttp_u32 arr_size = 0;

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
  char num_buff[10];
  shttp_u32 i;
  shttp_u8 printed;
  shttp_slice slice = SHTTP_SLICE(num_buff);
  if(arr_size == 0) return;
  for(i = 0; i < ARR_CAPACITY; i++) {
    if(arr[i] != -1) {
      printed = snprintf(num_buff, 10, "%u:%u", i, arr[i]);
      slice.end = slice.begin + printed;
      shttp_slice_cpy(&body_slice, slice);
      break;
    }
  }
  i++;
  for(; i < ARR_CAPACITY; i++) {
    if(arr[i] != -1) {
      printed = snprintf(num_buff, 10, " %u:%u", i, arr[i]);
      slice.end = slice.begin + printed;
      shttp_slice_cpy(&body_slice, slice);
    }
  }
}

static void create(shttp_u32 val) {
  char num_buff[10];
  shttp_u8 printed;
  shttp_slice slice;
  if(arr_size == ARR_CAPACITY) {
    res.code = SHTTP_CODE_INSUFFICIENT_STORAGE;
    return;
  }
  for(shttp_u32 i = 0; i < ARR_CAPACITY; i++) {
    if(arr[i] == -1) {
      printed = snprintf(num_buff, 10, "%u", i);
      if(printed < 1) {
        res.code = SHTTP_CODE_INTERNAL_SERVER_ERROR;
        return;
      }
      slice.begin = num_buff;
      slice.end = num_buff + printed;
      shttp_slice_cpy(&body_slice, slice);
      arr[i] = val;
      arr_size++;
      return;
    }
  }
  res.code = SHTTP_CODE_INTERNAL_SERVER_ERROR;
  return;
}

static void read(shttp_u32 id) {
  char num_buff[10];
  shttp_u8 printed;
  shttp_slice slice = SHTTP_SLICE(num_buff);
  if(arr_size == 0 || arr[id] == -1) {
    res.code = SHTTP_CODE_BAD_REQUEST;
    return;
  }
  printed = snprintf(num_buff, 10, "%u", arr[id]);
  if(printed < 1) {
    res.code = SHTTP_CODE_INTERNAL_SERVER_ERROR;
    return;
  }
  slice.end = slice.begin + printed;
  shttp_slice_cpy(&body_slice, slice);
}

static void update(shttp_u32 id, shttp_u32 val) {
  if(arr[id] == -1) {
    res.code = SHTTP_CODE_BAD_REQUEST;
    return;
  }
  arr[id] = val;
}

static void delete(shttp_u32 id) {
  arr_size -= (arr[id] != -1);
  arr[id] = -1;
}

static void path_dispatch(void) {
  shttp_u32 id;
  shttp_u32 val;
  shttp_slice path_slice;
  body_slice = SHTTP_MUT_SLICE(body_buff);
  res.body = (shttp_slice){0};
  if(shttp_slice_eq(req.path, SHTTP_SLICE("/")) ||
     shttp_slice_starts_with(req.path, SHTTP_SLICE("/options"))) {
    options();
  } else if(shttp_slice_starts_with(req.path, SHTTP_SLICE("/list"))) {
    list();
  } else if(shttp_slice_starts_with(req.path, SHTTP_SLICE("/create/"))) {
    path_slice = req.path;
    path_slice.begin += sizeof("/create/") - 1;
    if(shttp_slice_parse_number(&val, &path_slice)) {
      res.code = SHTTP_CODE_BAD_REQUEST;
      return;
    }
    create(val);
  } else if(shttp_slice_starts_with(req.path, SHTTP_SLICE("/read/"))) {
    path_slice = req.path;
    path_slice.begin += sizeof("/read/") - 1;
    if(shttp_slice_parse_number(&id, &path_slice)) {
      res.code = SHTTP_CODE_BAD_REQUEST;
      return;
    }
    read(id);
  } else if(shttp_slice_starts_with(req.path, SHTTP_SLICE("/update/"))) {
    path_slice = req.path;
    path_slice.begin += sizeof("/update/") - 1;
    if(shttp_slice_parse_number(&id, &path_slice) ||
       path_slice.begin[0] != ':') {
      res.code = SHTTP_CODE_BAD_REQUEST;
      return;
    }
    path_slice.begin++;
    if(shttp_slice_parse_number(&val, &path_slice)) {
      res.code = SHTTP_CODE_BAD_REQUEST;
      return;
    }
    update(id, val);
  } else if(shttp_slice_starts_with(req.path, SHTTP_SLICE("/delete/"))) {
    path_slice = req.path;
    path_slice.begin += sizeof("/delete/") - 1;
    if(shttp_slice_parse_number(&id, &path_slice)) {
      res.code = SHTTP_CODE_BAD_REQUEST;
      return;
    }
    delete(id);
  }
}

static void set_response(void) {
  res.body = (shttp_slice){.begin = body_buff, .end = body_slice.begin};
}

int main(void) {
  SHTTP_PROP(shttp_init(&sock, PORT));
  for(shttp_u32 i = 0; i < ARR_CAPACITY; i++) arr[i] = -1;

  while(true) {
    if(shttp_next_ignore(&req, &msg_slice, &sock, 10)) continue;
    shttp_response_to_request(&res, &req);
    path_dispatch();
    set_response();
    shttp_send_ignore(&msg_slice, &res);
    shttp_close(&sock, res.id);
  }

  return shttp_deinit(&sock, true);
}
