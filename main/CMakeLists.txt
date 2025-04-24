idf_component_register(
        SRCS
        "app_main.cpp"
        "comm/src/i2c_driver.cpp"
        "comm/src/wifi_ap_tcp_server.cpp"
        "discrete/src/coil.cpp"
        "discrete/src/program_coil.cpp"
        "discrete/src/simple_input.cpp"
        "discrete/src/simple_input_delayed.cpp"
        "misc/src/array_fun.cpp"
        "misc/src/math_fun.cpp"
        "sensors/src/shtc3.cpp"
        "tasks/src/test_task.cpp"
        "timer/src/common_timer.cpp"
        "timer/src/on_delay.cpp"
        "timer/src/off_delay.cpp"
        "timer/src/pulse.cpp"
        "timer/src/updater.cpp"

        PRIV_REQUIRES spi_flash

        INCLUDE_DIRS
        "comm/inc"
        "discrete/inc"
        "misc/inc"
        "sensors/inc"
        "tasks/inc"
        "timer/inc"
)
