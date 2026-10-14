cmake_minimum_required(VERSION 3.10)
project(lab2 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(WIN32)
    set(PLATFORM_SOURCES
        src/process_win.c
        src/pipe_win.c
        src/file_win.c
    )
else()
    set(PLATFORM_SOURCES
        src/process_posix.c
        src/pipe_posix.c
        src/file_posix.c
    )
endif()

add_executable(parent
    parent.c
    ${PLATFORM_SOURCES}
)
target_include_directories(parent PRIVATE headers)

add_executable(child
    child.c
)
target_include_directories(child PRIVATE headers)