#ifndef CONF_H
#define CONF_H

#define SHTTP_MAX_CONNS 10
#define SHTTP_SOCKET_BACKLOG_SIZE 2

// Attributes:
#define SHTTP_UNUSED_RESULT __attribute__((warn_unused_result))
#define SHTTP_PACKED __attribute__((__packed__))
#define SHTTP_PURE __attribute__((pure))
#define SHTTP_CONST __attribute__((const))

#endif
