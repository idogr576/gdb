#pragma once

void run_op(int pid, char *cmd);

void continue_op(int pid, char *cmd);

void next_op(int pid, char *cmd);

void examine_op(int pid, char *cmd);

void print_op(int pid, char *cmd);

void breakpoint_op(int pid, char *cmd);
