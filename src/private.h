#ifndef PRIVATE_H
#define PRIVATE_H

#define STRINGIFY(x) #x
#define MIN(x, y) (((x) > (y)) ? (y) : (x))
#define LENGTH(x) (sizeof(x) / sizeof((x)[0]))
#define PURE __attribute__((pure))
#define CONST __attribute__((const))

#endif

