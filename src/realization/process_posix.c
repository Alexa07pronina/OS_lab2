#include "process.h"
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

ProcessHandle create_child_process(const char* executable, const char* arg1,
    const char* arg2, intptr_t stdin_handle, intptr_t stdout_handle) 
{
    ProcessHandle handle;
    handle.valid = -1;
    handle.pid = -1;

    pid_t pid = fork();
    if (pid == -1) {
        fprintf(stderr, "Error: fork() failed: %s\n", strerror(errno));
        return handle;
    }

    if (pid == 0) {
        if (stdin_handle != -1) {
            if (dup2((int)stdin_handle, STDIN_FILENO) == -1) {
                fprintf(stderr, "Error: dup2() stdin failed: %s\n", strerror(errno));
                _exit(1);
            }
        }
        if (stdout_handle != -1) {
            if (dup2((int)stdout_handle, STDOUT_FILENO) == -1) {
                fprintf(stderr, "Error: dup2() stdout failed: %s\n", strerror(errno));
                _exit(1);
            }
        }

        char* argv[4];
        argv[0] = (char*)executable;
        argv[1] = (char*)arg1;
        argv[2] = (char*)arg2;
        argv[3] = NULL;

        execvp(executable, argv);
        fprintf(stderr, "Error: execvp() failed: %s\n", strerror(errno));
        _exit(1);
    }

    handle.valid = 1;
    handle.pid = pid;
    return handle;
}

ProcessResult wait_for_process(ProcessHandle* handle) {
    ProcessResult result;
    result.exit_code = -1;
    result.pid = (intptr_t)handle->pid;

    if (handle->valid!=1) {
        fprintf(stderr, "Error: Invalid process handle\n");
        return result;
    }

    int status = 0;
    if (waitpid(handle->pid, &status, 0) == -1) {
        fprintf(stderr, "Error: waitpid() failed: %s\n", strerror(errno));
        return result;
    }

    if (WIFEXITED(status)) {
        result.exit_code = WEXITSTATUS(status);
    }
    return result;
}

void close_process(ProcessHandle* handle) {
    handle->valid = -1;
    handle->pid = -1;
}