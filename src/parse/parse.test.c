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

TEST(shttp_parse_token_cpy, null_end) {
  char str[] = "test";
  char buff[sizeof(str)];
  assert_int_equal(4, shttp_parse_token_cpy(buff, str, sizeof(str) - 1));
  assert_string_equal(buff, "test");
}

TEST(shttp_parse_token_cmp, null_end) {
  char str[] = "test";
  assert_true(shttp_parse_token_cmp("test", str, sizeof(str) - 1));
}

TEST(shttp_parse_token_values, single) {
  char msg[] = "test\r\n";
  shttp_value val;
  assert_int_equal(sizeof(msg) - 1,
                   shttp_parse_token_values(&val, 1, msg, sizeof(msg)));
  assert_string_equal("test", val.value);
}

TEST(shttp_parse_token_values, multiple) {
  char msg[] = "test, test2\r\n";
  shttp_value vals[2];
  assert_int_equal(sizeof(msg) - 1, shttp_parse_token_values(vals, LENGTH(vals),
                                                             msg, sizeof(msg)));
  assert_string_equal("test", vals[0].value);
  assert_string_equal("test2", vals[1].value);
}

TEST(shttp_parse_token_values_weighted, single) {
  char msg[] = "test\r\n";
  shttp_value_weighted val;
  assert_int_equal(sizeof(msg) - 1, shttp_parse_token_values_weighted(
                                      &val, 1, msg, sizeof(msg)));
  assert_string_equal("test", val.value);
  assert_int_equal(SHTTP_WEIGHT_1_0, val.weight);
}

TEST(shttp_parse_token_values_weighted, multiple) {
  char msg[] = "test, test2\r\n";
  shttp_value_weighted vals[2];
  assert_int_equal(sizeof(msg) - 1, shttp_parse_token_values_weighted(
                                      vals, LENGTH(vals), msg, sizeof(msg)));
  assert_string_equal("test", vals[0].value);
  assert_string_equal("test2", vals[1].value);
  assert_int_equal(SHTTP_WEIGHT_1_0, vals[0].weight);
  assert_int_equal(SHTTP_WEIGHT_1_0, vals[1].weight);
}

TEST(shttp_parse_token_values_weighted, single_weighted) {
  char msg[] = "test;q=0.2\r\n";
  shttp_value_weighted val;
  assert_int_equal(sizeof(msg) - 1, shttp_parse_token_values_weighted(
                                      &val, 1, msg, sizeof(msg)));
  assert_string_equal("test", val.value);
  assert_int_equal(SHTTP_WEIGHT_000 + 200, val.weight);
}

TEST(shttp_parse_token_values_weighted, multiple_weighted) {
  char msg[] = "test;q=0.5, test2;q=0.01\r\n";
  shttp_value_weighted vals[2];
  assert_int_equal(sizeof(msg) - 1, shttp_parse_token_values_weighted(
                                      vals, LENGTH(vals), msg, sizeof(msg)));
  assert_string_equal("test", vals[0].value);
  assert_string_equal("test2", vals[1].value);
  assert_int_equal(SHTTP_WEIGHT_000 + 500, vals[0].weight);
  assert_int_equal(SHTTP_WEIGHT_000 + 10, vals[1].weight);
}

TEST(shttp_parse_token_values, single_LF) {
  char msg[] = "test\n";
  shttp_value val;
  assert_int_equal(sizeof(msg) - 1,
                   shttp_parse_token_values(&val, 1, msg, sizeof(msg)));
  assert_string_equal("test", val.value);
}

TEST(shttp_parse_token_values, multiple_LF) {
  char msg[] = "test, test2\n";
  shttp_value vals[2];
  assert_int_equal(sizeof(msg) - 1, shttp_parse_token_values(vals, LENGTH(vals),
                                                             msg, sizeof(msg)));
  assert_string_equal("test", vals[0].value);
  assert_string_equal("test2", vals[1].value);
}

TEST(shttp_parse_token_values_weighted, single_LF) {
  char msg[] = "test\n";
  shttp_value_weighted val;
  assert_int_equal(sizeof(msg) - 1, shttp_parse_token_values_weighted(
                                      &val, 1, msg, sizeof(msg)));
  assert_string_equal("test", val.value);
  assert_int_equal(SHTTP_WEIGHT_1_0, val.weight);
}

TEST(shttp_parse_token_values_weighted, multiple_LF) {
  char msg[] = "test, test2\n";
  shttp_value_weighted vals[2];
  assert_int_equal(sizeof(msg) - 1, shttp_parse_token_values_weighted(
                                      vals, LENGTH(vals), msg, sizeof(msg)));
  assert_string_equal("test", vals[0].value);
  assert_string_equal("test2", vals[1].value);
  assert_int_equal(SHTTP_WEIGHT_1_0, vals[0].weight);
  assert_int_equal(SHTTP_WEIGHT_1_0, vals[1].weight);
}

TEST(shttp_parse_token_values_weighted, single_weighted_LF) {
  char msg[] = "test;q=0.2\n";
  shttp_value_weighted val;
  assert_int_equal(sizeof(msg) - 1, shttp_parse_token_values_weighted(
                                      &val, 1, msg, sizeof(msg)));
  assert_string_equal("test", val.value);
  assert_int_equal(SHTTP_WEIGHT_000 + 200, val.weight);
}

TEST(shttp_parse_token_values_weighted, multiple_weighted_LF) {
  char msg[] = "test;q=0.5, test2;q=0.01\n";
  shttp_value_weighted vals[2];
  assert_int_equal(sizeof(msg) - 1, shttp_parse_token_values_weighted(
                                      vals, LENGTH(vals), msg, sizeof(msg)));
  assert_string_equal("test", vals[0].value);
  assert_string_equal("test2", vals[1].value);
  assert_int_equal(SHTTP_WEIGHT_000 + 500, vals[0].weight);
  assert_int_equal(SHTTP_WEIGHT_000 + 10, vals[1].weight);
}

TEST(shttp_parse_token_datetime, short_epoch_start) {
  shttp_datetime datetime;
  char msg[] = "01 Jan 1970 01:02 UT";
  assert_int_equal(sizeof(msg) - 1,
                   shttp_parse_token_datetime(&datetime, msg, sizeof(msg)));
  assert_int_equal(1, datetime.day);
  assert_int_equal(SHTTP_MONTH_JAN, datetime.month);
  assert_int_equal(1970, datetime.year);
  assert_int_equal(1, datetime.hours);
  assert_int_equal(2, datetime.minutes);
  assert_int_equal(SHTTP_TIMEZONE_UT, datetime.timezone);
}

TEST(shttp_parse_method, get) {
  shttp_request req;
  char msg[] = "GET";
  assert_int_equal(sizeof(msg) - 1,
                   shttp_parse_method(&req, msg, sizeof(msg) - 1));
  assert_int_equal(SHTTP_METHOD_GET, req.method);
}

TEST(shttp_parse_method, post) {
  shttp_request req;
  char msg[] = "POST";
  assert_int_equal(sizeof(msg) - 1,
                   shttp_parse_method(&req, msg, sizeof(msg) - 1));
  assert_int_equal(SHTTP_METHOD_POST, req.method);
}

TEST(shttp_parse_path, slash) {
  shttp_request req;
  char msg[] = "/";
  assert_int_equal(sizeof(msg) - 1,
                   shttp_parse_path(&req, msg, sizeof(msg) - 1));
  assert_string_equal("/", req.path);
}

TEST(shttp_parse_version, 1_1) {
  shttp_request req;
  char msg[] = "HTTP/1.1";
  assert_int_equal(sizeof(msg) - 1,
                   shttp_parse_version(&req, msg, sizeof(msg) - 1));
  assert_int_equal(SHTTP_VERSION_1_1, req.version);
}

TEST(shttp_parse_version, 3_0) {
  shttp_request req;
  char msg[] = "HTTP/3.0";
  assert_int_equal(sizeof(msg) - 1,
                   shttp_parse_version(&req, msg, sizeof(msg) - 1));
  assert_int_equal(SHTTP_VERSION_3_0, req.version);
}

TEST(shttp_parse_start_line, normal) {
  shttp_request req;
  char msg[] = "GET / HTTP/1.1\r\n";
  assert_int_equal(sizeof(msg) - 1,
                   shttp_parse_start_line(&req, msg, sizeof(msg) - 1));
  assert_int_equal(SHTTP_METHOD_GET, req.method);
  assert_string_equal("/", req.path);
  assert_int_equal(SHTTP_VERSION_1_1, req.version);
}

TEST(shttp_parse_start_line, normal_LF) {
  shttp_request req;
  char msg[] = "GET / HTTP/1.1\n";
  assert_int_equal(sizeof(msg) - 1,
                   shttp_parse_start_line(&req, msg, sizeof(msg) - 1));
  assert_int_equal(SHTTP_METHOD_GET, req.method);
  assert_string_equal("/", req.path);
  assert_int_equal(SHTTP_VERSION_1_1, req.version);
}

TEST(shttp_parse_header_host, domain) {
  shttp_request req;
  char msg[] = "Host: google.com\r\n";
  assert_int_equal(sizeof(msg) - 1,
                   shttp_parse_header_host(&req, msg, sizeof(msg) - 1));
  assert_string_equal("google.com", req.host_domain);
  assert_int_equal(80, req.host_port);
}

TEST(shttp_parse_header_host, domain_LF) {
  shttp_request req;
  char msg[] = "Host: google.com\n";
  assert_int_equal(sizeof(msg) - 1,
                   shttp_parse_header_host(&req, msg, sizeof(msg) - 1));
  assert_string_equal("google.com", req.host_domain);
  assert_int_equal(80, req.host_port);
}

TEST(shttp_parse_header_host, ip_port) {
  shttp_request req;
  char msg[] = "Host: 127.0.0.1:20\r\n";
  assert_int_equal(sizeof(msg) - 1,
                   shttp_parse_header_host(&req, msg, sizeof(msg) - 1));
  assert_string_equal("127.0.0.1", req.host_domain);
  assert_int_equal(20, req.host_port);
}

TEST(shttp_parse_header_host, ip_port_LF) {
  shttp_request req;
  char msg[] = "Host: 127.0.0.1:20\n";
  assert_int_equal(sizeof(msg) - 1,
                   shttp_parse_header_host(&req, msg, sizeof(msg) - 1));
  assert_string_equal("127.0.0.1", req.host_domain);
  assert_int_equal(20, req.host_port);
}

TEST(shttp_parse_request, start_line) {
  shttp_request req;
  char msg[] = "GET / HTTP/1.1\r\n";
  assert_int_equal(sizeof(msg) - 1,
                   shttp_parse_request(&req, msg, sizeof(msg) - 1));
  assert_int_equal(SHTTP_METHOD_GET, req.method);
  assert_string_equal("/", req.path);
  assert_int_equal(SHTTP_VERSION_1_1, req.version);
}

TEST(shttp_parse_request, start_line_LF) {
  shttp_request req;
  char msg[] = "GET / HTTP/1.1\n";
  assert_int_equal(sizeof(msg) - 1,
                   shttp_parse_request(&req, msg, sizeof(msg) - 1));
  assert_int_equal(SHTTP_METHOD_GET, req.method);
  assert_string_equal("/", req.path);
  assert_int_equal(SHTTP_VERSION_1_1, req.version);
}

TEST(shttp_parse_request_header, normal) {
  shttp_request req;
  char msg[] = "Host: google.com\r\n\r\n";
  assert_int_equal(sizeof(msg) - 1,
                   shttp_parse_header(&req, msg, sizeof(msg) - 1));
  assert_string_equal("google.com", req.host_domain);
  assert_int_equal(80, req.host_port);
}

TEST(shttp_parse_request_header, normal_LF) {
  shttp_request req;
  char msg[] = "Host: google.com\n\n";
  assert_int_equal(sizeof(msg) - 1,
                   shttp_parse_header(&req, msg, sizeof(msg) - 1));
  assert_string_equal("google.com", req.host_domain);
  assert_int_equal(80, req.host_port);
}

TEST(shttp_parse_request, httpie) {
  shttp_request req;
  char msg[] =
    "GET / HTTP/1.1\r\n"
    "Host: localhost:69\r\n"
    "Accept-Encoding: gzip, deflate, zstd\r\n"
    "Accept: */*\r\n"
    "Connection: keep-alive\r\n"
    "User-Agent: HTTPie/3.2.2\r\n"
    "\r\n";
  assert_int_equal(sizeof(msg) - 1,
                   shttp_parse_request(&req, msg, sizeof(msg) - 1));

  assert_int_equal(SHTTP_METHOD_GET, req.method);
  assert_string_equal("/", req.path);
  assert_int_equal(SHTTP_VERSION_1_1, req.version);

  assert_string_equal("localhost", req.host_domain);
  assert_int_equal(69, req.host_port);

  assert_string_equal("gzip", req.accept_encoding[0].value);
  assert_int_equal(SHTTP_WEIGHT_1_0, req.accept_encoding[0].weight);
  assert_string_equal("deflate", req.accept_encoding[1].value);
  assert_int_equal(SHTTP_WEIGHT_1_0, req.accept_encoding[1].weight);
  assert_string_equal("zstd", req.accept_encoding[2].value);
  assert_int_equal(SHTTP_WEIGHT_1_0, req.accept_encoding[2].weight);

  assert_string_equal("*/*", req.accept[0].value);
  assert_int_equal(SHTTP_WEIGHT_1_0, req.accept[0].weight);

  assert_string_equal("keep-alive", req.connection[0].value);
  assert_string_equal("HTTPie/3.2.2", req.user_agent.value);
}

int main(void) {
  const struct CMUnitTest tests[] = {
    ADD(shttp_parse_token_number, normal),
    ADD(shttp_parse_token_cpy, null_end),
    ADD(shttp_parse_token_cmp, null_end),
    ADD(shttp_parse_token_values, single),
    ADD(shttp_parse_token_values, single_LF),
    ADD(shttp_parse_token_values, multiple),
    ADD(shttp_parse_token_values, multiple_LF),
    ADD(shttp_parse_token_values_weighted, single),
    ADD(shttp_parse_token_values_weighted, single_LF),
    ADD(shttp_parse_token_values_weighted, multiple),
    ADD(shttp_parse_token_values_weighted, multiple_LF),
    ADD(shttp_parse_token_values_weighted, single_weighted),
    ADD(shttp_parse_token_values_weighted, single_weighted_LF),
    ADD(shttp_parse_token_values_weighted, multiple_weighted),
    ADD(shttp_parse_token_values_weighted, multiple_weighted_LF),
    ADD(shttp_parse_token_datetime, short_epoch_start),
    ADD(shttp_parse_method, get),
    ADD(shttp_parse_method, post),
    ADD(shttp_parse_path, slash),
    ADD(shttp_parse_version, 1_1),
    ADD(shttp_parse_version, 3_0),
    ADD(shttp_parse_start_line, normal),
    ADD(shttp_parse_start_line, normal_LF),
    ADD(shttp_parse_header_host, domain),
    ADD(shttp_parse_header_host, domain_LF),
    ADD(shttp_parse_header_host, ip_port),
    ADD(shttp_parse_header_host, ip_port_LF),
    ADD(shttp_parse_request_header, normal),
    ADD(shttp_parse_request_header, normal_LF),
    ADD(shttp_parse_request, start_line),
    ADD(shttp_parse_request, start_line_LF),
    ADD(shttp_parse_request, httpie),
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}
