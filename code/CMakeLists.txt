cmake_minimum_required(VERSION 3.13)

# Detect if the active kit is an ARM cross-compiler
if(CMAKE_CXX_COMPILER MATCHES "arm-none-eabi")
    message(STATUS "Detected that the current kit is a cross-compiler.")
    set(CrossCompiling 1)
else()
    message(STATUS "Detected that the current kit is a host compiler. Building the test harness.")
endif()

if(CrossCompiling)
    # Yes, build for the RP2040

    set(CMAKE_C_STANDARD 11)
    set(CMAKE_CXX_STANDARD 17)

    # Specify an increased oscillator startup delay to work around a potential hardware issue where the 
    # clock is slow to boot.
    add_compile_definitions(PICO_XOSC_STARTUP_DELAY_MULTIPLIER=64)
 
    # Pull in Raspberry Pi Pico SDK (must be before project)
    include(pico_sdk_import.cmake)

    # Add the FatFs_SPI submodule (core library for SD card and FatFs)
    add_subdirectory(src/libs/no-OS-FatFS/FatFs_SPI)  # This is the main CMakeLists.txt for FatFs and SPI

    project(cc3501-labs C CXX ASM)

    # Initialise the Raspberry Pi Pico SDK
    pico_sdk_init()

    add_executable(labs)
    target_sources(labs 
        PUBLIC
        src/main.cpp
        src/drivers/logging/logging.cpp
        src/drivers/sdi12/sdi12.cpp
        src/drivers/sd_card/sd_card.cpp
        src/drivers/loadcell/loadcell.cpp

        src/drivers/terminal/terminal.cpp
        src/drivers/dac/MCP4716.cpp

        src/libs/hw_config.c   
    )
    target_include_directories(labs
        PUBLIC 
        src/
    )

    pico_set_program_name(labs "cc3501-labs")
    pico_set_program_version(labs "0.2")

    # select UART for standard IO
    pico_enable_stdio_uart(labs 1)
    pico_enable_stdio_usb(labs 0) # change this flag when deploying this code without a debugger!

    # compile the PIO file
    pico_generate_pio_header(labs ${CMAKE_CURRENT_LIST_DIR}/src/drivers/WS2812/WS2812.pio)

    # Add the standard library and FatFs_SPI to the build
    target_link_libraries(labs
        pico_stdlib
        hardware_spi
        hardware_adc      # Add this for ADC hardware
        hardware_rtc      # Add this for RTC hardware
        hardware_i2c
        hardware_dma
        hardware_pio
        hardware_interp
        hardware_timer
        hardware_watchdog
        hardware_clocks
        hardware_pwm
        FatFs_SPI  # Link the FatFs_SPI library from the submodule
    )

    pico_add_extra_outputs(labs)

else()

    set(CMAKE_C_STANDARD 11)
    set(CMAKE_CXX_STANDARD 20)

    # We are building natively, so create the test harness instead
    project(cc3501-labs CXX)

    add_executable(labs)
    target_sources(labs 
        PUBLIC
        src/main.cpp
        src/drivers/logging/logging.cpp
        src/drivers/sdi12/sdi12.cpp
        src/drivers/loadcell/loadcell.cpp
        src/drivers/terminal/terminal.cpp
        src/drivers/dac/MCP4716.cpp
        src/libs/hw_config.c # configures the SD card hardware
        tests/mocks/pico/stdlib.cpp
        tests/mocks/pico/time.cpp
        tests/mocks/hardware/gpio.cpp
        tests/mocks/hardware/pio.cpp
        tests/mocks/ws2812.cpp
    )
    target_include_directories(labs
        PUBLIC 
        src/
        tests/
        tests/mocks/
    )
    target_compile_definitions(labs 
        PUBLIC
        TEST_HARNESS=1
    )

endif()

target_compile_definitions(labs 
    PUBLIC
    LOG_DRIVER_STYLE=${LogDriverImplementation}
)
