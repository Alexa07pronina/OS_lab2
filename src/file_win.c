#include "file.h"
#include <windows.h>
#include <stdio.h>

intptr_t open_file_for_write(const char* path) {
    SECURITY_ATTRIBUTES sa;
    HANDLE handle;

    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    handle = CreateFileA(
        path,
        GENERIC_WRITE,
        0,
        &sa,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (handle == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Error: CreateFile failed, code: %lu\n", GetLastError());
        return -1;
    }

    return (intptr_t)handle;
}

ssize_t file_write(intptr_t handle, const char* data, size_t size) {
    DWORD written = 0;
    if (!WriteFile((HANDLE)handle, data, (DWORD)size, &written, NULL)) {
        fprintf(stderr, "Error: file_write() failed, code: %lu\n", GetLastError());
        return -1;
    }
    return (ssize_t)written;
}

void close_file_handle(intptr_t handle) {
    if (!CloseHandle((HANDLE)handle)) {
        fprintf(stderr, "Error: close_file_handle() failed, code: %lu\n", GetLastError());
    }
}