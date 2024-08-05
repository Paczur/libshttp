#ifndef CONN_H
#define CONN_H

#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>

bool conn_accept(int timeout);
bool conn_accept_nblk(void);
bool conn_id_valid(size_t id);
size_t conn_next(char *req, ssize_t *len, size_t max_len, int timeout);
size_t conn_next_nblk(char *req, ssize_t *len, size_t max_len);
void conn_send(const char *res, size_t res_len, size_t id);
int conn_init(void);
void conn_deinit(void);

#endif
