#ifndef SHTTP_TYPES_H
#define SHTTP_TYPES_H

#include <stdbool.h>
#include <stdint.h>

#include "conf.h"

typedef uint_least8_t shttp_u8;
typedef uint_least16_t shttp_u16;
typedef uint_least32_t shttp_u32;
typedef uint_least64_t shttp_u64;

typedef int_least8_t shttp_s8;
typedef int_least16_t shttp_s16;
typedef int_least32_t shttp_s32;
typedef int_least64_t shttp_s64;

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

#define SHTTP_X_REQUEST_HEADERS_VALUE X(user_agent, "User-Agent: ")

#define SHTTP_X_REQUEST_HEADERS_VALUES X(connection, "Connection: ")

#define SHTTP_X_REQUEST_HEADERS_VALUES_WEIGHTED \
  X(a_im, "A-IM: ")                             \
  X(accept, "Accept: ")                         \
  X(accept_charset, "Accept-Charset: ")         \
  X(accept_encoding, "Accept-Encoding: ")       \
  X(accept_language, "Accept-Language: ")

typedef enum shttp_weekday {
  SHTTP_WEEKDAY_MON,
  SHTTP_WEEKDAY_TUE,
  SHTTP_WEEKDAY_WED,
  SHTTP_WEEKDAY_THU,
  SHTTP_WEEKDAY_FRI,
  SHTTP_WEEKDAY_SAT,
  SHTTP_WEEKDAY_SUN
} shttp_weekday;

typedef enum shttp_month {
  SHTTP_MONTH_JAN,
  SHTTP_MONTH_FEB,
  SHTTP_MONTH_MAR,
  SHTTP_MONTH_APR,
  SHTTP_MONTH_MAY,
  SHTTP_MONTH_JUN,
  SHTTP_MONTH_JUL,
  SHTTP_MONTH_AUG,
  SHTTP_MONTH_SEP,
  SHTTP_MONTH_OCT,
  SHTTP_MONTH_NOV,
  SHTTP_MONTH_DEC
} shttp_month;

typedef enum shttp_timezone {
  SHTTP_TIMEZONE_UT = 0,
  SHTTP_TIMEZONE_GMT = 0,
  SHTTP_TIMEZONE_EST,
  SHTTP_TIMEZONE_EDT,
  SHTTP_TIMEZONE_CST,
  SHTTP_TIMEZONE_CDT,
  SHTTP_TIMEZONE_MST,
  SHTTP_TIMEZONE_MDT,
  SHTTP_TIMEZONE_PST,
  SHTTP_TIMEZONE_PDT,
  SHTTP_TIMEZONE_Z = 0,
  SHTTP_TIMEZONE_A,
  SHTTP_TIMEZONE_M,
  SHTTP_TIMEZONE_N,
  SHTTP_TIMEZONE_Y,
} shttp_timezone;

typedef struct shttp_datetime {
  shttp_u8 timezone_offset_min : 2;  // * 15
  shttp_weekday weekday : 3;
  shttp_month month : 4;
  shttp_timezone timezone : 4;
  shttp_u8 day : 5;
  shttp_u8 hours : 5;
  shttp_s8 timezone_offset_hour : 6;
  shttp_u8 minutes : 6;
  shttp_u8 seconds : 6;
  shttp_u16 year : 12;
} PACK_STRUCTS shttp_datetime;

typedef shttp_u8 shttp_conn_id;

typedef enum shttp_method {
#define X(x) SHTTP_METHOD_##x,
  SHTTP_X_METHODS
#undef X
    SHTTP_METHOD_LENGTH
} shttp_method;

typedef enum shttp_version {
  SHTTP_VERSION_1_0,
  SHTTP_VERSION_1_1,
  SHTTP_VERSION_2_0,
  SHTTP_VERSION_3_0,
  SHTTP_VERSION_LENGTH
} shttp_version;

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
} shttp_code;

typedef enum shttp_weight {
  SHTTP_WEIGHT_1_0,
  SHTTP_WEIGHT_000,
  SHTTP_WEIGHT_999 = SHTTP_WEIGHT_000 + 999
} shttp_weight;

typedef struct shttp_value_weighted {
  char value[STRING_VALUE_LENGTH];
  shttp_u16 weight : 10;
} PACK_STRUCTS shttp_value_weighted;

typedef struct shttp_value {
  char value[STRING_VALUE_LENGTH];
} PACK_STRUCTS shttp_value;

typedef struct shttp_request {
  shttp_version version : 3;
  shttp_method method : 4;
  shttp_conn_id id;  // 16
  shttp_u16 host_port;
  char host_domain[DOMAIN_LENGTH];
#define X(name, token) shttp_value name;
  SHTTP_X_REQUEST_HEADERS_VALUE
#undef X
#define X(name, token) shttp_value name[VALUES_PER_HEADER];
  SHTTP_X_REQUEST_HEADERS_VALUES
#undef X
#define X(name, token) shttp_value_weighted name[VALUES_PER_HEADER];
  SHTTP_X_REQUEST_HEADERS_VALUES_WEIGHTED
#undef X
  char path[PATH_LENGTH];
} PACK_STRUCTS shttp_request;

typedef struct shttp_response {
  shttp_conn_id id;
  shttp_version version : 3;
  shttp_code code : 9;
} PACK_STRUCTS shttp_response;

#endif
