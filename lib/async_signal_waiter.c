#include "async_signal_waiter.h"

#include <assert.h>
#include <bits/pthreadtypes.h>
#include <pthread.h>
#include <signal.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

struct workerArg
{
    atomic_bool* is_triggered_p;

    pthread_barrier_t* barrier_p; // to finish copying

    sigset_t sigwait_set;
};

static void* signal_waiter_cb(void* const arg_p)
{
    // Copy into this thread-local storage from the argument:
    const struct workerArg arg = *(struct workerArg*)arg_p;

    pthread_barrier_wait(arg.barrier_p);

    // Wait for the signal specified:
    int unused_which_signal; // just to prevent invalid write
    sigwait(&arg.sigwait_set, &unused_which_signal);

    // Set the flag:
    atomic_store(arg.is_triggered_p, true);

    pthread_exit(arg_p);
}

atomic_bool* async_signal_waiter(pthread_t* const      thread_out_p,
                                 const int*            signals_arr,
                                 size_t                arr_len,
                                 const pthread_attr_t* attr_p,
                                 int* const            create_res_p)
{
    atomic_bool* const ret_p = malloc(sizeof(atomic_bool));
    if (ret_p == NULL || arr_len <= 0)
    {
        return NULL;
    }

    *ret_p = false;

    // Barrier to ensure the created thread completes copying the arguments:
    // https://man7.org/linux/man-pages/man3/pthread_barrier_destroy.3p.html
    pthread_barrier_t barrier;

    if (pthread_barrier_init(&barrier, NULL, 2) != 0)
    {
        goto bad_return;
    }

    // Get the signal set to wait for in the created thread:
    sigset_t sigwait_set;
    if (sigemptyset(&sigwait_set) != 0)
    {
        goto bad_return;
    }

    for (size_t i = 0; i < arr_len; i++)
    {
        if (sigaddset(&sigwait_set, signals_arr[i]) != 0)
        {
            goto bad_return;
        }
    }

    struct workerArg arg = {.barrier_p      = &barrier,
                            .is_triggered_p = ret_p,
                            .sigwait_set    = sigwait_set};

    // Create the thread:
    pthread_t signal_waiter_thread;

    const int res =
        pthread_create(&signal_waiter_thread, attr_p, signal_waiter_cb, &arg);

    // Handle optional output arguments:
    if (thread_out_p != NULL)
    {
        *thread_out_p = signal_waiter_thread;
    }

    if (create_res_p != NULL)
    {
        *create_res_p = res;
    }

    if (res != 0)
    {
        goto bad_return;
    }

    // Wait for the created thread to finish copying the arguments:
    pthread_barrier_wait(&barrier);

    // https://man7.org/linux/man-pages/man3/pthread_barrierattr_destroy.3p.html
    pthread_barrier_destroy(&barrier);

    return ret_p;
bad_return:
    free(ret_p);
    pthread_barrier_destroy(&barrier);
    return NULL;
}
