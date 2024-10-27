#ifndef SHTTP_KEEP_ALIVE_TIMEOUT
#define SHTTP_KEEP_ALIVE_TIMEOUT 90
#endif

#ifndef SHTTP_SOCKET_BACKLOG_SIZE
#define SHTTP_SOCKET_BACKLOG_SIZE 2
#endif

#ifndef SHTTP_CONN_CHECKS_PER_SECOND
#define SHTTP_CONN_CHECKS_PER_SECOND 10  // (1, 256)
#endif

#ifndef SHTTP_ASSERT
#define SHTTP_ASSERT(x) assert(x)
#endif

// Attributes:

#ifndef SHTTP_UNUSED_RESULT
#define SHTTP_UNUSED_RESULT __attribute__((warn_unused_result))
#endif

#ifndef SHTTP_PACKED
#define SHTTP_PACKED __attribute__((__packed__))
#endif

#ifndef SHTTP_PURE
#define SHTTP_PURE __attribute__((pure))
#endif

#ifndef SHTTP_CONST
#define SHTTP_CONST __attribute__((const))
#endif
