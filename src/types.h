#ifndef SHTTP_TYPES_H
#define SHTTP_TYPES_H

#include <stdbool.h>
#include <stdint.h>

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

typedef struct shttp_request {
  bool has_host : 1;

  shttp_version version : 3;
  shttp_method method : 4;
  shttp_conn_id id;  // 16

  shttp_u16 host_port;
  char host_domain[50];

  char path[128];
} __attribute__((packed)) shttp_request;

typedef struct shttp_response {
  shttp_conn_id id;
} shttp_response;

#endif
