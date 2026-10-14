#include "file.h"
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

intptr_t open_file_for_write(const char* path) {
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        fprintf(stderr, "Error: open() failed: %s\n", strerror(errno));
        return -1;
    }
    fcntl(fd, F_SETFD, FD_CLOEXEC);
    return (intptr_t)fd;
}

ssize_t file_write(intptr_t handle, const char* data, size_t size) {
    ssize_t result = write((int)handle, data, size);
    if (result == -1) {
        fprintf(stderr, "Error: file_write() failed: %s\n", strerror(errno));
    }
    return result;
}

void close_file_handle(intptr_t handle) {
    if (close((int)handle) == -1) {
        fprintf(stderr, "Error: close_file_handle() failed: %s\n", strerror(errno));
    }
}