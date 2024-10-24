#ifndef SHTTP_PRIVATE_H
#define SHTTP_PRIVATE_H

#include <limits.h>

#include "shttp.h"

#define STRINGIFY(x) #x
#define MIN(x, y) (((x) > (y)) ? (y) : (x))
#define LENGTH(x) (sizeof(x) / sizeof((x)[0]))

#define MAX_VALUE_UNSIGNED(a) \
  (MAX_VALUE_SIGNED(a) | ((a)1 << sizeof(a) * CHAR_BIT - 1))
#define MAX_VALUE_SIGNED(a) (~((a)1 << sizeof(a) * CHAR_BIT - 1))
#define IS_SIGNED(a) (MAX_VALUE_UNSIGNED(a) < 0)
#define MAX_VALUE(a) \
  (IS_SIGNED(a) ? MAX_VALUE_SIGNED(a) : MAX_VALUE_UNSIGNED(a))

#endif

