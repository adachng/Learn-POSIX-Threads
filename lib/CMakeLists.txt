add_library(async_signal_waiter
    async_signal_waiter.c async_signal_waiter.h
)

target_include_directories(async_signal_waiter
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)
