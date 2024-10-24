#ifndef SHTTP_H
#define SHTTP_H

#include <poll.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#include "conf.h"

#define SHTTP_SLICE(x) \
  (shttp_slice) { (x), (x) + sizeof(x) - 1 }
#define SHTTP_MUT_SLICE(x) \
  (shttp_mut_slice) { (x), (x) + sizeof(x) - 1 }
#define SHTTP_STRINGIFY(x) #x
#define SHTTP_STRINGIFY2(x) SHTTP_STRINGIFY(x)
#define SHTTP_PROP(x)               \
  do {                              \
    const shttp_status _status = x; \
    if(_status) return _status;     \
  } while(0)

typedef uint_least8_t shttp_u8;
typedef uint_least16_t shttp_u16;
typedef uint_least32_t shttp_u32;

typedef int_least8_t shttp_s8;
typedef int_least16_t shttp_s16;
typedef int_least32_t shttp_s32;

typedef enum shttp_status {
  SHTTP_STATUS_OK,
  SHTTP_STATUS_UNKNOWN_ERROR,
  SHTTP_STATUS_SLICE_END,
  SHTTP_STATUS_PREFIX_INVALID,
  SHTTP_STATUS_VALUE_INVALID,
  SHTTP_STATUS_NEWLINE_EXPECTED,
  SHTTP_STATUS_SPACE_EXPECTED,
  SHTTP_STATUS_SOCK_CREATE,
  SHTTP_STATUS_SOCK_OPT,
  SHTTP_STATUS_SOCK_BIND,
  SHTTP_STATUS_SOCK_LISTEN,
  SHTTP_STATUS_SOCK_FD_CLOSE,
  SHTTP_STATUS_CONN_ACCEPT,
  SHTTP_STATUS_CONN_SEND,
  SHTTP_STATUS_CONN_FD_CLOSE,
  SHTTP_STATUS_TIMEOUT,
  SHTTP_STATUS_TIMER_CREATE,
  SHTTP_STATUS_TIMER_LIMIT,
  SHTTP_STATUS_LAST,
} shttp_status;

typedef shttp_u8 shttp_conn_id;

typedef shttp_u16 shttp_reqi;

typedef shttp_u32 shttp_resi;

typedef shttp_s32 shttp_time;

#define SHTTP_X_CODES                \
  X(CONTINUE)                        \
  X(SWITCHING_PROTOCOLS)             \
  X(PROCESSING)                      \
  X(EARLY_HINTS)                     \
  X(OK)                              \
  X(CREATED)                         \
  X(ACCEPTED)                        \
  X(NON_AUTHORITATIVE_INFORMATION)   \
  X(NO_CONTENT)                      \
  X(RESET_CONTENT)                   \
  X(PARTIAL_CONTENT)                 \
  X(MULTI_STATUS)                    \
  X(ALREADY_REPORTED)                \
  X(IM_USED)                         \
  X(MULTIPLE_CHOICES)                \
  X(MOVED_PERMANENTLY)               \
  X(FOUND)                           \
  X(SEE_OTHER)                       \
  X(NOT_MODIFIED)                    \
  X(USE_PROXY)                       \
  X(TEMPORARY_REDIRECT)              \
  X(PERMANENT_REDIRECT)              \
  X(BAD_REQUEST)                     \
  X(UNAUTHORIZED)                    \
  X(PAYMENT_REQUIRED)                \
  X(FORBIDDEN)                       \
  X(NOT_FOUND)                       \
  X(METHOD_NOT_ALLOWED)              \
  X(NOT_ACCEPTABLE)                  \
  X(PROXY_AUTHENTICATION_REQUIRED)   \
  X(REQUEST_TIMEOUT)                 \
  X(CONFLICT)                        \
  X(GONE)                            \
  X(LENGTH_REQUIRED)                 \
  X(PRECONDITION_FAILED)             \
  X(PAYLOAD_TOO_LARGE)               \
  X(URI_TOO_LONG)                    \
  X(UNSUPPORTED_MEDIA_TYPE)          \
  X(RANGE_NOT_SATISFIABLE)           \
  X(EXPECTATION_FAILED)              \
  X(IM_A_TEAPOT)                     \
  X(MISDIRECTED_REQUEST)             \
  X(UNPROCESSABLE_CONTENT)           \
  X(LOCKED)                          \
  X(FAILED_DEPENDENCY)               \
  X(TOO_EARLY)                       \
  X(UPGRADE_REQUIRED)                \
  X(PRECONDITION_REQUIRED)           \
  X(TOO_MANY_REQUESTS)               \
  X(REQUEST_HEADER_FIELDS_TOO_LARGE) \
  X(UNAVAILABLE_FOR_LEGAL_REASONS)   \
  X(INTERNAL_SERVER_ERROR)           \
  X(NOT_IMPLEMENTED)                 \
  X(BAD_GATEWAY)                     \
  X(SERVICE_UNAVAILABLE)             \
  X(GATEWAY_TIMEOUT)                 \
  X(HTTP_VERSION_NOT_SUPPORTED)      \
  X(VARIANT_ALSO_NEGOTIATES)         \
  X(INSUFFICIENT_STORAGE)            \
  X(LOOP_DETECTED)                   \
  X(NOT_EXTENDED)                    \
  X(NETWORK_AUTHENTICATION_REQUIRED)
typedef enum shttp_code {
  SHTTP_CODE_CONTINUE = 100,
  SHTTP_CODE_SWITCHING_PROTOCOLS,
  SHTTP_CODE_PROCESSING,
  SHTTP_CODE_EARLY_HINTS,

  SHTTP_CODE_OK = 200,
  SHTTP_CODE_CREATED,
  SHTTP_CODE_ACCEPTED,
  SHTTP_CODE_NON_AUTHORITATIVE_INFORMATION,
  SHTTP_CODE_NO_CONTENT,
  SHTTP_CODE_RESET_CONTENT,
  SHTTP_CODE_PARTIAL_CONTENT,
  SHTTP_CODE_MULTI_STATUS,
  SHTTP_CODE_ALREADY_REPORTED,
  SHTTP_CODE_IM_USED = 226,

  SHTTP_CODE_MULTIPLE_CHOICES = 300,
  SHTTP_CODE_MOVED_PERMANENTLY,
  SHTTP_CODE_FOUND,
  SHTTP_CODE_SEE_OTHER,
  SHTTP_CODE_NOT_MODIFIED,
  SHTTP_CODE_USE_PROXY,
  SHTTP_CODE_TEMPORARY_REDIRECT,
  SHTTP_CODE_PERMANENT_REDIRECT,

  SHTTP_CODE_BAD_REQUEST = 400,
  SHTTP_CODE_UNAUTHORIZED,
  SHTTP_CODE_PAYMENT_REQUIRED,
  SHTTP_CODE_FORBIDDEN,
  SHTTP_CODE_NOT_FOUND,
  SHTTP_CODE_METHOD_NOT_ALLOWED,
  SHTTP_CODE_NOT_ACCEPTABLE,
  SHTTP_CODE_PROXY_AUTHENTICATION_REQUIRED,
  SHTTP_CODE_REQUEST_TIMEOUT,
  SHTTP_CODE_CONFLICT,
  SHTTP_CODE_GONE,
  SHTTP_CODE_LENGTH_REQUIRED,
  SHTTP_CODE_PRECONDITION_FAILED,
  SHTTP_CODE_PAYLOAD_TOO_LARGE,
  SHTTP_CODE_URI_TOO_LONG,
  SHTTP_CODE_UNSUPPORTED_MEDIA_TYPE,
  SHTTP_CODE_RANGE_NOT_SATISFIABLE,
  SHTTP_CODE_EXPECTATION_FAILED,
  SHTTP_CODE_IM_A_TEAPOT,
  SHTTP_CODE_MISDIRECTED_REQUEST = 421,
  SHTTP_CODE_UNPROCESSABLE_CONTENT,
  SHTTP_CODE_LOCKED,
  SHTTP_CODE_FAILED_DEPENDENCY,
  SHTTP_CODE_TOO_EARLY,
  SHTTP_CODE_UPGRADE_REQUIRED,
  SHTTP_CODE_PRECONDITION_REQUIRED = 428,
  SHTTP_CODE_TOO_MANY_REQUESTS,
  SHTTP_CODE_REQUEST_HEADER_FIELDS_TOO_LARGE = 431,
  SHTTP_CODE_UNAVAILABLE_FOR_LEGAL_REASONS = 451,

  SHTTP_CODE_INTERNAL_SERVER_ERROR = 500,
  SHTTP_CODE_NOT_IMPLEMENTED,
  SHTTP_CODE_BAD_GATEWAY,
  SHTTP_CODE_SERVICE_UNAVAILABLE,
  SHTTP_CODE_GATEWAY_TIMEOUT,
  SHTTP_CODE_HTTP_VERSION_NOT_SUPPORTED,
  SHTTP_CODE_VARIANT_ALSO_NEGOTIATES,
  SHTTP_CODE_INSUFFICIENT_STORAGE,
  SHTTP_CODE_LOOP_DETECTED,
  SHTTP_CODE_NOT_EXTENDED,
  SHTTP_CODE_NETWORK_AUTHENTICATION_REQUIRED
} SHTTP_PACKED shttp_code;

#define SHTTP_X_METHODS \
  X(GET)                \
  X(POST)               \
  X(HEAD)               \
  X(PUT)                \
  X(DELETE)             \
  X(CONNECT)            \
  X(OPTIONS)            \
  X(TRACE)              \
  X(PATCH)
typedef enum shttp_method {
#define X(x) SHTTP_METHOD_##x,
  SHTTP_X_METHODS
#undef X
    SHTTP_METHOD_LENGTH
} SHTTP_PACKED shttp_method;

typedef enum shttp_version {
  SHTTP_VERSION_1_0,
  SHTTP_VERSION_1_1,
  SHTTP_VERSION_2_0,
  SHTTP_VERSION_3_0,
  SHTTP_VERSION_LENGTH
} SHTTP_PACKED shttp_version;

typedef struct shttp_slice {
  const char *begin;
  const char *end;
} SHTTP_PACKED shttp_slice;

typedef struct shttp_mut_slice {
  char *begin;
  char *end;
} SHTTP_PACKED shttp_mut_slice;

typedef struct pollfd shttp_conn;

typedef struct shttp_conn_timer {
  shttp_time time;
  shttp_conn_id conn;
} shttp_conn_timer;

typedef struct shttp_socket {
  int sock;
  timer_t timer;
  shttp_conn *conns;
  shttp_conn_timer *conn_timers;
  shttp_conn_id conn_count;
} shttp_socket;

typedef struct shttp_request {
  shttp_version version;
  shttp_method method;
  shttp_conn_id id;
  shttp_socket *sock;
  shttp_slice path;
  shttp_slice headers;
} SHTTP_PACKED shttp_request;

typedef struct shttp_response {
  shttp_version version;
  shttp_code code;
  shttp_conn_id id;
  shttp_socket *sock;
  shttp_slice headers;
  shttp_slice body;
} SHTTP_PACKED shttp_response;

/* Currently only used for timer management
 * returns: TIMER_SET, CONN_FD_CLOSE
 */
SHTTP_UNUSED_RESULT shttp_status
shttp_signal_handler(shttp_socket sock[static 1]);

void shttp_response_to_request(shttp_response res[static 1],
                               const shttp_request req[static 1]);

/* returns: TIMEOUT, SLICE_END, PREFIX_INVALID, VALUE_INVALID, NEWLINE_EXPECTED,
 * SPACE_EXPECTED
 */
SHTTP_UNUSED_RESULT shttp_status shttp_next(shttp_request req[static 1],
                                            shttp_mut_slice buff[static 1],
                                            shttp_u16 timeout);

/* returns: TIMEOUT, SLICE_END, PREFIX_INVALID, VALUE_INVALID, NEWLINE_EXPECTED,
 * SPACE_EXPECTED
 */
SHTTP_UNUSED_RESULT shttp_status
shttp_next_nblk(shttp_request req[static 1], shttp_mut_slice buff[static 1]);

// returns: CONN_SEND, TIMEOUT, CONN_ACCEPT, VALUE_INVALID, SLICE_END
SHTTP_UNUSED_RESULT shttp_status shttp_send(shttp_mut_slice buff[static 1],
                                            const shttp_response res[static 1]);

// returns: CONN_FD_CLOSE
SHTTP_UNUSED_RESULT shttp_status shttp_close(shttp_socket sock[static 1],
                                             shttp_conn_id id);

// returns: SOCK_CREATE, SOCK_BIND, SOCK_LISTEN
SHTTP_UNUSED_RESULT shttp_status shttp_init(shttp_socket sock[static 1],
                                            shttp_u16 port);

// returns: CONN_FD_CLOSE, SOCK_FD_CLOSE
SHTTP_UNUSED_RESULT shttp_status shttp_deinit(shttp_socket sock[static 1],
                                              bool force);

// returns: SLICE_END, NEWLINE_EXPECTED
SHTTP_UNUSED_RESULT shttp_status
shttp_parse_slice_newline(shttp_slice s[static 1]);

// returns: SLICE_END, SPACE_EXPECTED
SHTTP_UNUSED_RESULT shttp_status
shttp_parse_slice_space(shttp_slice s[static 1]);

// returns: SLICE_END
SHTTP_UNUSED_RESULT shttp_status
shttp_parse_slice_space_optional(shttp_slice s[static 1]);

// returns: SLICE_END, VALUE_INVALID
SHTTP_UNUSED_RESULT shttp_status
shttp_parse_slice_number(shttp_u16 out[static 1], shttp_slice s[static 1]);

// returns: SLICE_END
SHTTP_UNUSED_RESULT shttp_status shttp_parse_slice_skip(shttp_slice s[static 1],
                                                        shttp_u16 x);

// returns: SLICE_END
SHTTP_UNUSED_RESULT shttp_status
shttp_parse_slice_skip_until(shttp_slice s[static 1], char mark);

// returns: SLICE_END
SHTTP_UNUSED_RESULT shttp_status
shttp_parse_slice_skip_until_or(shttp_slice s[static 1], char marks[static 3]);

// returns: NEWLINE_EXPECTED
SHTTP_UNUSED_RESULT shttp_status
shttp_parse_slice_skip_until_newline(shttp_slice s[static 1]);

// returns: SPACE_EXPECTED
SHTTP_UNUSED_RESULT shttp_status
shttp_parse_slice_skip_until_space(shttp_slice s[static 1]);

// returns: SLICE_END
SHTTP_UNUSED_RESULT shttp_status
shttp_parse_slice_skip_past(shttp_slice s[static 1], char mark);

// returns: SLICE_END
SHTTP_UNUSED_RESULT shttp_status
shttp_parse_slice_skip_past_or(shttp_slice s[static 1], char marks[static 3]);

// returns: NEWLINE_EXPECTED
SHTTP_UNUSED_RESULT shttp_status
shttp_parse_slice_skip_past_newline(shttp_slice s[static 1]);

// returns: SPACE_EXPECTED
SHTTP_UNUSED_RESULT shttp_status
shttp_parse_slice_skip_past_space(shttp_slice s[static 1]);

// Slices cannot point to overlapping memory
SHTTP_PURE SHTTP_UNUSED_RESULT bool shttp_parse_slice_cmp(shttp_slice s1,
                                                          shttp_slice s2);

// Slices cannot point to overlapping memory
SHTTP_PURE SHTTP_UNUSED_RESULT bool shttp_parse_slice_cmp_until(shttp_slice s1,
                                                                shttp_slice s2,
                                                                char mark);

// returns: SLICE_END NEWLINE_EXPECTED
SHTTP_UNUSED_RESULT shttp_status
shttp_parse_header_next(shttp_slice out[static 1], shttp_slice s[static 1]);

// returns: SLICE_END
SHTTP_UNUSED_RESULT shttp_status
shttp_parse_header_key(shttp_slice out[static 1], shttp_slice s[static 1]);

// returns: SLICE_END
SHTTP_UNUSED_RESULT shttp_status
shttp_parse_header_value(shttp_slice out[static 1], shttp_slice s[static 1]);

/* Fills request and makes "s" point to request body
 * returns: SLICE_END, PREFIX_INVALID, VALUE_INVALID, NEWLINE_EXPECTED,
 * SPACE_EXPECTED
 */
SHTTP_UNUSED_RESULT shttp_status
shttp_parse_request(shttp_request req[static 1], shttp_slice s[static 1]);

// returns: SLICE_END
SHTTP_UNUSED_RESULT shttp_status
shttp_compose_slice_newline(shttp_mut_slice msg[static 1]);

// returns: SLICE_END
SHTTP_UNUSED_RESULT shttp_status
shttp_compose_slice_cpy(shttp_mut_slice msg[static 1], shttp_slice slice);

// returns: SLICE_END
SHTTP_UNUSED_RESULT shttp_status
shttp_compose_slice_cpy_char(shttp_mut_slice msg[static 1], char c);

// returns: VALUE_INVALID, SLICE_END
SHTTP_UNUSED_RESULT shttp_status shttp_compose_response(
  shttp_mut_slice msg[static 1], const shttp_response res[static 1]);

#endif
