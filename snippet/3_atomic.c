#include <pthread.h>

#include <assert.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

// https://en.cppreference.com/c/atomic/memory_order
const memory_order STORE_MEM_ORDER = memory_order_release;
const memory_order LOAD_MEM_ORDER  = memory_order_acquire;

// https://youtu.be/OyNG4qiWnmU?si=qNOWUc0u2iNqNHTF
// NOTE: think of acquire and release as subscribe and publish respectively.

_Atomic(char*) some_task_str = NULL;

atomic_bool is_running = true;

void* producer_cb(void* arg_p)
{
    char* task_str = NULL;

    task_str = malloc(128UL * sizeof(char));
    assert(task_str != NULL);

    const time_t unix_time_now = time(NULL);

    while (atomic_load_explicit(&is_running, memory_order_acquire)) { }
    return NULL;
}

void* consumer_cb(void* arg_p)
{
    while (atomic_load_explicit(&is_running, memory_order_acquire)) { }
    return NULL;
}

int main(const int                argc,
         const char* const* const argv_str_arr)
{
    some_task_str = NULL;
    return 0;
}
