#include "compose.c"

#include "../test.h"
#include "compose_token.c"

TEST(shttp_compose_token_cpy, normal) {
  char token[] = "token";
  char dest[sizeof(token)];
  assert_int_equal(sizeof(token) - 1,
                   shttp_compose_token_cpy(dest, token, sizeof(token)));
  assert_string_equal(token, dest);
}

TEST(shttp_compose_version, 1_0) {
  char ans[] = "HTTP/1.0";
  shttp_response res = {.version = SHTTP_VERSION_1_0};
  char msg[sizeof(ans)];
  assert_int_equal(sizeof(ans) - 1,
                   shttp_compose_version(msg, sizeof(msg), &res));
  assert_string_equal(ans, msg);
}

TEST(shttp_compose_code, OK) {
  char ans[] = "200 OK";
  char msg[sizeof(ans)];
  shttp_response res = {.code = 200};
  assert_int_equal(sizeof(ans) - 1, shttp_compose_code(msg, sizeof(msg), &res));
  assert_string_equal(ans, msg);
}

TEST(shttp_compose_start_line, 1_1_OK) {
  char ans[] = "HTTP/1.1 200 OK\r\n";
  char msg[sizeof(ans)];
  shttp_response res = {.version = SHTTP_VERSION_1_1, .code = 200};
  assert_int_equal(sizeof(ans) - 1,
                   shttp_compose_start_line(msg, sizeof(msg), &res));
  assert_string_equal(ans, msg);
}

int main(void) {
  const struct CMUnitTest tests[] = {
    ADD(shttp_compose_token_cpy, normal),
    ADD(shttp_compose_version, 1_0),
    ADD(shttp_compose_code, OK),
    ADD(shttp_compose_start_line, 1_1_OK),
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}
