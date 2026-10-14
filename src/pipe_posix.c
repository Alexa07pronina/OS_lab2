#include "pipe.h"
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

PipeHandles create_pipe() {
    PipeHandles handles;
    int fds[2];

    if (pipe(fds) == -1) {
        fprintf(stderr, "Error: pipe() failed: %s\n", strerror(errno));
        handles.read_handle = -1;
        handles.write_handle = -1;
        return handles;
    }

    fcntl(fds[0], F_SETFD, FD_CLOEXEC);
    fcntl(fds[1], F_SETFD, FD_CLOEXEC);

    handles.read_handle = (intptr_t)fds[0];
    handles.write_handle = (intptr_t)fds[1];
    return handles;
}

ssize_t pipe_write(intptr_t handle, const char* data, size_t size) {
    ssize_t result = write((int)handle, data, size);
    if (result == -1) {
        fprintf(stderr, "Error: pipe_write() failed: %s\n", strerror(errno));
    }
    return result;
}

ssize_t pipe_read(intptr_t handle, char* buffer, size_t size) {
    ssize_t result = read((int)handle, buffer, size);
    if (result == -1) {
        fprintf(stderr, "Error: pipe_read() failed: %s\n", strerror(errno));
    }
    return result;
}

void close_pipe_handle(intptr_t handle) {
    if (close((int)handle) == -1) {
        fprintf(stderr, "Error: close_pipe_handle() failed: %s\n", strerror(errno));
    }
}