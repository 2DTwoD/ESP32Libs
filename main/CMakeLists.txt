idf_component_register(
        SRCS
        "app_main.cpp"
        "discrete/src/coil.cpp"
        "discrete/src/programm_coil.cpp"
        "discrete/src/simple_input.cpp"
#        PRIV_REQUIRES spi_flash
        INCLUDE_DIRS
        "discrete/inc")
