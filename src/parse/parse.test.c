#include "parse.c"

#include "../test.h"

TEST(shttp_parse_slice_newline, CRLF) {
  const char msg[] = "\r\n";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK, shttp_parse_slice_newline(&smsg));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_slice_newline, LF) {
  const char msg[] = "\n";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK, shttp_parse_slice_newline(&smsg));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_slice_newline, CR_SPACE) {
  const char msg[] = "\r ";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_NEWLINE_EXPECTED,
                   shttp_parse_slice_newline(&smsg));
  assert_ptr_equal(msg + sizeof(msg) - 2, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_slice_newline, CR_END) {
  const char msg[] = "\r";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_SLICE_END, shttp_parse_slice_newline(&smsg));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_slice_newline, END) {
  const char msg[] = "";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_SLICE_END, shttp_parse_slice_newline(&smsg));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_slice_space, SPACE) {
  const char msg[] = " ";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK, shttp_parse_slice_space(&smsg));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_slice_space, NOT_SPACE) {
  const char msg[] = "s";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_SPACE_EXPECTED, shttp_parse_slice_space(&smsg));
  assert_ptr_equal(msg + sizeof(msg) - 2, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_slice_space, END) {
  const char msg[] = "";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_SLICE_END, shttp_parse_slice_space(&smsg));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_slice_space_optional, SPACE) {
  const char msg[] = " ";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK, shttp_parse_slice_space_optional(&smsg));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_slice_space_optional, NOT_SPACE) {
  const char msg[] = "s";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK, shttp_parse_slice_space_optional(&smsg));
  assert_ptr_equal(msg + sizeof(msg) - 2, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_slice_space_optional, END) {
  const char msg[] = "";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_SLICE_END,
                   shttp_parse_slice_space_optional(&smsg));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_slice_number, 90) {
  const char msg[] = "90";
  shttp_u16 n;
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK, shttp_parse_slice_number(&n, &smsg));
  assert_int_equal(90, n);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_slice_number, END) {
  const char msg[] = "";
  shttp_u16 n;
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_SLICE_END, shttp_parse_slice_number(&n, &smsg));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_slice_number, NAN) {
  const char msg[] = "a";
  shttp_u16 n;
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_VALUE_INVALID,
                   shttp_parse_slice_number(&n, &smsg));
  assert_ptr_equal(msg + sizeof(msg) - 2, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_slice_skip, LEFT) {
  const char msg[] = "1234";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK,
                   shttp_parse_slice_skip(&smsg, sizeof(msg) - 2));
  assert_ptr_equal(msg + sizeof(msg) - 2, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_slice_skip, EVERYTHING) {
  const char msg[] = "1234";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK,
                   shttp_parse_slice_skip(&smsg, sizeof(msg) - 1));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_slice_skip, NOT_ENOUGH) {
  const char msg[] = "1234";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_SLICE_END,
                   shttp_parse_slice_skip(&smsg, sizeof(msg)));
  assert_ptr_equal(msg, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_slice_skip, END) {
  const char msg[] = "";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_SLICE_END,
                   shttp_parse_slice_skip(&smsg, sizeof(msg) + 1));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_slice_skip_until, FOUND) {
  const char msg[] = " :";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK, shttp_parse_slice_skip_until(&smsg, ':'));
  assert_ptr_equal(msg + sizeof(msg) - 2, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_slice_skip_until, NOT_FOUND) {
  const char msg[] = " .";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_SLICE_END,
                   shttp_parse_slice_skip_until(&smsg, ':'));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_slice_skip_until, END) {
  const char msg[] = "";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_SLICE_END,
                   shttp_parse_slice_skip_until(&smsg, ':'));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_slice_skip_until_or, FOUND) {
  const char msg[] = " :";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK,
                   shttp_parse_slice_skip_until_or(&smsg, "\n:"));
  assert_ptr_equal(msg + sizeof(msg) - 2, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_slice_skip_until_or, NOT_FOUND) {
  const char msg[] = " .";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_SLICE_END,
                   shttp_parse_slice_skip_until_or(&smsg, ";:"));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_slice_skip_until_or, END) {
  const char msg[] = "";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_SLICE_END,
                   shttp_parse_slice_skip_until_or(&smsg, ";'"));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_slice_skip_until_newline, FOUND_LF) {
  const char msg[] = "\n";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK,
                   shttp_parse_slice_skip_until_newline(&smsg));
  assert_ptr_equal(msg, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_slice_skip_until_newline, FOUND_CRLF) {
  const char msg[] = "\r\n";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK,
                   shttp_parse_slice_skip_until_newline(&smsg));
  assert_ptr_equal(msg, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_slice_skip_until_newline, NOT_FOUND) {
  const char msg[] = " ";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_NEWLINE_EXPECTED,
                   shttp_parse_slice_skip_until_newline(&smsg));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_slice_skip_until_newline, END) {
  const char msg[] = "";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_NEWLINE_EXPECTED,
                   shttp_parse_slice_skip_until_newline(&smsg));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_slice_skip_until_space, FOUND) {
  const char msg[] = " ";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK, shttp_parse_slice_skip_until_space(&smsg));
  assert_ptr_equal(msg, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_slice_skip_until_space, NOT_FOUND) {
  const char msg[] = ".";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_SPACE_EXPECTED,
                   shttp_parse_slice_skip_until_space(&smsg));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_slice_skip_until_space, END) {
  const char msg[] = "";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_SPACE_EXPECTED,
                   shttp_parse_slice_skip_until_space(&smsg));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_slice_skip_past, FOUND) {
  const char msg[] = " :";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK, shttp_parse_slice_skip_past(&smsg, ':'));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_slice_skip_past, NOT_FOUND) {
  const char msg[] = " .";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_SLICE_END,
                   shttp_parse_slice_skip_past(&smsg, ':'));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_slice_skip_past, END) {
  const char msg[] = "";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_SLICE_END,
                   shttp_parse_slice_skip_past(&smsg, ':'));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_slice_skip_past_or, FOUND) {
  const char msg[] = " :";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK,
                   shttp_parse_slice_skip_past_or(&smsg, "\n:"));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_slice_skip_past_or, NOT_FOUND) {
  const char msg[] = " .";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_SLICE_END,
                   shttp_parse_slice_skip_past_or(&smsg, ";:"));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_slice_skip_past_or, END) {
  const char msg[] = "";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_SLICE_END,
                   shttp_parse_slice_skip_past_or(&smsg, ": "));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_slice_skip_past_newline, FOUND_LF) {
  const char msg[] = "\n";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK, shttp_parse_slice_skip_past_newline(&smsg));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_slice_skip_past_newline, FOUND_CRLF) {
  const char msg[] = "\r\n";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK, shttp_parse_slice_skip_past_newline(&smsg));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_slice_skip_past_newline, NOT_FOUND) {
  const char msg[] = " ";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_NEWLINE_EXPECTED,
                   shttp_parse_slice_skip_past_newline(&smsg));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_slice_skip_past_newline, END) {
  const char msg[] = "";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_NEWLINE_EXPECTED,
                   shttp_parse_slice_skip_past_newline(&smsg));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_slice_skip_past_space, FOUND) {
  const char msg[] = " ";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK, shttp_parse_slice_skip_past_space(&smsg));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_slice_skip_past_space, NOT_FOUND) {
  const char msg[] = ".";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_SPACE_EXPECTED,
                   shttp_parse_slice_skip_past_space(&smsg));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_slice_skip_past_space, END) {
  const char msg[] = "";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_SPACE_EXPECTED,
                   shttp_parse_slice_skip_past_space(&smsg));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_slice_eq, PART) {
  char msg[] = "test";
  shttp_slice smsg = SHTTP_SLICE(msg);
  shttp_slice token = {msg, msg + sizeof(msg) - 2};
  assert_true(shttp_parse_slice_eq(token, smsg));
}

TEST(shttp_parse_slice_eq, EVERYTHING) {
  char msg[] = "test";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_true(shttp_parse_slice_eq(smsg, smsg));
}

TEST(shttp_parse_slice_eq, DIFFERENT) {
  char msg[] = "test";
  char buff[] = "tesa";
  shttp_slice smsg = SHTTP_SLICE(msg);
  shttp_slice sbuff = SHTTP_SLICE(buff);
  assert_false(shttp_parse_slice_eq(sbuff, smsg));
}

TEST(shttp_parse_method, GET) {
  shttp_request req;
  char msg[] = "GET";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK, shttp_parse_method(&req, &smsg));
  assert_int_equal(SHTTP_METHOD_GET, req.method);
  assert_int_equal(msg + sizeof(msg) - 1, smsg.begin);
}

TEST(shttp_parse_method, POST) {
  shttp_request req;
  char msg[] = "POST";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK, shttp_parse_method(&req, &smsg));
  assert_int_equal(SHTTP_METHOD_POST, req.method);
  assert_int_equal(msg + sizeof(msg) - 1, smsg.begin);
}

TEST(shttp_parse_path, SLASH) {
  shttp_request req;
  char msg[] = "/ ";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK, shttp_parse_path(&req, &smsg));
  assert_ptr_equal(msg + sizeof("/") - 1, smsg.begin);
  assert_ptr_equal(msg, req.path.begin);
  assert_ptr_equal(msg + sizeof("/") - 1, req.path.end);
}

TEST(shttp_parse_version, 1_1) {
  shttp_request req;
  char msg[] = "HTTP/1.1";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK, shttp_parse_version(&req, &smsg));
  assert_int_equal(SHTTP_VERSION_1_1, req.version);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
}

TEST(shttp_parse_version, 3_0) {
  shttp_request req;
  char msg[] = "HTTP/3.0";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK, shttp_parse_version(&req, &smsg));
  assert_int_equal(SHTTP_VERSION_3_0, req.version);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
}

TEST(shttp_parse_version, 1_2) {
  shttp_request req;
  char msg[] = "HTTP/1.2";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_VALUE_INVALID,
                   shttp_parse_version(&req, &smsg));
  assert_ptr_equal(msg + sizeof("HTTP/") - 1, smsg.begin);
}

TEST(shttp_parse_start_line, BASIC) {
  char msg[] = "GET / HTTP/1.1\r\n";
  shttp_request req;
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK, shttp_parse_start_line(&req, &smsg));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_int_equal(SHTTP_METHOD_GET, req.method);
  assert_ptr_equal(msg + sizeof("GET ") - 1, req.path.begin);
  assert_ptr_equal(msg + sizeof("GET /") - 1, req.path.end);
  assert_int_equal(SHTTP_VERSION_1_1, req.version);
}

TEST(shttp_parse_slices, HEADER) {
  const char msg[] = "Header: value value2\r\n\r\n";
  shttp_slice smsg = SHTTP_SLICE(msg);
  shttp_request req;
  assert_int_equal(SHTTP_STATUS_OK, shttp_parse_slices(&req, &smsg));
  assert_ptr_equal(msg, req.headers.begin);
  assert_ptr_equal(msg + sizeof(msg) - 3, req.headers.end);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_slices, HEADER_BODY) {
  const char msg[] = "Header: value value2\r\n\r\nBody message\r\n";
  shttp_slice smsg = SHTTP_SLICE(msg);
  shttp_request req;
  assert_int_equal(SHTTP_STATUS_OK, shttp_parse_slices(&req, &smsg));
  assert_ptr_equal(msg, req.headers.begin);
  assert_ptr_equal(msg + sizeof("Header: value value2\r\n") - 1,
                   req.headers.end);
  assert_ptr_equal(msg + sizeof("Header: value value2\r\n\r\n") - 1,
                   smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 3, smsg.end);
}

TEST(shttp_parse_header_next, SINGLE) {
  const char msg[] = "key: value value2\r\n";
  shttp_slice smsg = SHTTP_SLICE(msg);
  shttp_slice out;
  assert_int_equal(SHTTP_STATUS_OK, shttp_parse_header_next(&out, &smsg));
  assert_ptr_equal(msg, out.begin);
  assert_ptr_equal(msg + sizeof("key: value value2") - 1, out.end);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_header_next, MULTIPLE) {
  const char msg[] = "key: value value3\r\nkey: value value2\r\n";
  shttp_slice smsg = SHTTP_SLICE(msg);
  shttp_slice out;
  assert_int_equal(SHTTP_STATUS_OK, shttp_parse_header_next(&out, &smsg));
  assert_ptr_equal(msg, out.begin);
  assert_ptr_equal(msg + sizeof("key: value value3") - 1, out.end);
  assert_ptr_equal(msg + sizeof("key: value value3\r\n") - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);

  assert_int_equal(SHTTP_STATUS_OK, shttp_parse_header_next(&out, &smsg));
  assert_ptr_equal(msg + sizeof("key: value value3\r\n") - 1, out.begin);
  assert_ptr_equal(msg + sizeof("key: value value3\r\nkey: value value2") - 1,
                   out.end);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_header_key, BASIC) {
  const char msg[] = "key: value value2";
  shttp_slice smsg = SHTTP_SLICE(msg);
  shttp_slice out;
  assert_int_equal(SHTTP_STATUS_OK, shttp_parse_header_key(&out, &smsg));
  assert_ptr_equal(msg, out.begin);
  assert_ptr_equal(msg + sizeof("key") - 1, out.end);
  assert_ptr_equal(msg + sizeof("key: ") - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_header_value, SINGLE) {
  const char msg[] = "value\r\n";
  shttp_slice smsg = SHTTP_SLICE(msg);
  shttp_slice out;
  assert_int_equal(SHTTP_STATUS_OK, shttp_parse_header_value(&out, &smsg));
  assert_ptr_equal(msg, out.begin);
  assert_ptr_equal(msg + sizeof(msg) - 3, out.end);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_header_value, COMMA) {
  const char msg[] = "value,value2";
  shttp_slice smsg = SHTTP_SLICE(msg);
  shttp_slice out;
  assert_int_equal(SHTTP_STATUS_OK, shttp_parse_header_value(&out, &smsg));
  assert_ptr_equal(msg, out.begin);
  assert_ptr_equal(msg + sizeof("value") - 1, out.end);
  assert_ptr_equal(msg + sizeof("value,") - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_header_value, SPACE) {
  const char msg[] = "value value2";
  shttp_slice smsg = SHTTP_SLICE(msg);
  shttp_slice out;
  assert_int_equal(SHTTP_STATUS_OK, shttp_parse_header_value(&out, &smsg));
  assert_ptr_equal(msg, out.begin);
  assert_ptr_equal(msg + sizeof("value") - 1, out.end);
  assert_ptr_equal(msg + sizeof("value ") - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_parse_header_value, COMMA_SPACE) {
  const char msg[] = "value, value2";
  shttp_slice smsg = SHTTP_SLICE(msg);
  shttp_slice out;
  assert_int_equal(SHTTP_STATUS_OK, shttp_parse_header_value(&out, &smsg));
  assert_ptr_equal(msg, out.begin);
  assert_ptr_equal(msg + sizeof("value") - 1, out.end);
  assert_ptr_equal(msg + sizeof("value, ") - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

int main(void) {
  const struct CMUnitTest tests[] = {
    ADD(shttp_parse_slice_newline, CRLF),
    ADD(shttp_parse_slice_newline, LF),
    ADD(shttp_parse_slice_newline, CR_SPACE),
    ADD(shttp_parse_slice_newline, CR_END),
    ADD(shttp_parse_slice_newline, END),
    ADD(shttp_parse_slice_space, SPACE),
    ADD(shttp_parse_slice_space, NOT_SPACE),
    ADD(shttp_parse_slice_space, END),
    ADD(shttp_parse_slice_space_optional, SPACE),
    ADD(shttp_parse_slice_space_optional, NOT_SPACE),
    ADD(shttp_parse_slice_space_optional, END),
    ADD(shttp_parse_slice_number, 90),
    ADD(shttp_parse_slice_number, END),
    ADD(shttp_parse_slice_number, NAN),
    ADD(shttp_parse_slice_skip, LEFT),
    ADD(shttp_parse_slice_skip, EVERYTHING),
    ADD(shttp_parse_slice_skip, NOT_ENOUGH),
    ADD(shttp_parse_slice_skip, END),
    ADD(shttp_parse_slice_skip_until, FOUND),
    ADD(shttp_parse_slice_skip_until, NOT_FOUND),
    ADD(shttp_parse_slice_skip_until, END),
    ADD(shttp_parse_slice_skip_until_or, FOUND),
    ADD(shttp_parse_slice_skip_until_or, NOT_FOUND),
    ADD(shttp_parse_slice_skip_until_or, END),
    ADD(shttp_parse_slice_skip_until_newline, FOUND_LF),
    ADD(shttp_parse_slice_skip_until_newline, FOUND_CRLF),
    ADD(shttp_parse_slice_skip_until_newline, NOT_FOUND),
    ADD(shttp_parse_slice_skip_until_newline, END),
    ADD(shttp_parse_slice_skip_until_space, FOUND),
    ADD(shttp_parse_slice_skip_until_space, NOT_FOUND),
    ADD(shttp_parse_slice_skip_until_space, END),
    ADD(shttp_parse_slice_skip_past, FOUND),
    ADD(shttp_parse_slice_skip_past, NOT_FOUND),
    ADD(shttp_parse_slice_skip_past, END),
    ADD(shttp_parse_slice_skip_past_or, FOUND),
    ADD(shttp_parse_slice_skip_past_or, NOT_FOUND),
    ADD(shttp_parse_slice_skip_past_or, END),
    ADD(shttp_parse_slice_skip_past_newline, FOUND_LF),
    ADD(shttp_parse_slice_skip_past_newline, FOUND_CRLF),
    ADD(shttp_parse_slice_skip_past_newline, NOT_FOUND),
    ADD(shttp_parse_slice_skip_past_newline, END),
    ADD(shttp_parse_slice_skip_past_space, FOUND),
    ADD(shttp_parse_slice_skip_past_space, NOT_FOUND),
    ADD(shttp_parse_slice_skip_past_space, END),
    ADD(shttp_parse_slice_eq, PART),
    ADD(shttp_parse_slice_eq, EVERYTHING),
    ADD(shttp_parse_slice_eq, DIFFERENT),
    ADD(shttp_parse_method, GET),
    ADD(shttp_parse_method, POST),
    ADD(shttp_parse_path, SLASH),
    ADD(shttp_parse_version, 1_1),
    ADD(shttp_parse_version, 3_0),
    ADD(shttp_parse_version, 1_2),
    ADD(shttp_parse_start_line, BASIC),
    ADD(shttp_parse_slices, HEADER),
    ADD(shttp_parse_slices, HEADER_BODY),
    ADD(shttp_parse_header_next, SINGLE),
    ADD(shttp_parse_header_next, MULTIPLE),
    ADD(shttp_parse_header_key, BASIC),
    ADD(shttp_parse_header_value, SINGLE),
    ADD(shttp_parse_header_value, COMMA),
    ADD(shttp_parse_header_value, SPACE),
    ADD(shttp_parse_header_value, COMMA_SPACE),
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}
