#include "parse.c"

#include "../test.h"
#include "parse_header.c"
#include "parse_token.c"

TEST(shttp_parse_token_number, normal) {
  char num[] = "1234";
  shttp_u16 n;
  assert_int_equal(sizeof(num) - 1,
                   shttp_parse_token_number(&n, num, sizeof(num)));
  assert_int_equal(1234, n);
}

TEST(shttp_parse_token_cpy, space_end) {
  char str[] = "test ";
  char buff[sizeof(str)];
  assert_int_equal(4, shttp_parse_token_cpy(buff, str, sizeof(str)));
  assert_string_equal(buff, "test");
}

TEST(shttp_parse_token_cpy, null_end) {
  char str[] = "test";
  char buff[sizeof(str)];
  assert_int_equal(4, shttp_parse_token_cpy(buff, str, sizeof(str)));
  assert_string_equal(buff, "test");
}

TEST(shttp_parse_token_cpy, newline_end) {
  char str[] = "test\n";
  char buff[sizeof(str)];
  assert_int_equal(4, shttp_parse_token_cpy(buff, str, sizeof(str)));
  assert_string_equal(buff, "test");
}

TEST(shttp_parse_token_cpy, carriage_end) {
  char str[] = "test\r";
  char buff[sizeof(str)];
  assert_int_equal(4, shttp_parse_token_cpy(buff, str, sizeof(str)));
  assert_string_equal(buff, "test");
}

TEST(shttp_parse_token_cmp, space_end) {
  char str[] = "test ";
  assert_true(shttp_parse_token_cmp("test", str, sizeof(str)));
}

TEST(shttp_parse_token_cmp, null_end) {
  char str[] = "test";
  assert_true(shttp_parse_token_cmp("test", str, sizeof(str)));
}

TEST(shttp_parse_token_cmp, newline_end) {
  char str[] = "test\n";
  assert_true(shttp_parse_token_cmp("test", str, sizeof(str)));
}

TEST(shttp_parse_token_cmp, carriage_end) {
  char str[] = "test\r";
  assert_true(shttp_parse_token_cmp("test", str, sizeof(str)));
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

TEST(shttp_parse_start_line, normal) {
  shttp_request req;
  char msg[] = "GET / HTTP/1.1\r\n";
  assert_int_equal(sizeof(msg) - 1,
                   shttp_parse_start_line(&req, msg, sizeof(msg)));
  assert_int_equal(SHTTP_METHOD_GET, req.method);
  assert_string_equal("/", req.path);
  assert_int_equal(SHTTP_VERSION_1_1, req.version);
}

TEST(shttp_parse_header_host, domain) {
  shttp_request req;
  char msg[] = "Host: google.com\r\n";
  assert_int_equal(sizeof(msg) - 3,
                   shttp_parse_header_host(&req, msg, sizeof(msg)));
  assert_string_equal("google.com", req.host_domain);
  assert_int_equal(80, req.host_port);
}

TEST(shttp_parse_header_host, ip_port) {
  shttp_request req;
  char msg[] = "Host: 127.0.0.1:20\r\n";
  assert_int_equal(sizeof(msg) - 3,
                   shttp_parse_header_host(&req, msg, sizeof(msg)));
  assert_string_equal("127.0.0.1", req.host_domain);
  assert_int_equal(20, req.host_port);
}

TEST(shttp_parse_request_header, normal) {
  shttp_request req;
  char msg[] = "Host: google.com\r\n\r\n";
  assert_int_equal(sizeof(msg) - 1, shttp_parse_header(&req, msg, sizeof(msg)));
  assert_true(req.has_host);
  assert_string_equal("google.com", req.host_domain);
  assert_int_equal(80, req.host_port);
}

TEST(shttp_parse_request, start_line) {
  shttp_request req;
  char msg[] = "GET / HTTP/1.1\r\n";
  assert_int_equal(sizeof(msg) - 1,
                   shttp_parse_request(&req, msg, sizeof(msg)));
  assert_int_equal(SHTTP_METHOD_GET, req.method);
  assert_string_equal("/", req.path);
  assert_int_equal(SHTTP_VERSION_1_1, req.version);
  assert_false(req.has_host);
}

TEST(shttp_parse_request, header) {
  shttp_request req;
  char msg[] = "GET / HTTP/1.1\r\nHost: google.com\r\n\r\n";
  assert_int_equal(sizeof(msg) - 1,
                   shttp_parse_request(&req, msg, sizeof(msg)));
  assert_int_equal(SHTTP_METHOD_GET, req.method);
  assert_string_equal("/", req.path);
  assert_int_equal(SHTTP_VERSION_1_1, req.version);
  assert_true(req.has_host);
  assert_string_equal("google.com", req.host_domain);
  assert_int_equal(80, req.host_port);
}

int main(void) {
  const struct CMUnitTest tests[] = {
    ADD(shttp_parse_token_number, normal),
    ADD(shttp_parse_token_cpy, space_end),
    ADD(shttp_parse_token_cpy, null_end),
    ADD(shttp_parse_token_cpy, newline_end),
    ADD(shttp_parse_token_cpy, carriage_end),
    ADD(shttp_parse_token_cmp, space_end),
    ADD(shttp_parse_token_cmp, null_end),
    ADD(shttp_parse_token_cmp, newline_end),
    ADD(shttp_parse_token_cmp, carriage_end),
    ADD(shttp_parse_method, get),
    ADD(shttp_parse_method, post),
    ADD(shttp_parse_path, slash),
    ADD(shttp_parse_version, 1_1),
    ADD(shttp_parse_version, 3_0),
    ADD(shttp_parse_start_line, normal),
    ADD(shttp_parse_header_host, domain),
    ADD(shttp_parse_header_host, ip_port),
    ADD(shttp_parse_request_header, normal),
    ADD(shttp_parse_request, start_line),
    ADD(shttp_parse_request, header),
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}
