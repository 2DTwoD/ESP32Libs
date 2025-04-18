idf_component_register(
        SRCS
        "app_main.cpp"
        "discrete/src/coil.cpp"
        "discrete/src/program_coil.cpp"
        "discrete/src/simple_input.cpp"
        "misc/src/array_fun.cpp"
        "misc/src/math_fun.cpp"
        "timer/src/common_timer.cpp"
        "timer/src/on_delay.cpp"
        "timer/src/off_delay.cpp"
        "timer/src/pulse.cpp"
        "timer/src/updater.cpp"

        PRIV_REQUIRES spi_flash

        INCLUDE_DIRS
        "discrete/inc"
        "misc/inc"
        "timer/inc"
)
