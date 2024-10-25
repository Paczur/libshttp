#include "msg.c"

#include "../slice/slice.c"
#include "../test.h"

TEST(method, GET) {
  shttp_request req;
  char msg[] = "GET";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK, method(&req, &smsg));
  assert_int_equal(SHTTP_METHOD_GET, req.method);
  assert_int_equal(msg + sizeof(msg) - 1, smsg.begin);
}

TEST(method, POST) {
  shttp_request req;
  char msg[] = "POST";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK, method(&req, &smsg));
  assert_int_equal(SHTTP_METHOD_POST, req.method);
  assert_int_equal(msg + sizeof(msg) - 1, smsg.begin);
}

TEST(path, SLASH) {
  shttp_request req;
  char msg[] = "/ ";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK, path(&req, &smsg));
  assert_ptr_equal(msg + sizeof("/") - 1, smsg.begin);
  assert_ptr_equal(msg, req.path.begin);
  assert_ptr_equal(msg + sizeof("/") - 1, req.path.end);
}

TEST(parse_version, 1_1) {
  shttp_request req;
  char msg[] = "HTTP/1.1";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK, parse_version(&req, &smsg));
  assert_int_equal(SHTTP_VERSION_1_1, req.version);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
}

TEST(parse_version, 3_0) {
  shttp_request req;
  char msg[] = "HTTP/3.0";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK, parse_version(&req, &smsg));
  assert_int_equal(SHTTP_VERSION_3_0, req.version);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
}

TEST(parse_version, 1_2) {
  shttp_request req;
  char msg[] = "HTTP/1.2";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_VALUE_INVALID, parse_version(&req, &smsg));
  assert_ptr_equal(msg + sizeof("HTTP/") - 1, smsg.begin);
}

TEST(request_line, BASIC) {
  char msg[] = "GET / HTTP/1.1\r\n";
  shttp_request req;
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK, request_line(&req, &smsg));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_int_equal(SHTTP_METHOD_GET, req.method);
  assert_ptr_equal(msg + sizeof("GET ") - 1, req.path.begin);
  assert_ptr_equal(msg + sizeof("GET /") - 1, req.path.end);
  assert_int_equal(SHTTP_VERSION_1_1, req.version);
}

TEST(slices, HEADER) {
  const char msg[] = "Header: value value2\r\n\r\n";
  shttp_slice smsg = SHTTP_SLICE(msg);
  shttp_request req;
  assert_int_equal(SHTTP_STATUS_OK, slices(&req, &smsg));
  assert_ptr_equal(msg, req.headers.begin);
  assert_ptr_equal(msg + sizeof(msg) - 3, req.headers.end);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(slices, HEADER_BODY) {
  const char msg[] = "Header: value value2\r\n\r\nBody message\r\n";
  shttp_slice smsg = SHTTP_SLICE(msg);
  shttp_request req;
  assert_int_equal(SHTTP_STATUS_OK, slices(&req, &smsg));
  assert_ptr_equal(msg, req.headers.begin);
  assert_ptr_equal(msg + sizeof("Header: value value2\r\n") - 1,
                   req.headers.end);
  assert_ptr_equal(msg + sizeof("Header: value value2\r\n\r\n") - 1,
                   smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 3, smsg.end);
}

TEST(insert_version, 1_0) {
  const char ans[] = "HTTP/1.0";
  shttp_response res = {.version = SHTTP_VERSION_1_0};
  char msg[sizeof(ans)];
  shttp_mut_slice smsg = SHTTP_MUT_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK, insert_version(&smsg, &res));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_string_equal(ans, msg);
}

TEST(code, OK) {
  const char ans[] = "200 OK";
  char msg[sizeof(ans)];
  shttp_mut_slice smsg = SHTTP_MUT_SLICE(msg);
  shttp_response res = {.code = 200};
  assert_int_equal(SHTTP_STATUS_OK, code(&smsg, &res));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_string_equal(ans, msg);
}

TEST(status_line, 1_1_OK) {
  const char ans[] = "HTTP/1.1 200 OK\r\n";
  char msg[sizeof(ans)];
  shttp_mut_slice smsg = {msg, msg + sizeof(msg)};
  shttp_response res = {.version = SHTTP_VERSION_1_1, .code = 200};
  assert_int_equal(SHTTP_STATUS_OK, status_line(&smsg, &res));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg), smsg.end);
  assert_string_equal(ans, msg);
}

TEST(shttp_msg_response, START_ONLY) {
  const char ans[] = "HTTP/1.0 200 OK\r\n";
  char msg[sizeof(ans)];
  shttp_mut_slice smsg = {msg, msg + sizeof(msg)};
  shttp_response res = {.version = SHTTP_VERSION_1_0, .code = 200};
  assert_int_equal(SHTTP_STATUS_OK, shttp_msg_response(&smsg, &res));
  assert_ptr_equal(msg, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
  assert_string_equal(ans, msg);
}

TEST(shttp_msg_response, HEADERS) {
  const char ans[] = "HTTP/1.0 200 OK\r\nConnection: keep-alive\r\n\r\n";
  const char headers[] = "Connection: keep-alive\r\n";
  char msg[sizeof(ans)];
  shttp_mut_slice smsg = {msg, msg + sizeof(msg)};
  shttp_response res = {
    .version = SHTTP_VERSION_1_0, .code = 200, .headers = SHTTP_SLICE(headers)};
  assert_int_equal(SHTTP_STATUS_OK, shttp_msg_response(&smsg, &res));
  assert_ptr_equal(msg, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
  assert_string_equal(ans, msg);
}

int main(void) {
  const struct CMUnitTest tests[] = {
    ADD(method, GET),
    ADD(method, POST),
    ADD(path, SLASH),
    ADD(parse_version, 1_1),
    ADD(parse_version, 3_0),
    ADD(parse_version, 1_2),
    ADD(request_line, BASIC),
    ADD(slices, HEADER),
    ADD(slices, HEADER_BODY),
    ADD(insert_version, 1_0),
    ADD(code, OK),
    ADD(status_line, 1_1_OK),
    ADD(shttp_msg_response, START_ONLY),
    ADD(shttp_msg_response, HEADERS),
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}
