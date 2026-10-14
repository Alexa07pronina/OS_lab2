#ifndef PROCESS_H
#define PROCESS_H

#include <stddef.h>
#include <stdint.h>

#ifdef _WIN32
#include <windows.h>
typedef long ssize_t;
#else
#include <sys/types.h>
#endif

typedef struct {
    int exit_code;
    intptr_t pid;
} ProcessResult;

typedef struct {
    int valid;
#ifdef _WIN32
    HANDLE process_handle;
    DWORD pid;
#else
    pid_t pid;
#endif
} ProcessHandle;

ProcessHandle create_child_process(
    const char* executable,
    const char* arg1,
    const char* arg2,
    intptr_t stdin_handle,
    intptr_t stdout_handle
);

ProcessResult wait_for_process(ProcessHandle* handle);
void close_process(ProcessHandle* handle);

#endif