add_executable(snippet_0_hello_world
    0_hello_world.c
)

add_executable(snippet_1_signals
    1_signals.c
)

add_executable(snippet_2_signals_clean
    2_signals_clean.c
)

target_link_libraries(snippet_2_signals_clean
    PRIVATE async_signal_waiter
)
