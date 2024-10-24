#ifndef SHTTP_TIMER_H
#define SHTTP_TIMER_H

#include "../shttp.h"

void shttp_timer_next(timer_t timer, shttp_conn_timer *conn_timers,
                      shttp_conn_id conn_count);

// returns: TIMER_CREATE
shttp_status shttp_timer_init(timer_t *timer, shttp_conn_timer *conn_timers,
                              shttp_conn_id conn_count);

void shttp_timer_deinit(timer_t timer);

void shttp_timer_stop(shttp_conn_id conn, timer_t timer,
                      shttp_conn_timer *conn_timers, shttp_conn_id conn_count);

// returns: TIMER_LIMIT
shttp_status shttp_timer_start(shttp_conn_id conn, timer_t timer,
                               shttp_conn_timer *conn_timers,
                               shttp_conn_id conn_count);

#endif
