#ifndef PIPE_H
#define PIPE_H

#include <stddef.h>
#include <stdint.h>

#ifdef _WIN32
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#else
#include <sys/types.h>
#endif

typedef struct {
    intptr_t read_handle;
    intptr_t write_handle;
} PipeHandles;

PipeHandles create_pipe(void);
ssize_t pipe_write(intptr_t handle, const char* data, size_t size);
ssize_t pipe_read(intptr_t handle, char* buffer, size_t size);
void close_pipe_handle(intptr_t handle);

#endif