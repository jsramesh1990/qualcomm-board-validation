cmake_minimum_required(VERSION 3.16)
project(EmbeddedSimulationFramework VERSION 1.0.0)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Options
option(BUILD_TESTS "Build test suite" ON)
option(BUILD_SIMULATION "Build simulation mode" ON)
option(ENABLE_COVERAGE "Enable code coverage" OFF)
option(ENABLE_ASAN "Enable AddressSanitizer" OFF)

# Platform detection
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(PLATFORM_LINUX ON)
    add_compile_definitions(PLATFORM_LINUX)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(PLATFORM_WINDOWS ON)
    add_compile_definitions(PLATFORM_WINDOWS)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(PLATFORM_MACOS ON)
    add_compile_definitions(PLATFORM_MACOS)
endif()

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
endif()

# Directories
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(CONFIG_DIR ${CMAKE_SOURCE_DIR}/config)

include_directories(${INCLUDE_DIR})
include_directories(${SRC_DIR})

# Sources
set(SOURCES
    ${SRC_DIR}/main.c
    ${SRC_DIR}/boot/boot_sequence.c
    ${SRC_DIR}/boot/error_handler.c
    ${SRC_DIR}/drivers/pmic/pmic_adc.c
    ${SRC_DIR}/drivers/pmic/thermistor.c
    ${SRC_DIR}/drivers/usb/usb_host.c
    ${SRC_DIR}/drivers/can/can_bus.c
    ${SRC_DIR}/drivers/sd_card/sd_card.c
    ${SRC_DIR}/drivers/led/led_ctrl.c
    ${SRC_DIR}/drivers/ina231/ina231.c
    ${SRC_DIR}/simulation/sensor_sim.c
    ${SRC_DIR}/simulation/event_injector.c
    ${SRC_DIR}/simulation/virtual_dashboard.c
    ${SRC_DIR}/core/scheduler.c
    ${SRC_DIR}/core/logger.c
    ${SRC_DIR}/core/watchdog.c
    ${SRC_DIR}/core/config_manager.c
)

# Dependencies
find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)

if(BUILD_SIMULATION)
    find_package(Curses REQUIRED)
    include_directories(${CURSES_INCLUDE_DIR})
    set(SOURCES ${SOURCES} ${SRC_DIR}/simulation/virtual_dashboard.c)
endif()

# Executable
add_executable(embedded_sim ${SOURCES})
target_link_libraries(embedded_sim
    ${CMAKE_THREAD_LIBS_INIT}
    ${CURSES_LIBRARIES}
    m
)

# Compiler flags
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_CLANG)
    target_compile_options(embedded_sim PRIVATE
        -Wall -Wextra -Wpedantic
        -Werror -Wconversion
    )
endif()

if(ENABLE_ASAN)
    target_compile_options(embedded_sim PRIVATE -fsanitize=address)
    target_link_options(embedded_sim PRIVATE -fsanitize=address)
endif()

if(ENABLE_COVERAGE)
    target_compile_options(embedded_sim PRIVATE --coverage)
    target_link_options(embedded_sim PRIVATE --coverage)
endif()

# Tests
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(src/tests)
endif()

# Installation
install(TARGETS embedded_sim DESTINATION bin)
install(FILES ${CONFIG_DIR}/sim_params.json DESTINATION config)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/scripts DESTINATION .)

# Custom targets
add_custom_target(run
    COMMAND ./embedded_sim
    DEPENDS embedded_sim
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running embedded simulation"
)

add_custom_target(clean_all
    COMMAND ${CMAKE_COMMAND} -E remove_directory build
    COMMAND ${CMAKE_COMMAND} -E remove_directory coverage
    COMMENT "Cleaning all build artifacts"
)
