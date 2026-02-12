#include <sys/ptrace.h>
#include <errno.h>
#include <logger.h>

#include "data_utils.h"

/*
read size bytes from tracee memory at address addr.
return the number of bytes read. on success, the return value will be equal to size.
*/
size_t read_tracee_mem(tracee *tracee, long *addr, int *data, size_t n)
{
    int retval;
    size_t read_size = 0;
    for (size_t i = 0; i < n; i++)
    {
        errno = 0;
        retval = (int)ptrace(PTRACE_PEEKDATA, tracee->pid, addr + i, 0);
        if (!errno)
        {
            data[i] = retval;
            read_size += sizeof(retval);
            continue;
        }
        LOG_ERROR("error in reading tracee's mem: %s", strerror(errno));
        break;
    }
    return read_size;
}