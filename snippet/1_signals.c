#include <pthread.h>

#include <assert.h>
#include <signal.h> // sigemptyset(), sigaddset(), sigwait()
#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h> // exit()
#include <unistd.h> // sleep()

int get_sigset(sigset_t* const set_p)
{
    assert(set_p != NULL);

    // Empty the set:
    {
        const int res = sigemptyset(
            set_p); // https://man7.org/linux/man-pages/man3/sigemptyset.3p.html
        if (res != 0)
        {
            return res;
        }
    }

    const int signals_arr[3] = {SIGINT, SIGTERM, SIGPIPE};

    // Block all 3 signals:
    for (size_t i = 0; i < 3; i++)
    {
        const int res = sigaddset(
            set_p,
            signals_arr
                [i]); // https://man7.org/linux/man-pages/man3/sigaddset.3p.html
        if (res != 0)
        {
            return res;
        }
    }

    return 0;
}

void* signal_thread_cb(void* const arg_p)
{
    atomic_bool* const is_end_p = arg_p;
    sigset_t           signal_set;

    if (get_sigset(&signal_set) != 0)
    {
        perror("get_sigset()");
        exit(1);
    }

    if (sigdelset(&signal_set, SIGPIPE) != 0)
    {
        perror("sigdelset()");
        exit(1);
    }

    while (!atomic_load(is_end_p))
    {
        printf("Calling sigwait()...\n");
        // https://man7.org/linux/man-pages/man3/sigwait.3.html
        int       which_signal = 0;
        const int res          = sigwait(&signal_set, &which_signal);
        if (res != 0)
        {
            perror("sigwait()");
            exit(1);
        }

        if (which_signal == SIGINT)
        {
            atomic_store(is_end_p, true);
            printf("SIGINT received, termination flag set\n");
        }
        else if (which_signal == SIGTERM)
        {
            atomic_store(is_end_p, true);
            printf("SIGTERM received, termination flag set\n");
        }
        else
        {
            printf("Unexpected: res = %d and which_signal = %d\n",
                   res,
                   which_signal);
        }
    }

    pthread_exit(NULL);
}

void* dummy_thread_cb(void* const arg_p)
{
    atomic_bool* const is_end_p = arg_p;

    while (!atomic_load(is_end_p))
    {
        printf("Doing work...\n");
        sleep(5);
        printf("Completed work\n");
    }

    pthread_exit(NULL);
}

int get_pthreads(pthread_t* const   signal_thread_p,
                 pthread_t* const   dummy_thread_p,
                 atomic_bool* const is_end_p)
{
    assert(is_end_p != NULL);

    {
        const int res =
            pthread_create(signal_thread_p, NULL, signal_thread_cb, is_end_p);
        if (res != 0)
        {
            return res;
        }
    }

    {
        const int res =
            pthread_create(dummy_thread_p, NULL, dummy_thread_cb, is_end_p);
        if (res != 0)
        {
            return res;
        }
    }

    return 0;
}

int main()
{
    sigset_t sig_ignore_set;
    if (get_sigset(&sig_ignore_set) != 0)
    {
        perror("get_sigset()");
        return 1;
    }

    // https://pubs.opengroup.org/onlinepubs/9799919799/functions/pthread_sigmask.html
    // https://man7.org/linux/man-pages/man2/sigprocmask.2.html
    {
        const int res = pthread_sigmask(SIG_BLOCK, &sig_ignore_set, NULL);
        if (res != 0)
        {
            perror("pthread_sigmask()");
            return 1;
        }
    }

    pthread_t signal_thread;
    pthread_t dummy_thread;

    atomic_bool is_end = false;

    {
        const int res = get_pthreads(&signal_thread, &dummy_thread, &is_end);
        if (res != 0)
        {
            perror("get_pthreads()");
            return 1;
        }
    }

    pthread_join(signal_thread, NULL);
    pthread_join(dummy_thread, NULL);

    printf("Process gracefully shut down\n");

    return 0;
}
