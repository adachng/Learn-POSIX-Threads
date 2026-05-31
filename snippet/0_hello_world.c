#include <pthread.h>

#include <assert.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>

void* worker_cb(void* arg_p)
{
    printf("World!\n");

    assert(arg_p != NULL);
    printf("worker_cb(%s)\n", (const char*)arg_p);

    int* i_p = malloc(sizeof(int));
    *i_p     = 123;
    pthread_exit(i_p);
}

int main()
{
    pthread_t my_thread;

    printf("Hello, ");
    // Start the thread:
    {
        const char* const input_str = "This is the thread's input";

        const int res =
            pthread_create(&my_thread, NULL, worker_cb, (void*)input_str);

        if (res != 0)
        {
            perror("pthread_create()");
            return 1;
        }
    }

    // Synchronise with the termination of the thread:
    {
        int* thread_res_p = NULL;

        const int res = pthread_join(my_thread, (void**)&thread_res_p);
        if (res != 0)
        {
            perror("pthread_join()");
            return 1;
        }

        assert(thread_res_p != NULL);
        printf("thread_res_p = %d\n", *thread_res_p);
        free(thread_res_p);
    }

    return 0;
}
