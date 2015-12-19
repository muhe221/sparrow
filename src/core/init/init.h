#ifndef SPARROW_INIT_H
#define SPARROW_INIT_H

static void create_poll();

static void signal_handler_init();

static void sigchld_handler(int signal);

static void reap_any_outstanding_children();

static int wait_for_one_process();

static void handle_signal();

static void init_parse_config_file(const char *file);

static void start_services();
#endif
