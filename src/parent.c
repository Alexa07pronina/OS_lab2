#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "process.h"
#include "pipe.h"
#include "file.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#define MAX_LINE 4096

static int parent_main(int argc, char* argv[]) {
#ifdef _WIN32
    printf("[Parent] PID: %lu\n", GetCurrentProcessId());
#else
    printf("[Parent] PID: %d\n", getpid());
#endif
    fflush(stdout);

    char file1[MAX_LINE], file2[MAX_LINE];

    printf("Enter output file for Child1: ");
    fflush(stdout);
    if (fgets(file1, sizeof(file1), stdin) == NULL || file1[0] == '\n' || file1[0] == '\0') {
        fprintf(stderr, "Error: empty filename for Child1\n");
        return 1;
    }
    file1[strcspn(file1, "\n")] = '\0';

    printf("Enter output file for Child2: ");
    fflush(stdout);
    if (fgets(file2, sizeof(file2), stdin) == NULL || file2[0] == '\n' || file2[0] == '\0') {
        fprintf(stderr, "Error: empty filename for Child2\n");
        return 1;
    }
    file2[strcspn(file2, "\n")] = '\0';

    PipeHandles pipe1 = create_pipe();
    if (pipe1.read_handle == -1 || pipe1.write_handle == -1) {
        fprintf(stderr, "Error: failed to create pipe1\n");
        return 1;
    }

    PipeHandles pipe2 = create_pipe();
    if (pipe2.read_handle == -1 || pipe2.write_handle == -1) {
        fprintf(stderr, "Error: failed to create pipe2\n");
        close_pipe_handle(pipe1.read_handle);
        close_pipe_handle(pipe1.write_handle);
        return 1;
    }

    intptr_t file1_handle = open_file_for_write(file1);
    intptr_t file2_handle = -1;
    if (file1_handle == -1) {
        fprintf(stderr, "Error: failed to open file1\n");
    }
    else file2_handle = open_file_for_write(file2);
    if (file2_handle == -1) {
        fprintf(stderr, "Error: failed to open file2\n");
    }

    if (file1_handle == -1 || file2_handle == -1)
    {
        close_file_handle(file1_handle);
        close_file_handle(file2_handle);
        close_pipe_handle(pipe1.read_handle);
        close_pipe_handle(pipe1.write_handle);
        close_pipe_handle(pipe2.read_handle);
        close_pipe_handle(pipe2.write_handle);
        return 1;
    }

#ifdef _WIN32
    const char* executable = "child.exe"; 
#else
    const char* executable = "./child";
#endif


    ProcessHandle child1 = {-1, -1};
    ProcessHandle child2 = {-1, -1};

    child1 = create_child_process(executable, "child", "1", pipe1.read_handle, file1_handle);
    if (child1.valid!=1) {
        fprintf(stderr, "Error: failed to create Child1\n");
    }
    else {
        child2 = create_child_process(executable, "child", "2", pipe2.read_handle, file2_handle);
        if (child2.valid!=1) {
            fprintf(stderr, "Error: failed to create Child2\n");
        }
    }

    if (child1.valid!=1 || child2.valid!=1) {
        if (child1.valid==1) {
            close_pipe_handle(pipe1.write_handle);
            wait_for_process(&child1);
        }
                
        close_file_handle(file1_handle);
        close_file_handle(file2_handle);
        close_pipe_handle(pipe1.read_handle);
        close_pipe_handle(pipe1.write_handle);
        close_pipe_handle(pipe2.read_handle);
        close_pipe_handle(pipe2.write_handle);
        close_process(&child1);
        close_process(&child2);
        return 1;
    }

    printf("[Parent] Child1 created with PID: %ld\n", (long)child1.pid);
    printf("[Parent] Child2 created with PID: %ld\n", (long)child2.pid);
    fflush(stdout);

    close_pipe_handle(pipe1.read_handle);
    close_pipe_handle(pipe2.read_handle);
    close_file_handle(file1_handle);
    close_file_handle(file2_handle);

    printf("Enter lines (Ctrl+D / Ctrl+Z to finish):\n");
    fflush(stdout);

    int line_number = 0;
    char line[MAX_LINE];

    while (fgets(line, sizeof(line), stdin) != NULL) {
        line_number++;
        size_t payload_size = strlen(line);

        ssize_t written;
        if (line_number % 2 == 1) {
            written = pipe_write(pipe1.write_handle, line, payload_size);
            if (written == -1) {
                fprintf(stderr, "Error: failed to write to pipe1\n");
            }
        } else {
            written = pipe_write(pipe2.write_handle, line, payload_size);
            if (written == -1) {
                fprintf(stderr, "Error: failed to write to pipe2\n");
            }
        }
    }

    close_pipe_handle(pipe1.write_handle);
    close_pipe_handle(pipe2.write_handle);

    ProcessResult res1 = wait_for_process(&child1);
    ProcessResult res2 = wait_for_process(&child2);

    printf("[Parent] Child1 (PID: %ld) exit code: %d\n", (long)res1.pid, res1.exit_code);
    printf("[Parent] Child2 (PID: %ld) exit code: %d\n", (long)res2.pid, res2.exit_code);
    printf("[Parent] Results written to: %s, %s\n", file1, file2);
    fflush(stdout);
    close_process(&child1);
    close_process(&child2);

    return 0;
}

int main(int argc, char* argv[]) {
    return parent_main(argc, argv);
}