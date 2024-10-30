// clang-format off
#include "../test.h"

#include "slice.c"
// clang-format on

TEST(shttp_slice_parse_newline, CRLF) {
  const char msg[] = "\r\n";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK, shttp_slice_parse_newline(&smsg));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_slice_parse_newline, LF) {
  const char msg[] = "\n";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK, shttp_slice_parse_newline(&smsg));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_slice_parse_newline, CR_SPACE) {
  const char msg[] = "\r ";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_NEWLINE_EXPECTED,
                   shttp_slice_parse_newline(&smsg));
  assert_ptr_equal(msg + sizeof(msg) - 2, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_slice_parse_newline, CR_END) {
  const char msg[] = "\r";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_SLICE_END, shttp_slice_parse_newline(&smsg));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_slice_parse_newline, END) {
  const char msg[] = "";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_SLICE_END, shttp_slice_parse_newline(&smsg));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_slice_parse_space, SPACE) {
  const char msg[] = " ";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK, shttp_slice_parse_space(&smsg));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_slice_parse_space, NOT_SPACE) {
  const char msg[] = "s";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_SPACE_EXPECTED, shttp_slice_parse_space(&smsg));
  assert_ptr_equal(msg + sizeof(msg) - 2, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_slice_parse_space, END) {
  const char msg[] = "";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_SLICE_END, shttp_slice_parse_space(&smsg));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_slice_parse_space_optional, SPACE) {
  const char msg[] = " ";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK, shttp_slice_parse_space_optional(&smsg));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_slice_parse_space_optional, NOT_SPACE) {
  const char msg[] = "s";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK, shttp_slice_parse_space_optional(&smsg));
  assert_ptr_equal(msg + sizeof(msg) - 2, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_slice_parse_space_optional, END) {
  const char msg[] = "";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_SLICE_END,
                   shttp_slice_parse_space_optional(&smsg));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_slice_parse_u32, 90) {
  const char msg[] = "90";
  uint32_t n;
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK, shttp_slice_parse_u32(&n, &smsg));
  assert_int_equal(90, n);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_slice_parse_u32, END) {
  const char msg[] = "";
  uint32_t n;
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_SLICE_END, shttp_slice_parse_u32(&n, &smsg));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_slice_parse_u32, NAN) {
  const char msg[] = "a";
  uint32_t n;
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_VALUE_INVALID,
                   shttp_slice_parse_u32(&n, &smsg));
  assert_ptr_equal(msg + sizeof(msg) - 2, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_slice_parse_i32, NEGATIVE_90) {
  const char msg[] = "-90";
  int32_t n;
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK, shttp_slice_parse_i32(&n, &smsg));
  assert_int_equal(-90, n);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_slice_parse_i32, END) {
  const char msg[] = "";
  int32_t n;
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_SLICE_END, shttp_slice_parse_i32(&n, &smsg));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_slice_parse_i32, NAN) {
  const char msg[] = "a";
  int32_t n;
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_VALUE_INVALID,
                   shttp_slice_parse_i32(&n, &smsg));
  assert_ptr_equal(msg + sizeof(msg) - 2, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_slice_parse_format, NO_ARGS) {
  shttp_slice text = SHTTP_SLICE("Text to check");
  assert_int_equal(SHTTP_STATUS_OK, shttp_slice_parse_format(&text, text));
  assert_ptr_equal(text.end, text.begin);
}

TEST(shttp_slice_parse_format, ONLY_u32) {
  uint32_t val;
  shttp_slice slice = SHTTP_SLICE("23");
  assert_int_equal(SHTTP_STATUS_OK, shttp_slice_parse_format(
                                      &slice, SHTTP_SLICE("{u32}"), &val));
  assert_int_equal(23, val);
  assert_ptr_equal(slice.end, slice.begin);
}

TEST(shttp_slice_parse_format, MULTIPLE_u32) {
  uint32_t val;
  uint32_t val2;
  shttp_slice slice = SHTTP_SLICE("23 32");
  assert_int_equal(
    SHTTP_STATUS_OK,
    shttp_slice_parse_format(&slice, SHTTP_SLICE("{u32} {u32}"), &val, &val2));
  assert_int_equal(23, val);
  assert_int_equal(32, val2);
  assert_ptr_equal(slice.end, slice.begin);
}

TEST(shttp_slice_parse_format, u32_INSIDE_TEXT) {
  uint32_t val;
  shttp_slice slice = SHTTP_SLICE("thing: 23,");
  assert_int_equal(
    SHTTP_STATUS_OK,
    shttp_slice_parse_format(&slice, SHTTP_SLICE("thing: {u32},"), &val));
  assert_int_equal(23, val);
  assert_ptr_equal(slice.end, slice.begin);
}

TEST(shttp_slice_parse_format, ONLY_slice) {
  char buff[25];
  shttp_mut_slice slice = SHTTP_MUT_SLICE(buff);
  shttp_slice res = SHTTP_SLICE("string");
  assert_int_equal(SHTTP_STATUS_OK, shttp_slice_parse_format(
                                      &res, SHTTP_SLICE("{slice}"), slice));
  assert_memory_equal("string", buff, sizeof("string") - 1);
  assert_ptr_equal(res.end, res.begin);
}

TEST(shttp_slice_parse_format, MULTIPLE_slice) {
  char buff[25];
  char buff2[25];
  shttp_mut_slice slice = SHTTP_MUT_SLICE(buff);
  shttp_mut_slice slice2 = SHTTP_MUT_SLICE(buff2);
  shttp_slice res = SHTTP_SLICE("string str");
  assert_int_equal(SHTTP_STATUS_OK,
                   shttp_slice_parse_format(
                     &res, SHTTP_SLICE("{slice} {slice}"), slice, slice2));
  assert_memory_equal("string", buff, sizeof("string") - 1);
  assert_memory_equal("str", buff2, sizeof("str") - 1);
  assert_ptr_equal(res.end, res.begin);
}

TEST(shttp_slice_parse_format, slice_INSIDE_TEXT) {
  char buff[25];
  shttp_mut_slice slice = SHTTP_MUT_SLICE(buff);
  shttp_slice res = SHTTP_SLICE("thing: string,");
  assert_int_equal(
    SHTTP_STATUS_OK,
    shttp_slice_parse_format(&res, SHTTP_SLICE("thing: {slice},"), slice));
  assert_memory_equal("string", buff, sizeof("string") - 1);
  assert_ptr_equal(res.end, res.begin);
}

TEST(shttp_slice_parse_format, NOT_CLOSED) {
  uint32_t val;
  shttp_slice slice = SHTTP_SLICE("23");
  assert_int_equal(SHTTP_STATUS_FORMAT_INVALID,
                   shttp_slice_parse_format(&slice, SHTTP_SLICE("{u32"), &val));
  assert_ptr_equal(slice.end - sizeof("23") + 1, slice.begin);
}

TEST(shttp_slice_parse_format, u32_INSIDE_TEXT_ESCAPED) {
  uint32_t val;
  shttp_slice slice = SHTTP_SLICE("{thing: 23,}");
  assert_int_equal(
    SHTTP_STATUS_OK,
    shttp_slice_parse_format(&slice, SHTTP_SLICE("{{thing: {u32},}"), &val));
  assert_int_equal(23, val);
  assert_ptr_equal(slice.end, slice.begin);
}

TEST(shttp_slice_parse_format, u32_AFTER_ESCAPE) {
  uint32_t val;
  shttp_slice slice = SHTTP_SLICE("{23}");
  assert_int_equal(SHTTP_STATUS_OK, shttp_slice_parse_format(
                                      &slice, SHTTP_SLICE("{{{u32}}"), &val));
  assert_int_equal(23, val);
  assert_ptr_equal(slice.end, slice.begin);
}

TEST(shttp_slice_skip, LEFT) {
  const char msg[] = "1234";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK, shttp_slice_skip(&smsg, sizeof(msg) - 2));
  assert_ptr_equal(msg + sizeof(msg) - 2, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_slice_skip, EVERYTHING) {
  const char msg[] = "1234";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK, shttp_slice_skip(&smsg, sizeof(msg) - 1));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_slice_skip, NOT_ENOUGH) {
  const char msg[] = "1234";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_SLICE_END,
                   shttp_slice_skip(&smsg, sizeof(msg)));
  assert_ptr_equal(msg, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_slice_skip, END) {
  const char msg[] = "";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_SLICE_END,
                   shttp_slice_skip(&smsg, sizeof(msg) + 1));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_slice_skip_until, FOUND) {
  const char msg[] = " :";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK, shttp_slice_skip_until(&smsg, ':'));
  assert_ptr_equal(msg + sizeof(msg) - 2, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_slice_skip_until, NOT_FOUND) {
  const char msg[] = " .";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_SLICE_END, shttp_slice_skip_until(&smsg, ':'));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_slice_skip_until, END) {
  const char msg[] = "";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_SLICE_END, shttp_slice_skip_until(&smsg, ':'));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_slice_skip_until_or, FOUND) {
  const char msg[] = " :";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK, shttp_slice_skip_until_or(&smsg, "\n:"));
  assert_ptr_equal(msg + sizeof(msg) - 2, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_slice_skip_until_or, NOT_FOUND) {
  const char msg[] = " .";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_SLICE_END,
                   shttp_slice_skip_until_or(&smsg, ";:"));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_slice_skip_until_or, END) {
  const char msg[] = "";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_SLICE_END,
                   shttp_slice_skip_until_or(&smsg, ";'"));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_slice_skip_until_newline, FOUND_LF) {
  const char msg[] = "\n";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK, shttp_slice_skip_until_newline(&smsg));
  assert_ptr_equal(msg, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_slice_skip_until_newline, FOUND_CRLF) {
  const char msg[] = "\r\n";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK, shttp_slice_skip_until_newline(&smsg));
  assert_ptr_equal(msg, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_slice_skip_until_newline, NOT_FOUND) {
  const char msg[] = " ";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_NEWLINE_EXPECTED,
                   shttp_slice_skip_until_newline(&smsg));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_slice_skip_until_newline, END) {
  const char msg[] = "";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_NEWLINE_EXPECTED,
                   shttp_slice_skip_until_newline(&smsg));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_slice_skip_until_space, FOUND) {
  const char msg[] = " ";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK, shttp_slice_skip_until_space(&smsg));
  assert_ptr_equal(msg, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_slice_skip_until_space, NOT_FOUND) {
  const char msg[] = ".";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_SPACE_EXPECTED,
                   shttp_slice_skip_until_space(&smsg));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_slice_skip_until_space, END) {
  const char msg[] = "";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_SPACE_EXPECTED,
                   shttp_slice_skip_until_space(&smsg));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_slice_skip_past, FOUND) {
  const char msg[] = " :";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK, shttp_slice_skip_past(&smsg, ':'));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_slice_skip_past, NOT_FOUND) {
  const char msg[] = " .";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_SLICE_END, shttp_slice_skip_past(&smsg, ':'));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_slice_skip_past, END) {
  const char msg[] = "";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_SLICE_END, shttp_slice_skip_past(&smsg, ':'));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_slice_skip_past_or, FOUND) {
  const char msg[] = " :";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK, shttp_slice_skip_past_or(&smsg, "\n:"));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_slice_skip_past_or, NOT_FOUND) {
  const char msg[] = " .";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_SLICE_END,
                   shttp_slice_skip_past_or(&smsg, ";:"));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_slice_skip_past_or, END) {
  const char msg[] = "";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_SLICE_END,
                   shttp_slice_skip_past_or(&smsg, ": "));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_slice_skip_past_newline, FOUND_LF) {
  const char msg[] = "\n";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK, shttp_slice_skip_past_newline(&smsg));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_slice_skip_past_newline, FOUND_CRLF) {
  const char msg[] = "\r\n";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK, shttp_slice_skip_past_newline(&smsg));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_slice_skip_past_newline, NOT_FOUND) {
  const char msg[] = " ";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_NEWLINE_EXPECTED,
                   shttp_slice_skip_past_newline(&smsg));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_slice_skip_past_newline, END) {
  const char msg[] = "";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_NEWLINE_EXPECTED,
                   shttp_slice_skip_past_newline(&smsg));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_slice_skip_past_space, FOUND) {
  const char msg[] = " ";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK, shttp_slice_skip_past_space(&smsg));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_slice_skip_past_space, NOT_FOUND) {
  const char msg[] = ".";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_SPACE_EXPECTED,
                   shttp_slice_skip_past_space(&smsg));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_slice_skip_past_space, END) {
  const char msg[] = "";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_SPACE_EXPECTED,
                   shttp_slice_skip_past_space(&smsg));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_slice_starts_with, WRONG_ORDER) {
  char msg[] = "test";
  shttp_slice smsg = SHTTP_SLICE(msg);
  shttp_slice token = {msg, msg + sizeof(msg) - 2};
  assert_false(shttp_slice_starts_with(token, smsg));
}

TEST(shttp_slice_starts_with, PART) {
  char msg[] = "test";
  shttp_slice smsg = SHTTP_SLICE(msg);
  shttp_slice token = {msg, msg + sizeof(msg) - 2};
  assert_true(shttp_slice_starts_with(smsg, token));
}

TEST(shttp_slice_starts_with, EVERYTHING) {
  char msg[] = "test";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_true(shttp_slice_starts_with(smsg, smsg));
}

TEST(shttp_slice_starts_with, DIFFERENT) {
  char msg[] = "test";
  char buff[] = "tesa";
  shttp_slice smsg = SHTTP_SLICE(msg);
  shttp_slice sbuff = SHTTP_SLICE(buff);
  assert_false(shttp_slice_starts_with(sbuff, smsg));
}

TEST(shttp_slice_eq, PART) {
  char msg[] = "test";
  shttp_slice smsg = SHTTP_SLICE(msg);
  shttp_slice token = {msg, msg + sizeof(msg) - 2};
  assert_false(shttp_slice_eq(token, smsg));
}

TEST(shttp_slice_eq, EVERYTHING) {
  char msg[] = "test";
  shttp_slice smsg = SHTTP_SLICE(msg);
  assert_true(shttp_slice_eq(smsg, smsg));
}

TEST(shttp_slice_eq, DIFFERENT) {
  char msg[] = "test";
  char buff[] = "tesa";
  shttp_slice smsg = SHTTP_SLICE(msg);
  shttp_slice sbuff = SHTTP_SLICE(buff);
  assert_false(shttp_slice_eq(sbuff, smsg));
}

TEST(shttp_slice_eq_until, PART) {
  char msg[] = "test;as";
  char token[] = "test";
  assert_true(shttp_slice_eq_until(SHTTP_SLICE(token), SHTTP_SLICE(msg), ';'));
}

TEST(shttp_slice_eq_until, EVERYTHING) {
  char msg[] = "test";
  char token[] = "test";
  assert_true(shttp_slice_eq_until(SHTTP_SLICE(token), SHTTP_SLICE(msg), ';'));
}

TEST(shttp_slice_eq_until, DIFFERENT) {
  char msg[] = "test;as";
  char token[] = "tess";
  assert_false(shttp_slice_eq_until(SHTTP_SLICE(token), SHTTP_SLICE(msg), ';'));
}

TEST(shttp_header_next, SINGLE) {
  const char msg[] = "key: value value2\r\n";
  shttp_slice smsg = SHTTP_SLICE(msg);
  shttp_slice out;
  assert_int_equal(SHTTP_STATUS_OK, shttp_header_next(&out, &smsg));
  assert_ptr_equal(msg, out.begin);
  assert_ptr_equal(msg + sizeof("key: value value2") - 1, out.end);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_header_next, MULTIPLE) {
  const char msg[] = "key: value value3\r\nkey: value value2\r\n";
  shttp_slice smsg = SHTTP_SLICE(msg);
  shttp_slice out;
  assert_int_equal(SHTTP_STATUS_OK, shttp_header_next(&out, &smsg));
  assert_ptr_equal(msg, out.begin);
  assert_ptr_equal(msg + sizeof("key: value value3") - 1, out.end);
  assert_ptr_equal(msg + sizeof("key: value value3\r\n") - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);

  assert_int_equal(SHTTP_STATUS_OK, shttp_header_next(&out, &smsg));
  assert_ptr_equal(msg + sizeof("key: value value3\r\n") - 1, out.begin);
  assert_ptr_equal(msg + sizeof("key: value value3\r\nkey: value value2") - 1,
                   out.end);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_header_key, BASIC) {
  const char msg[] = "key: value value2";
  shttp_slice smsg = SHTTP_SLICE(msg);
  shttp_slice out;
  assert_int_equal(SHTTP_STATUS_OK, shttp_header_key(&out, &smsg));
  assert_ptr_equal(msg, out.begin);
  assert_ptr_equal(msg + sizeof("key") - 1, out.end);
  assert_ptr_equal(msg + sizeof("key: ") - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_header_value, SINGLE) {
  const char msg[] = "value\r\n";
  shttp_slice smsg = SHTTP_SLICE(msg);
  shttp_slice out;
  assert_int_equal(SHTTP_STATUS_OK, shttp_header_value(&out, &smsg));
  assert_ptr_equal(msg, out.begin);
  assert_ptr_equal(msg + sizeof(msg) - 3, out.end);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_header_value, COMMA) {
  const char msg[] = "value,value2";
  shttp_slice smsg = SHTTP_SLICE(msg);
  shttp_slice out;
  assert_int_equal(SHTTP_STATUS_OK, shttp_header_value(&out, &smsg));
  assert_ptr_equal(msg, out.begin);
  assert_ptr_equal(msg + sizeof("value") - 1, out.end);
  assert_ptr_equal(msg + sizeof("value,") - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_header_value, SPACE) {
  const char msg[] = "value value2";
  shttp_slice smsg = SHTTP_SLICE(msg);
  shttp_slice out;
  assert_int_equal(SHTTP_STATUS_OK, shttp_header_value(&out, &smsg));
  assert_ptr_equal(msg, out.begin);
  assert_ptr_equal(msg + sizeof("value") - 1, out.end);
  assert_ptr_equal(msg + sizeof("value ") - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_header_value, COMMA_SPACE) {
  const char msg[] = "value, value2";
  shttp_slice smsg = SHTTP_SLICE(msg);
  shttp_slice out;
  assert_int_equal(SHTTP_STATUS_OK, shttp_header_value(&out, &smsg));
  assert_ptr_equal(msg, out.begin);
  assert_ptr_equal(msg + sizeof("value") - 1, out.end);
  assert_ptr_equal(msg + sizeof("value, ") - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
}

TEST(shttp_slice_insert_newline, BASIC) {
  char msg[sizeof("\r\n")];
  shttp_mut_slice smsg = SHTTP_MUT_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK, shttp_slice_insert_newline(&smsg));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_string_equal("\r\n", msg);
}

TEST(shttp_slice_insert_newline, TOO_SHORT) {
  char msg[sizeof("\r")];
  shttp_mut_slice smsg = SHTTP_MUT_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_SLICE_END, shttp_slice_insert_newline(&smsg));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_string_equal("\r", msg);
}

TEST(shttp_slice_insert_u32, BASIC) {
  char buff[5];
  shttp_mut_slice slice = SHTTP_MUT_SLICE(buff);
  assert_int_equal(SHTTP_STATUS_OK, shttp_slice_insert_u32(&slice, 1234));
  assert_memory_equal("1234", buff, sizeof("1234") - 1);
  assert_ptr_equal(slice.end, slice.begin);
}

TEST(shttp_slice_insert_u32, TOO_SHORT) {
  char buff[4];
  shttp_mut_slice slice = SHTTP_MUT_SLICE(buff);
  assert_int_equal(SHTTP_STATUS_SLICE_END,
                   shttp_slice_insert_u32(&slice, 1234));
  assert_memory_equal("123", buff, sizeof("123") - 1);
  assert_ptr_equal(slice.end, slice.begin);
}

TEST(shttp_slice_insert_i32, BASIC) {
  char buff[6];
  shttp_mut_slice slice = SHTTP_MUT_SLICE(buff);
  assert_int_equal(SHTTP_STATUS_OK, shttp_slice_insert_i32(&slice, -1234));
  assert_memory_equal("-1234", buff, sizeof("-1234") - 1);
  assert_ptr_equal(slice.end, slice.begin);
}

TEST(shttp_slice_insert_i32, TOO_SHORT) {
  char buff[5];
  shttp_mut_slice slice = SHTTP_MUT_SLICE(buff);
  assert_int_equal(SHTTP_STATUS_SLICE_END,
                   shttp_slice_insert_i32(&slice, -1234));
  assert_memory_equal("-123", buff, sizeof("-123") - 1);
  assert_ptr_equal(slice.end, slice.begin);
}

TEST(shttp_slice_insert_i32, ONLY_MINUS) {
  char buff[2];
  shttp_mut_slice slice = SHTTP_MUT_SLICE(buff);
  assert_int_equal(SHTTP_STATUS_SLICE_END,
                   shttp_slice_insert_i32(&slice, -1234));
  assert_memory_equal("-", buff, sizeof("-") - 1);
  assert_ptr_equal(slice.end, slice.begin);
}

TEST(shttp_slice_insert_format, NO_ARGS) {
  char buff[10];
  const char res[] = "string";
  shttp_mut_slice slice = SHTTP_MUT_SLICE(buff);
  assert_int_equal(SHTTP_STATUS_OK,
                   shttp_slice_insert_format(&slice, SHTTP_SLICE(res)));
  assert_memory_equal(res, buff, sizeof(res) - 1);
}

TEST(shttp_slice_insert_format, ONLY_u32) {
  char buff[10];
  const char res[] = "23";
  shttp_mut_slice slice = SHTTP_MUT_SLICE(buff);
  assert_int_equal(SHTTP_STATUS_OK,
                   shttp_slice_insert_format(&slice, SHTTP_SLICE("{u32}"), 23));
  assert_memory_equal(res, buff, sizeof(res) - 1);
}

TEST(shttp_slice_insert_format, MULTIPLE_u32) {
  char buff[10];
  const char res[] = "23 42";
  shttp_mut_slice slice = SHTTP_MUT_SLICE(buff);
  assert_int_equal(
    SHTTP_STATUS_OK,
    shttp_slice_insert_format(&slice, SHTTP_SLICE("{u32} {u32}"), 23, 42));
  assert_memory_equal(res, buff, sizeof(res) - 1);
}

TEST(shttp_slice_insert_format, u32_INSIDE_TEXT) {
  char buff[15];
  const char res[] = "value: 23,";
  shttp_mut_slice slice = SHTTP_MUT_SLICE(buff);
  assert_int_equal(
    SHTTP_STATUS_OK,
    shttp_slice_insert_format(&slice, SHTTP_SLICE("value: {u32},"), 23));
  assert_memory_equal(res, buff, sizeof(res) - 1);
}

TEST(shttp_slice_insert_format, ONLY_slice) {
  char buff[25];
  shttp_mut_slice slice = SHTTP_MUT_SLICE(buff);
  assert_int_equal(SHTTP_STATUS_OK,
                   shttp_slice_insert_format(&slice, SHTTP_SLICE("{slice}"),
                                             SHTTP_SLICE("string")));
  assert_memory_equal("string", buff, sizeof("string") - 1);
}

TEST(shttp_slice_insert_format, MULTIPLE_slice) {
  char buff[50];
  shttp_mut_slice slice = SHTTP_MUT_SLICE(buff);
  assert_int_equal(
    SHTTP_STATUS_OK,
    shttp_slice_insert_format(&slice, SHTTP_SLICE("{slice} {slice}"),
                              SHTTP_SLICE("string"), SHTTP_SLICE("str")));
  assert_memory_equal("string str", buff, sizeof("string str") - 1);
}

TEST(shttp_slice_insert_format, slice_INSIDE_TEXT) {
  char buff[25];
  shttp_mut_slice slice = SHTTP_MUT_SLICE(buff);
  assert_int_equal(SHTTP_STATUS_OK, shttp_slice_insert_format(
                                      &slice, SHTTP_SLICE("thing: {slice},"),
                                      SHTTP_SLICE("string")));
  assert_memory_equal("thing: string", buff, sizeof("thing: string") - 1);
}

TEST(shttp_slice_insert_format, u32_INSIDE_TEXT_ESCAPED) {
  char buff[15];
  const char res[] = "{value: 23,}";
  shttp_mut_slice slice = SHTTP_MUT_SLICE(buff);
  assert_int_equal(
    SHTTP_STATUS_OK,
    shttp_slice_insert_format(&slice, SHTTP_SLICE("{{value: {u32},}"), 23));
  assert_memory_equal(res, buff, sizeof(res) - 1);
}

TEST(shttp_slice_insert_format, u32_AFTER_ESCAPE) {
  char buff[15];
  const char res[] = "{23}";
  shttp_mut_slice slice = SHTTP_MUT_SLICE(buff);
  assert_int_equal(SHTTP_STATUS_OK, shttp_slice_insert_format(
                                      &slice, SHTTP_SLICE("{{{u32}}"), 23));
  assert_memory_equal(res, buff, sizeof(res) - 1);
}

TEST(shttp_slice_cpy, BASIC) {
  const char slice[] = "slice";
  char msg[sizeof(slice)];
  shttp_slice sslice = SHTTP_SLICE(slice);
  shttp_mut_slice smsg = SHTTP_MUT_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK, shttp_slice_cpy(&smsg, sslice));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_memory_equal(slice, msg, sizeof(msg) - 1);
}

TEST(shttp_slice_cpy, TOO_SHORT) {
  const char slice[] = "slice";
  char msg[sizeof(slice) - 1];
  shttp_slice sslice = SHTTP_SLICE(slice);
  shttp_mut_slice smsg = SHTTP_MUT_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_SLICE_END, shttp_slice_cpy(&smsg, sslice));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_memory_equal("slic", msg, sizeof(msg) - 1);
}

int main(void) {
  const struct CMUnitTest tests[] = {
    ADD(shttp_slice_parse_newline, CRLF),
    ADD(shttp_slice_parse_newline, LF),
    ADD(shttp_slice_parse_newline, CR_SPACE),
    ADD(shttp_slice_parse_newline, CR_END),
    ADD(shttp_slice_parse_newline, END),
    ADD(shttp_slice_parse_space, SPACE),
    ADD(shttp_slice_parse_space, NOT_SPACE),
    ADD(shttp_slice_parse_space, END),
    ADD(shttp_slice_parse_space_optional, SPACE),
    ADD(shttp_slice_parse_space_optional, NOT_SPACE),
    ADD(shttp_slice_parse_space_optional, END),
    ADD(shttp_slice_parse_u32, 90),
    ADD(shttp_slice_parse_u32, END),
    ADD(shttp_slice_parse_u32, NAN),
    ADD(shttp_slice_parse_i32, NEGATIVE_90),
    ADD(shttp_slice_parse_i32, END),
    ADD(shttp_slice_parse_i32, NAN),
    ADD(shttp_slice_parse_format, NO_ARGS),
    ADD(shttp_slice_parse_format, ONLY_u32),
    ADD(shttp_slice_parse_format, MULTIPLE_u32),
    ADD(shttp_slice_parse_format, u32_INSIDE_TEXT),
    ADD(shttp_slice_parse_format, ONLY_slice),
    ADD(shttp_slice_parse_format, MULTIPLE_slice),
    ADD(shttp_slice_parse_format, slice_INSIDE_TEXT),
    ADD(shttp_slice_parse_format, NOT_CLOSED),
    ADD(shttp_slice_parse_format, u32_INSIDE_TEXT_ESCAPED),
    ADD(shttp_slice_parse_format, u32_AFTER_ESCAPE),
    ADD(shttp_slice_skip, LEFT),
    ADD(shttp_slice_skip, EVERYTHING),
    ADD(shttp_slice_skip, NOT_ENOUGH),
    ADD(shttp_slice_skip, END),
    ADD(shttp_slice_skip_until, FOUND),
    ADD(shttp_slice_skip_until, NOT_FOUND),
    ADD(shttp_slice_skip_until, END),
    ADD(shttp_slice_skip_until_or, FOUND),
    ADD(shttp_slice_skip_until_or, NOT_FOUND),
    ADD(shttp_slice_skip_until_or, END),
    ADD(shttp_slice_skip_until_newline, FOUND_LF),
    ADD(shttp_slice_skip_until_newline, FOUND_CRLF),
    ADD(shttp_slice_skip_until_newline, NOT_FOUND),
    ADD(shttp_slice_skip_until_newline, END),
    ADD(shttp_slice_skip_until_space, FOUND),
    ADD(shttp_slice_skip_until_space, NOT_FOUND),
    ADD(shttp_slice_skip_until_space, END),
    ADD(shttp_slice_skip_past, FOUND),
    ADD(shttp_slice_skip_past, NOT_FOUND),
    ADD(shttp_slice_skip_past, END),
    ADD(shttp_slice_skip_past_or, FOUND),
    ADD(shttp_slice_skip_past_or, NOT_FOUND),
    ADD(shttp_slice_skip_past_or, END),
    ADD(shttp_slice_skip_past_newline, FOUND_LF),
    ADD(shttp_slice_skip_past_newline, FOUND_CRLF),
    ADD(shttp_slice_skip_past_newline, NOT_FOUND),
    ADD(shttp_slice_skip_past_newline, END),
    ADD(shttp_slice_skip_past_space, FOUND),
    ADD(shttp_slice_skip_past_space, NOT_FOUND),
    ADD(shttp_slice_skip_past_space, END),
    ADD(shttp_slice_starts_with, WRONG_ORDER),
    ADD(shttp_slice_starts_with, PART),
    ADD(shttp_slice_starts_with, EVERYTHING),
    ADD(shttp_slice_starts_with, DIFFERENT),
    ADD(shttp_slice_eq, PART),
    ADD(shttp_slice_eq, EVERYTHING),
    ADD(shttp_slice_eq, DIFFERENT),
    ADD(shttp_slice_eq_until, PART),
    ADD(shttp_slice_eq_until, EVERYTHING),
    ADD(shttp_slice_eq_until, DIFFERENT),
    ADD(shttp_header_next, SINGLE),
    ADD(shttp_header_next, MULTIPLE),
    ADD(shttp_header_key, BASIC),
    ADD(shttp_header_value, SINGLE),
    ADD(shttp_header_value, COMMA),
    ADD(shttp_header_value, SPACE),
    ADD(shttp_header_value, COMMA_SPACE),
    ADD(shttp_slice_insert_newline, BASIC),
    ADD(shttp_slice_insert_newline, TOO_SHORT),
    ADD(shttp_slice_insert_u32, BASIC),
    ADD(shttp_slice_insert_u32, TOO_SHORT),
    ADD(shttp_slice_insert_i32, BASIC),
    ADD(shttp_slice_insert_i32, TOO_SHORT),
    ADD(shttp_slice_insert_i32, ONLY_MINUS),
    ADD(shttp_slice_insert_format, NO_ARGS),
    ADD(shttp_slice_insert_format, ONLY_u32),
    ADD(shttp_slice_insert_format, MULTIPLE_u32),
    ADD(shttp_slice_insert_format, u32_INSIDE_TEXT),
    ADD(shttp_slice_insert_format, ONLY_slice),
    ADD(shttp_slice_insert_format, MULTIPLE_slice),
    ADD(shttp_slice_insert_format, slice_INSIDE_TEXT),
    ADD(shttp_slice_insert_format, u32_INSIDE_TEXT_ESCAPED),
    ADD(shttp_slice_insert_format, u32_AFTER_ESCAPE),
    ADD(shttp_slice_cpy, BASIC),
    ADD(shttp_slice_cpy, TOO_SHORT),
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}
