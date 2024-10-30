#include "timer.h"

#include <assert.h>
#include <signal.h>

#define TIME2ITIMERSPEC(spec, time)                                          \
  do {                                                                       \
    spec.it_value.tv_sec = time / 1000;                                      \
    spec.it_value.tv_nsec = (time - value.it_value.tv_sec * 1000) / 1000000; \
  } while(0)
#define ITIMERSPEC2TIME(spec) \
  (spec.it_value.tv_sec * 1000 + spec.it_value.tv_nsec / 1000000)

#define TIME_SEC_MULTIPLIER 1000
#define TIME_NSEC_DIVISOR 1000000

#define timer_valid(i, conn_timers, conn_count) \
  (i < conn_count && conn_timers[i].time != -1)

SHTTP_PURE static shttp_conn_id timer_find_index(shttp_conn_id conn,
                                                 shttp_conn_timer *conn_timers,
                                                 shttp_conn_id conn_count) {
  SHTTP_ASSERT(conn_timers);
  shttp_conn_id i = 0;
  while(i < conn_count && conn_timers[i].time != -1 &&
        conn_timers[i].conn != conn)
    i++;
  return i;
}

static void timer_remove_index(shttp_conn_id i, shttp_conn_timer *conn_timers,
                               shttp_conn_id conn_count) {
  SHTTP_ASSERT(i < conn_count);
  SHTTP_ASSERT(conn_timers);
  while(i < conn_count - 1 && conn_timers[i + 1].time != -1) {
    conn_timers[i] = conn_timers[i + 1];
    i++;
  }
  conn_timers[i].time = -1;
  if(conn_timers[0].time != -1) conn_timers[0].time = 0;
}

static shttp_status timer_add(shttp_conn_id conn, shttp_time timer_time,
                              shttp_conn_timer *conn_timers,
                              shttp_conn_id conn_count) {
  SHTTP_ASSERT(conn_timers);
  shttp_conn_id i = 0;
  shttp_time combined_time = timer_time;
  if(conn_timers[0].time == -1) {
    conn_timers[0].time = 0;
    conn_timers[0].conn = conn;
    return SHTTP_STATUS_OK;
  }
  while(i < conn_count && conn_timers[i].time != -1) {
    combined_time += conn_timers[i].time;
    i++;
  }
  if(i == conn_count) return SHTTP_STATUS_TIMER_LIMIT;
  conn_timers[i].time =
    SHTTP_KEEP_ALIVE_TIMEOUT * TIME_SEC_MULTIPLIER - combined_time;
  conn_timers[i].conn = conn;
  return SHTTP_STATUS_OK;
}

static void settime_spec(timer_t timer, uint32_t sec, uint32_t nsec) {
  struct itimerspec value = {.it_value = {.tv_sec = sec, .tv_nsec = nsec}};
#ifdef NDEBUG
  timer_settime(timer, 0, &value, NULL);
#else
  SHTTP_ASSERT(!timer_settime(timer, 0, &value, NULL));
#endif
}

static void settime_time(timer_t timer, shttp_time time) {
  struct itimerspec value = {
    .it_value = {
      .tv_sec = time / TIME_SEC_MULTIPLIER,
      .tv_nsec = (time - (time / TIME_SEC_MULTIPLIER) * TIME_SEC_MULTIPLIER) *
                 TIME_NSEC_DIVISOR}};
#ifdef NDEBUG
  timer_settime(timer, 0, &value, NULL);
#else
  SHTTP_ASSERT(!timer_settime(timer, 0, &value, NULL));
#endif
}

static shttp_time gettime(timer_t timer) {
  struct itimerspec value;
  timer_gettime(timer, &value);
  return value.it_value.tv_sec * TIME_SEC_MULTIPLIER +
         value.it_value.tv_nsec / TIME_NSEC_DIVISOR;
}

static shttp_time getsettime(timer_t timer, uint32_t sec, uint32_t nsec) {
  struct itimerspec new = {.it_value = {.tv_sec = sec, .tv_nsec = nsec}};
  struct itimerspec old;
#ifdef NDEBUG
  timer_settime(timer, 0, &new, &old);
#else
  SHTTP_ASSERT(!timer_settime(timer, 0, &new, &old));
#endif
  return old.it_value.tv_sec * TIME_SEC_MULTIPLIER +
         old.it_value.tv_nsec / TIME_NSEC_DIVISOR;
}

void shttp_timer_next(timer_t timer, shttp_conn_timer *conn_timers,
                      shttp_conn_id conn_count) {
  SHTTP_ASSERT(conn_timers);
  SHTTP_ASSERT(conn_timers[0].time != -1);
  shttp_time time = conn_timers[1].time;
  timer_remove_index(0, conn_timers, conn_count);
  if(time != -1) settime_time(timer, time);
}

shttp_status shttp_timer_init(timer_t *timer, shttp_conn_timer *conn_timers,
                              shttp_conn_id conn_count) {
  SHTTP_ASSERT(conn_timers);
  if(timer_create(CLOCK_MONOTONIC, NULL, timer))
    return SHTTP_STATUS_TIMER_CREATE;
  for(shttp_conn_id i = 0; i < conn_count; i++) conn_timers[i].time = -1;
  return SHTTP_STATUS_OK;
}

void shttp_timer_deinit(timer_t timer) { timer_delete(timer); }

void shttp_timer_stop(shttp_conn_id conn, timer_t timer,
                      shttp_conn_timer *conn_timers, shttp_conn_id conn_count) {
  SHTTP_ASSERT(conn_timers);
  shttp_time time;
  shttp_conn_id i;
  i = timer_find_index(conn, conn_timers, conn_count);
  if(!timer_valid(i, conn_timers, conn_count)) return;
  if(i == 0) {
    time = getsettime(timer, 0, 0);
    if(conn_timers[1].time != -1) {
      time += conn_timers[1].time;
      timer_remove_index(0, conn_timers, conn_count);
      settime_time(timer, time);
    } else {
      timer_remove_index(0, conn_timers, conn_count);
    }
    return;
  }
  if(timer_valid(i + 1, conn_timers, conn_count))
    conn_timers[i + 1].time += conn_timers[i].time;
  timer_remove_index(i, conn_timers, conn_count);
}

shttp_status shttp_timer_start(shttp_conn_id conn, timer_t timer,
                               shttp_conn_timer *conn_timers,
                               shttp_conn_id conn_count) {
  SHTTP_ASSERT(conn_timers);
  shttp_time time = 0;
  if(conn_timers[0].time != -1) time = gettime(timer);
  SHTTP_PROP(timer_add(conn, time, conn_timers, conn_count));
  if(!time) settime_spec(timer, SHTTP_KEEP_ALIVE_TIMEOUT, 0);
  return SHTTP_STATUS_OK;
}
