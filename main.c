#include <stdio.h>
#include <sys/ptrace.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#include <logger.h>
#include "cli.h"
#include "operation.h"

// define a global variable
// char str[] = "pasten";

int main(int argc, char *argv[])
{
    char *binary_path;
    pid_t pid;
    int wstatus;

    logger_initConsoleLogger(stderr);
    logger_setLevel(LogLevel_DEBUG);

    if (argc != 2)
    {
        LOG_ERROR("Usage: %s <binary_name>", argv[0]);
        goto error;
    }
    binary_path = argv[1];
    if (access(binary_path, F_OK) == -1)
    {
        LOG_DEBUG("did not find \"%s\", attempt to run from PATH", binary_path);
    }
    else if (access(binary_path, X_OK) == -1)
    {
        LOG_ERROR("failed to access binary in path %s", binary_path);
        goto error;
    }

    pid = fork();

    if (pid == -1)
    {
        LOG_ERROR("[parent] error forking the process");
        goto error;
    }
    else if (pid) // parent process
    {
        command_op cmd_op;
        state state = {.is_running = false};
        // long first_letter;

        waitpid(pid, &wstatus, 0); // wait for the tracee to initiate
        if (!WIFSTOPPED(wstatus))
        {
            LOG_ERROR("[parent] child process didn't stop as intended!\n");
            goto error;
        }
        ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_TRACEEXEC | PTRACE_O_EXITKILL);
        ptrace(PTRACE_CONT, pid, 0, 0);
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
        do
        {
            cmd_op = read_command();
            LOG_DEBUG("read command \"%s\"", cmd_op.cmdline);
            if (cmd_op.func_op)
            {
                cmd_op.func_op(&state, pid, cmd_op.cmdline);
            }
            if (state.is_running)
            {
                waitpid(pid, &wstatus, 0);
                LOG_DEBUG("waitpid catch status %d", wstatus);
                state.is_running = false;
            }
        } while (!WIFEXITED(wstatus) && !IS_QUIT_OP(cmd_op));
    }
    else // child process
    {
        ptrace(PTRACE_TRACEME, 0, 0, 0);
        raise(SIGSTOP);
        // tracer continue execution!
        // TODO: support run arguments for the binary
        char *const exec_argv[] = {binary_path, NULL};
        execvp(binary_path, exec_argv);
    }

    return 0;

error:
    char *err;
    if (errno)
    {
        err = strerror(errno);
        LOG_ERROR(err);
    }
    return errno;
}