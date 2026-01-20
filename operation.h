#pragma once
#include <stdbool.h>

#define IS_QUIT_OP(command_op) (command_op.func_op == quit_op)

typedef struct tracee_state {
    bool start;
    bool is_running;
} state;

void run_op(state *state, int pid, char *cmd);

void continue_op(state *state, int pid, char *cmd);

void next_op(state *state, int pid, char *cmd);

void examine_op(state *state, int pid, char *cmd);

void print_op(state *state, int pid, char *cmd);

void breakpoint_op(state *state, int pid, char *cmd);

void help_op(state *state, int pid, char *cmd);

void quit_op(state *state, int pid, char *cmd);

void list_op(state *state, int pid, char *cmd);
