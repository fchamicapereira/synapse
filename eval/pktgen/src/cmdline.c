
#include <cmdline.h>
#include <cmdline_parse.h>
#include <cmdline_parse_etheraddr.h>
#include <cmdline_parse_num.h>
#include <cmdline_parse_string.h>
#include <cmdline_socket.h>
#include <rte_atomic.h>
#include <rte_ethdev.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "pktgen.h"

#define CMDLINE_PARSE_INT_NTOKENS(NTOKENS)            \
  struct {                                            \
    void (*f)(void *, struct cmdline *, void *);      \
    void *data;                                       \
    const char *help_str;                             \
    cmdline_parse_token_hdr_t *tokens[(NTOKENS) + 1]; \
  }

struct pktgen_get_params {
  cmdline_fixed_string_t cmd;
};
struct pktgen_int_params {
  cmdline_fixed_string_t cmd;
  uint32_t param;
};
struct pktgen_intint_params {
  cmdline_fixed_string_t cmd;
  uint32_t param1;
  uint32_t param2;
};

/* Parameter-less commands */
cmdline_parse_token_string_t pktgen_quit_token_cmd =
    TOKEN_STRING_INITIALIZER(struct pktgen_get_params, cmd, "quit");
cmdline_parse_token_string_t pktgen_start_token_cmd =
    TOKEN_STRING_INITIALIZER(struct pktgen_get_params, cmd, "start");
cmdline_parse_token_string_t pktgen_stop_token_cmd =
    TOKEN_STRING_INITIALIZER(struct pktgen_get_params, cmd, "stop");
cmdline_parse_token_string_t pktgen_stats_token_cmd =
    TOKEN_STRING_INITIALIZER(struct pktgen_get_params, cmd, "stats");

/* Commands taking just an int */
cmdline_parse_token_string_t pktgen_rate_token_cmd =
    TOKEN_STRING_INITIALIZER(struct pktgen_int_params, cmd, "rate");
cmdline_parse_token_string_t pktgen_churn_token_cmd =
    TOKEN_STRING_INITIALIZER(struct pktgen_int_params, cmd, "churn");
cmdline_parse_token_num_t pktgen_int_token_param =
    TOKEN_NUM_INITIALIZER(struct pktgen_int_params, param, RTE_UINT32);

static inline void signal_new_config() {
  rte_smp_mb();
  rte_atomic64_inc((rte_atomic64_t *)&config.runtime.update_cnt);
}

void pktgen_start() {
  config.runtime.running = true;
  signal_new_config();
}

void pktgen_stop() {
  config.runtime.running = false;
  signal_new_config();
}

void pktgen_rate(rate_gbps_t rate) {
  config.rate = rate;
  config.runtime.rate_per_core = config.rate / config.tx.num_cores;
  signal_new_config();
}

void pktgen_churn(churn_fpm_t churn) {
  uint16_t num_base_flows = config.num_flows / 2;
  config.runtime.churn = churn / 60;

  // Getting the churn per flow.
  config.runtime.flow_ttl =
      (1e9 * (uint64_t)num_base_flows) / config.runtime.churn;

  // We must divide the churn among all the workers.
  config.runtime.flow_ttl *= config.tx.num_cores;

  signal_new_config();
}

static void pktgen_quit_callback(__rte_unused void *ptr_params,
                                 struct cmdline *ctx,
                                 __rte_unused void *ptr_data) {
  cmdline_quit(ctx);
}

static void pktgen_start_callback(__rte_unused void *ptr_params,
                                  __rte_unused struct cmdline *ctx,
                                  __rte_unused void *ptr_data) {
  pktgen_start();
}

static void pktgen_stop_callback(__rte_unused void *ptr_params,
                                 __rte_unused struct cmdline *ctx,
                                 __rte_unused void *ptr_data) {
  pktgen_stop();
}

static void pktgen_stats_callback(__rte_unused void *ptr_params,
                                  __rte_unused struct cmdline *ctx,
                                  __rte_unused void *ptr_data) {
  pktgen_stats_display(config.rx.port);
}

static void pktgen_rate_callback(__rte_unused void *ptr_params,
                                 __rte_unused struct cmdline *ctx,
                                 __rte_unused void *ptr_data) {
  struct pktgen_int_params *params = ptr_params;
  rate_gbps_t rate = (double)params->param / 1000.0;
  pktgen_rate(rate);
}

static void pktgen_churn_callback(__rte_unused void *ptr_params,
                                  __rte_unused struct cmdline *ctx,
                                  __rte_unused void *ptr_data) {
  struct pktgen_int_params *params = ptr_params;
  churn_fpm_t churn = (double)params->param;
  pktgen_churn(churn);
}

CMDLINE_PARSE_INT_NTOKENS(1)
pktgen_quit_cmd = {
    .f = pktgen_quit_callback,
    .data = NULL,
    .help_str = "quit\n     Exit program",
    .tokens = {(void *)&pktgen_quit_token_cmd, NULL},
};

CMDLINE_PARSE_INT_NTOKENS(1)
pktgen_start_cmd = {
    .f = pktgen_start_callback,
    .data = NULL,
    .help_str = "start\n     Start packet generation",
    .tokens = {(void *)&pktgen_start_token_cmd, NULL},
};

CMDLINE_PARSE_INT_NTOKENS(1)
pktgen_stop_cmd = {
    .f = pktgen_stop_callback,
    .data = NULL,
    .help_str = "stop\n     Stop packet generation",
    .tokens = {(void *)&pktgen_stop_token_cmd, NULL},
};

CMDLINE_PARSE_INT_NTOKENS(1)
pktgen_stats_cmd = {
    .f = pktgen_stats_callback,
    .data = NULL,
    .help_str = "stats\n     Show stats",
    .tokens = {(void *)&pktgen_stats_token_cmd, NULL},
};

CMDLINE_PARSE_INT_NTOKENS(2)
pktgen_rate_cmd = {
    .f = pktgen_rate_callback,
    .data = NULL,
    .help_str = "rate <rate>\n     Set rate in Mbps",
    .tokens = {(void *)&pktgen_rate_token_cmd, (void *)&pktgen_int_token_param,
               NULL},
};

CMDLINE_PARSE_INT_NTOKENS(2)
pktgen_churn_cmd = {
    .f = pktgen_churn_callback,
    .data = NULL,
    .help_str = "churn <churn>\n     Set churn in fpm",
    .tokens = {(void *)&pktgen_churn_token_cmd, (void *)&pktgen_int_token_param,
               NULL},
};

cmdline_parse_ctx_t list_prompt_commands[] = {
    (cmdline_parse_inst_t *)&pktgen_quit_cmd,
    (cmdline_parse_inst_t *)&pktgen_start_cmd,
    (cmdline_parse_inst_t *)&pktgen_stop_cmd,
    (cmdline_parse_inst_t *)&pktgen_stats_cmd,
    (cmdline_parse_inst_t *)&pktgen_rate_cmd,
    (cmdline_parse_inst_t *)&pktgen_churn_cmd,
    NULL,
};

void pktgen_cmdline() {
  struct cmdline *ctx_cmdline;

  ctx_cmdline = cmdline_stdin_new(list_prompt_commands, "Pktgen> ");
  cmdline_interact(ctx_cmdline);
  cmdline_stdin_exit(ctx_cmdline);
}
