#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#define MAX_LINE 4096

static int is_vowel(char c) {
    return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' ||
           c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U';
}

static void remove_vowels(const char* input, char* output) {
    size_t j = 0;
    for (size_t i = 0; input[i] != '\0'; i++) {
        if (!is_vowel(input[i])) {
            output[j++] = input[i];
        }
    }
    output[j] = '\0';
}

int main(int argc, char* argv[]) {
    int child_id = 1;
    if (argc > 2) {
        child_id = atoi(argv[2]);
    }

#ifdef _WIN32
    printf("[Child%d] PID: %lu, started\n", child_id, GetCurrentProcessId());
#else
    printf("[Child%d] PID: %d, started\n", child_id, getpid());
#endif
    fflush(stdout);

    char line[MAX_LINE];
    char processed[MAX_LINE];

    while (fgets(line, sizeof(line), stdin) != NULL) {
        remove_vowels(line, processed);

        size_t len = strlen(processed);
        if (len > 0 && processed[len - 1] != '\n') {
            printf("%s\n", processed);
        } else {
            printf("%s", processed);
        }
        fflush(stdout);
    }

#ifdef _WIN32
    printf("[Child%d] PID: %lu, finished\n", child_id, GetCurrentProcessId());
#else
    printf("[Child%d] PID: %d, finished\n", child_id, getpid());
#endif
    fflush(stdout);

    return 0;
}