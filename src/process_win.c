#include "process.h"
#include <windows.h>
#include <stdio.h>
#include <string.h>

ProcessHandle create_child_process(const char* executable, const char* arg1,
    const char* arg2, intptr_t stdin_handle,intptr_t stdout_handle) 
{
    ProcessHandle handle;
    handle.valid = -1;
    handle.process_handle = NULL;
    handle.pid = 0;

    char command_line[1024];
    if (arg1 != NULL && arg2 != NULL) {
        snprintf(command_line, sizeof(command_line), "%s %s %s",
                 executable, arg1, arg2);
    } else if (arg1 != NULL) {
        snprintf(command_line, sizeof(command_line), "%s %s",
                 executable, arg1);
    } else {
        snprintf(command_line, sizeof(command_line), "%s", executable);
    }

    STARTUPINFOA si = {0};
    PROCESS_INFORMATION pi = {0};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = (HANDLE)stdin_handle;
    si.hStdOutput = (HANDLE)stdout_handle;
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);

    if (!CreateProcessA(
            executable,
            command_line,
            NULL, NULL,
            TRUE,
            0,
            NULL, NULL,
            &si, &pi)) {
        fprintf(stderr, "Error: CreateProcess failed, code: %lu\n", GetLastError());
        return handle;
    }

    CloseHandle(pi.hThread);

    handle.valid = 1;
    handle.process_handle = pi.hProcess;
    handle.pid = pi.dwProcessId;
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

    if (WaitForSingleObject(handle->process_handle, INFINITE) == WAIT_FAILED) {
        fprintf(stderr, "Error: WaitForSingleObject failed, code: %lu\n", GetLastError());
        return result;
    }

    DWORD exit_code = 0;
    if (!GetExitCodeProcess(handle->process_handle, &exit_code)) {
        fprintf(stderr, "Error: GetExitCodeProcess failed, code: %lu\n", GetLastError());
        return result;
    }


    result.exit_code = (int)exit_code;
    return result;
}

void close_process(ProcessHandle* handle) {
    if (handle->process_handle) {
        CloseHandle(handle->process_handle);
    }
    handle->valid = -1;
    handle->process_handle = NULL;
    handle->pid = 0;
}