#include <stdio.h>
#include <sys/ptrace.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

// define a global variable
char str[] = "pasten";

int main()
{
    pid_t pid;
    int wstatus;

    printf("str address is %p\n", str);

    pid = fork();

    if (pid == -1)
    {
        perror("[parent] error forking the process");
        return errno;
    }
    else if (pid)
    {
        long first_letter;

        waitpid(pid, &wstatus, 0); // wait for the tracee to initiate
        if (!WIFSTOPPED(wstatus))
        {
            perror("[parent] child process didn't stop as intended!\n");
            return 1;
        }
        // set the tracee to stop on exit
        // ptrace(PTRACE_SETOPTIONS, pid, NULL, PTRACE_O_TRACEEXIT);

        // stop child at execution
        errno = 0;
        first_letter = ptrace(PTRACE_PEEKDATA, pid, str, NULL);
        if (first_letter == -1)
        {
            printf("[parent] cannot peek data, errno is %d\n", errno);
            return errno;
        }
        printf("[parent] first letter of child's str is %c\n", (char)first_letter);

        // continue execution of stopped child
        ptrace(PTRACE_CONT, pid, NULL, NULL);

        // wait for child to finish execution, still no terminate
        waitpid(pid, &wstatus, 0);
        if (!WIFSTOPPED(wstatus))
        {
            perror("[parent] child didn't terminate normally!");
            return 2;
        }
        printf("[parent] \"%s\" is in address %p\n", str, str);
        // attempt to peek at data using ptrace
        first_letter = ptrace(PTRACE_PEEKDATA, pid, str, NULL);
        if (first_letter == -1)
        {
            printf("[parent] cannot peek data, errno is %d\n", errno);
            return errno;
        }
        printf("[parent] first letter of child's str is %c\n", (char)first_letter);

        ptrace(PTRACE_CONT, pid, NULL, NULL);
    }
    else
    {
        // child process
        ptrace(PTRACE_TRACEME, 0, 0, 0);
        raise(SIGSTOP);
        // continued execution!
        str[0] = 'A';
        printf("[child] change first letter to \'%c\'!\n", str[0]);
        printf("[child] \"%s\" is in address %p\n", str, str);
        printf("[child] stopping again...\n");
        raise(SIGSTOP);
        printf("[child] child terminated!\n");
    }

    return 0;
}