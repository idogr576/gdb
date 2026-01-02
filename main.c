#include <stdio.h>
#include <sys/ptrace.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#include <logger.h>

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
        LOG_INFO("did not find \"%s\", attempt to run from PATH", binary_path);
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
        // long first_letter;

        waitpid(pid, &wstatus, 0); // wait for the tracee to initiate
        if (!WIFSTOPPED(wstatus))
        {
            LOG_ERROR("[parent] child process didn't stop as intended!\n");
            goto error;
        }
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
        LOG_DEBUG("continue execution of child");
        ptrace(PTRACE_CONT, pid, NULL, NULL);
    }
    else // child process
    {
        ptrace(PTRACE_TRACEME, 0, 0, 0);
        raise(SIGSTOP);
        // tracer continue execution!
        char *const exec_argv[] = {binary_path, NULL};
        execvp(binary_path, exec_argv);
    }

    return 0;

error:
    if (errno)
    {
        perror("");
    }
    return errno;
}