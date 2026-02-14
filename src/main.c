#include <stdio.h>
#include <sys/ptrace.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "logger.h"

#include "cli.h"
#include "operation.h"
#include "elf/symbols.h"
#include "utils/path_utils.h"
#include "tracee.h"
#include "arch/x86_64.h"
#include "breakpoint.h"
#include "print.h"

// #define STB_DS_IMPLEMENTATION
// #include "stb_ds.h"

// define a global variable
// char str[] = "pasten";

int main(int argc, char *argv[])
{
    char binary_path[PATH_MAX_LEN] = {0};
    pid_t _pid;
    int wstatus;

    logger_initConsoleLogger(stderr);
    // logger_setLevel(LogLevel_DEBUG);
    logger_setLevel(LogLevel_ERROR);

    if (argc < 2)
    {
        printf("Usage: %s <binary path> <args>\n", argv[0]);
        goto error;
    }
    strcpy(binary_path, argv[1]);

    if (!binary_path_exists(binary_path))
    {
        goto error;
    }

    _pid = fork();

    if (_pid == -1)
    {
        LOG_ERROR("[parent] error forking the process");
        goto error;
    }
    else if (_pid) // parent process
    {
        state _state = {.start = false, .is_running = false};
        tracee tracee = {.pid = _pid, .state = _state};

        waitpid(tracee.pid, &wstatus, 0); // wait for the tracee to initiate
        if (!WIFSTOPPED(wstatus))
        {
            LOG_ERROR("[parent] child process didn't stop as intended!\n");
            goto error;
        }
        ptrace(PTRACE_SETOPTIONS, tracee.pid, 0, PTRACE_O_TRACEEXEC | PTRACE_O_EXITKILL);
        // load the symbol table before getting commands
        symtab_elf_load(binary_path, &tracee.symtab);
        // initialize breakpoints hashmap
        breakpoint_init(&tracee);

        LOG_DEBUG("symtab: size = %d", tracee.symtab.size);
        ptrace(PTRACE_CONT, tracee.pid, 0, 0);
        /*
        // set the tracee to stop on exit
        ptrace(PTRACE_SETOPTIONS, pid, NULL, PTRACE_O_TRACEEXIT);

        // stop child at execution
        errno = 0;
        first_letter = ptrace(PTRACE_PEEKDATA, pid, str, NULL);
        if (first_letter == -1)
        {
            LOG_ERROR("[parent] cannot peek data, errno is %d\n", errno);
            return errno;
        }
        LOG_DEBUG("[parent] first letter of child's str is %c\n", (char)first_letter);

        // continue execution of stopped child
        ptrace(PTRACE_CONT, pid, NULL, NULL);

        // wait for child to finish execution, still no terminate
        waitpid(pid, &wstatus, 0);
        if (!WIFSTOPPED(wstatus))
        {
            LOG_ERROR("[parent] child didn't terminate normally!");
            return 2;
        }
        LOG_DEBUG("[parent] \"%s\" is in address %p\n", str, str);
        // attempt to peek at data using ptrace
        first_letter = ptrace(PTRACE_PEEKDATA, pid, str, NULL);
        if (first_letter == -1)
        {
            LOG_ERROR("[parent] cannot peek data, errno is %d\n", errno);
            return errno;
        }
        LOG_DEBUG("[parent] first letter of child's str is %c\n", (char)first_letter);
        */

        /*
        TODO: insert a cli client here and use PTRACE_CONT only on "run" command
        */

        // start the main loop
        char opcode[OPCODE_MAX_REPR] = {0};
        command_op cmd_op, last_cmd_op;
        cli_init();
        do
        {
            if (tracee.state.start)
            {
                get_current_opcode(&tracee, opcode);
                printf("\n%s\n", opcode);
            }
            cmd_op = read_command(">>");
            if (!strlen(cmd_op.cmdline) && tracee.state.start)
            {
                // run last command again
                cmd_op = last_cmd_op;
            }
            if (cmd_op.func_op)
            {
                cmd_op.func_op(&tracee, cmd_op.cmdline);
            }
            if (tracee.state.is_running)
            {
                waitpid(tracee.pid, &wstatus, 0);
                LOG_DEBUG("waitpid catch status %d", wstatus);
                tracee.state.is_running = !WIFSTOPPED(wstatus);
            }
            last_cmd_op = cmd_op;
        } while (!WIFEXITED(wstatus) && !IS_QUIT_OP(cmd_op));
    }
    else // child process
    {
        ptrace(PTRACE_TRACEME, 0, 0, 0);
        raise(SIGSTOP);
        // tracer continue execution!
        // TODO: support run arguments for the binary
        char **exec_argv = (char **)malloc(argc * sizeof(char *));
        if (!exec_argv)
        {
            goto error;
        }
        exec_argv[0] = binary_path;
        exec_argv[argc - 1] = NULL;
        for (int i = 1; i < argc - 1; i++)
        {
            exec_argv[i] = argv[i + 1];
        }
        execv(binary_path, exec_argv);
    }

    return 0;

error:
    if (errno)
    {
        char *err = strerror(errno);
        LOG_ERROR(err);
    }
    return errno;
}