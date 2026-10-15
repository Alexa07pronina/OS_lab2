#include "pipe.h"
#include <windows.h>
#include <stdio.h>

PipeHandles create_pipe(void) {
    PipeHandles handles;
    SECURITY_ATTRIBUTES sa;
    HANDLE read_handle, write_handle;

    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    if (!CreatePipe(&read_handle, &write_handle, &sa, 0)) {
        fprintf(stderr, "Error: CreatePipe failed, code: %lu\n", GetLastError());
        handles.read_handle = -1;
        handles.write_handle = -1;
        return handles;
    }

    SetHandleInformation(write_handle, HANDLE_FLAG_INHERIT, 0);

    handles.read_handle = (intptr_t)read_handle;
    handles.write_handle = (intptr_t)write_handle;
    return handles;
}

ssize_t pipe_write(intptr_t handle, const char* data, size_t size) {
    DWORD written = 0;
    if (!WriteFile((HANDLE)handle, data, (DWORD)size, &written, NULL)) {
        fprintf(stderr, "Error: pipe_write() failed, code: %lu\n", GetLastError());
        return -1;
    }
    return (ssize_t)written;
}

ssize_t pipe_read(intptr_t handle, char* buffer, size_t size) {
    DWORD read_bytes = 0;
    if (!ReadFile((HANDLE)handle, buffer, (DWORD)size, &read_bytes, NULL)) {
        fprintf(stderr, "Error: pipe_read() failed, code: %lu\n", GetLastError());
        return -1;
    }
    return (ssize_t)read_bytes;
}

void close_pipe_handle(intptr_t handle) {
    if (handle != -1 && handle != (intptr_t)INVALID_HANDLE_VALUE) {
        if (!CloseHandle((HANDLE)handle)) {
            fprintf(stderr, "Error: close_pipe_handle() failed, code: %lu\n", GetLastError());
        }
    }
}