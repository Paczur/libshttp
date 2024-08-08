#include "parse.c"

#include "test.h"

TEST(shttp_token_cpy, space_end) {
  char str[] = "test ";
  char buff[sizeof(str)];
  assert_int_equal(4, shttp_token_cpy(buff, str, sizeof(str)));
  assert_string_equal(buff, "test");
}

TEST(shttp_token_cpy, null_end) {
  char str[] = "test";
  char buff[sizeof(str)];
  assert_int_equal(4, shttp_token_cpy(buff, str, sizeof(str)));
  assert_string_equal(buff, "test");
}

TEST(shttp_token_cpy, newline_end) {
  char str[] = "test\n";
  char buff[sizeof(str)];
  assert_int_equal(4, shttp_token_cpy(buff, str, sizeof(str)));
  assert_string_equal(buff, "test");
}

TEST(shttp_token_cpy, carriage_end) {
  char str[] = "test\r";
  char buff[sizeof(str)];
  assert_int_equal(4, shttp_token_cpy(buff, str, sizeof(str)));
  assert_string_equal(buff, "test");
}

TEST(shttp_token_cmp, space_end) {
  char str[] = "test ";
  assert_true(shttp_token_cmp("test", str, sizeof(str)));
}

TEST(shttp_token_cmp, null_end) {
  char str[] = "test";
  assert_true(shttp_token_cmp("test", str, sizeof(str)));
}

TEST(shttp_token_cmp, newline_end) {
  char str[] = "test\n";
  assert_true(shttp_token_cmp("test", str, sizeof(str)));
}

TEST(shttp_token_cmp, carriage_end) {
  char str[] = "test\r";
  assert_true(shttp_token_cmp("test", str, sizeof(str)));
}

TEST(shttp_parse_method, get) {
  shttp_request req;
  char msg[] = "GET";
  assert_int_equal(sizeof(msg) - 1, shttp_parse_method(&req, msg, sizeof(msg)));
  assert_int_equal(SHTTP_METHOD_GET, req.method);
}

TEST(shttp_parse_method, post) {
  shttp_request req;
  char msg[] = "POST";
  assert_int_equal(sizeof(msg) - 1, shttp_parse_method(&req, msg, sizeof(msg)));
  assert_int_equal(SHTTP_METHOD_POST, req.method);
}

TEST(shttp_parse_path, slash) {
  shttp_request req;
  char msg[] = "/";
  assert_int_equal(sizeof(msg) - 1, shttp_parse_path(&req, msg, sizeof(msg)));
  assert_string_equal("/", req.path);
}

TEST(shttp_parse_version, 1_1) {
  shttp_request req;
  char msg[] = "HTTP/1.1";
  assert_int_equal(sizeof(msg) - 1,
                   shttp_parse_version(&req, msg, sizeof(msg)));
  assert_int_equal(SHTTP_VERSION_1_1, req.version);
}

TEST(shttp_parse_version, 3_0) {
  shttp_request req;
  char msg[] = "HTTP/3.0";
  assert_int_equal(sizeof(msg) - 1,
                   shttp_parse_version(&req, msg, sizeof(msg)));
  assert_int_equal(SHTTP_VERSION_3_0, req.version);
}

TEST(shttp_parse_request, necessary_only) {
  shttp_request req;
  char msg[] = "GET / HTTP/1.1\r\n";
  shttp_parse_request(&req, msg, sizeof(msg));
  assert_int_equal(SHTTP_METHOD_GET, req.method);
  assert_string_equal("/", req.path);
  assert_int_equal(SHTTP_VERSION_1_1, req.version);
}

int main(void) {
  const struct CMUnitTest tests[] = {
    ADD(shttp_token_cpy, space_end),   ADD(shttp_token_cpy, null_end),
    ADD(shttp_token_cpy, newline_end), ADD(shttp_token_cpy, carriage_end),
    ADD(shttp_token_cmp, space_end),   ADD(shttp_token_cmp, null_end),
    ADD(shttp_token_cmp, newline_end), ADD(shttp_token_cmp, carriage_end),
    ADD(shttp_parse_method, get),      ADD(shttp_parse_method, post),
    ADD(shttp_parse_path, slash),      ADD(shttp_parse_version, 1_1),
    ADD(shttp_parse_version, 3_0),     ADD(shttp_parse_request, necessary_only),
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}
