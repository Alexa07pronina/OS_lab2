#ifndef FILE_H
#define FILE_H

#include <stddef.h>
#include <stdint.h>

#ifdef _WIN32
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#else
#include <sys/types.h>
#endif

intptr_t open_file_for_write(const char* path);
ssize_t file_write(intptr_t handle, const char* data, size_t size);
void close_file_handle(intptr_t handle);

#endif