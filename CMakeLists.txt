cmake_minimum_required(VERSION 3.22)

project(
    learn_posix_threads
    LANGUAGES C
)

list(PREPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

include(ProjectEssentials)

option(BUILD_SHARED_LIBS "Build using shared libraries." OFF)

if(PROJECT_IS_TOP_LEVEL)
    set(CMAKE_C_STANDARD 17) # NOTE: stdatomic.h is introduced in C11
    set(CMAKE_C_STANDARD_REQUIRED ON)
    set(CMAKE_C_EXTENSIONS OFF)

    # https://cmake.org/cmake/help/latest/module/FindThreads.html
    set(THREADS_PREFER_PTHREAD_FLAG TRUE)
    find_package(Threads)
    if(NOT CMAKE_USE_PTHREADS_INIT)
        message(FATAL_ERROR "The thread library to use is incompatible with POSIX threads API")
    endif()

    link_libraries(Threads::Threads)

    add_subdirectory(snippet)
else()
    message(FATAL_ERROR "This project is never intended to be used as a library.")
endif()
