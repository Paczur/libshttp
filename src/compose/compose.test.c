#include "compose.c"

#include "../test.h"

TEST(shttp_compose_slice_newline, BASIC) {
  char msg[sizeof("\r\n")];
  shttp_mut_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK, shttp_compose_slice_newline(&smsg));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_string_equal("\r\n", msg);
}

TEST(shttp_compose_slice_newline, TOO_SHORT) {
  char msg[sizeof("\r")];
  shttp_mut_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_SLICE_END, shttp_compose_slice_newline(&smsg));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_string_equal("\r", msg);
}

TEST(shttp_compose_slice_cpy, BASIC) {
  const char slice[] = "slice";
  char msg[sizeof(slice)];
  shttp_slice sslice = SHTTP_SLICE(slice);
  shttp_mut_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK, shttp_compose_slice_cpy(&smsg, sslice));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_string_equal(slice, msg);
}

TEST(shttp_compose_slice_cpy, TOO_SHORT) {
  const char slice[] = "slice";
  char msg[sizeof(slice) - 1];
  shttp_slice sslice = SHTTP_SLICE(slice);
  shttp_mut_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_SLICE_END,
                   shttp_compose_slice_cpy(&smsg, sslice));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_string_equal("slic", msg);
}

TEST(shttp_compose_version, 1_0) {
  const char ans[] = "HTTP/1.0";
  shttp_response res = {.version = SHTTP_VERSION_1_0};
  char msg[sizeof(ans)];
  shttp_mut_slice smsg = SHTTP_SLICE(msg);
  assert_int_equal(SHTTP_STATUS_OK, shttp_compose_version(&smsg, &res));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_string_equal(ans, msg);
}

TEST(shttp_compose_code, OK) {
  const char ans[] = "200 OK";
  char msg[sizeof(ans)];
  shttp_mut_slice smsg = SHTTP_SLICE(msg);
  shttp_response res = {.code = 200};
  assert_int_equal(SHTTP_STATUS_OK, shttp_compose_code(&smsg, &res));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_string_equal(ans, msg);
}

TEST(shttp_compose_start_line, 1_1_OK) {
  const char ans[] = "HTTP/1.1 200 OK\r\n";
  char msg[sizeof(ans)];
  shttp_mut_slice smsg = {msg, msg + sizeof(msg)};
  shttp_response res = {.version = SHTTP_VERSION_1_1, .code = 200};
  assert_int_equal(SHTTP_STATUS_OK, shttp_compose_start_line(&smsg, &res));
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.begin);
  assert_ptr_equal(msg + sizeof(msg) - 1, smsg.end);
  assert_string_equal(ans, msg);
}

int main(void) {
  const struct CMUnitTest tests[] = {
    ADD(shttp_compose_slice_newline, BASIC),
    ADD(shttp_compose_slice_newline, TOO_SHORT),
    ADD(shttp_compose_slice_cpy, BASIC),
    ADD(shttp_compose_slice_cpy, TOO_SHORT),
    ADD(shttp_compose_version, 1_0),
    ADD(shttp_compose_code, OK),
    ADD(shttp_compose_start_line, 1_1_OK),
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}
