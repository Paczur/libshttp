#ifndef SHTTP_CONF_H
#define SHTTP_CONF_H

#define SHTTP_SOCKET_BACKLOG_SIZE 2
#define SHTTP_CONN_CHECKS_PER_SECOND 10  // (1, 256)
#define SHTTP_KEEP_ALIVE_TIMEOUT 5

// Attributes:

#define SHTTP_UNUSED_RESULT __attribute__((warn_unused_result))
#define SHTTP_PACKED __attribute__((__packed__))
#define SHTTP_PURE __attribute__((pure))
#define SHTTP_CONST __attribute__((const))

#endif
