#ifndef CONN_H
#define CONN_H

void conn_close(int *fd);
void conn_loop(void);
int conn_init(void);
void conn_deinit(void);

#endif
