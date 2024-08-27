#ifndef CONF_H
#define CONF_H

#define SHTTP_REQUEST_LENGTH 500
#define SHTTP_RESPONSE_LENGTH 500
#define SHTTP_MAX_CONNS 10
#define SHTTP_PORT 1337
#define SHTTP_SOCKET_BACKLOG_SIZE 2
#define SHTTP_VALUES_PER_HEADER 10
#define SHTTP_STRING_VALUE_LENGTH 22
#define SHTTP_DOMAIN_LENGTH 50
#define SHTTP_PATH_LENGTH 128
#define SHTTP_PACKED __attribute__((__packed__))
#define SHTTP_PURE __attribute__((pure))
#define SHTTP_CONST __attribute__((const))

#endif
