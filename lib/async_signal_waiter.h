#ifndef LEARN_POSIX_THREADS__ASYNC_SIGNAL_WAITER
#define LEARN_POSIX_THREADS__ASYNC_SIGNAL_WAITER

#include <pthread.h>

#include <stdatomic.h>

#ifdef __cplusplus
extern "C"
{
#endif

atomic_bool* async_signal_waiter(pthread_t* const      thread_out_p,
                                 const int*            signals_arr,
                                 size_t                arr_len,
                                 const pthread_attr_t* attr_p,
                                 int*                  create_res_p);

#ifdef __cplusplus
}
#endif

#endif // LEARN_POSIX_THREADS__ASYNC_SIGNAL_WAITER
