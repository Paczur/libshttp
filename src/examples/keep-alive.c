#include <signal.h>
#include <stdio.h>

#define SHTTP_UNUSED_RESULT  // disables required error handling
#define SHTTP_KEEP_ALIVE_ENABLE
#include "../shttp.h"

#define MSG_BUFF_LENGTH 500
#define MAX_CONNS 10
#define PORT 2137

static char msg_buff[MSG_BUFF_LENGTH];
static shttp_response res;
static shttp_request req;
static shttp_conn conns[MAX_CONNS];
static shttp_conn_timer conn_timers[MAX_CONNS];
static shttp_socket sock = {
  .conns = conns, .conn_timers = conn_timers, .conn_count = MAX_CONNS};
static shttp_mut_slice msg_slice = SHTTP_MUT_SLICE(msg_buff);

static void signal_handler(int sig) {
  (void)sig;
  if(shttp_signal_handler(&sock)) return;
  puts("Connection closed");
}

int main(void) {
  struct sigaction act = {.sa_handler = signal_handler};
  SHTTP_PROP(shttp_init(&sock, PORT));
  sigaction(SIGALRM, &act, NULL);

  while(true) {
    if(shttp_next_ignore(&req, &msg_slice, &sock, 10)) continue;
    shttp_response_to_request(&res, &req);
    shttp_send_ignore(&msg_slice, &res);
    shttp_close(&sock, res.id);
  }

  return shttp_deinit(&sock, true);
}
