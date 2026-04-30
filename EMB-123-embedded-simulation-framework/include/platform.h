#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Platform detection
#if defined(__linux__)
    #define PLATFORM_LINUX 1
    #define PLATFORM_STRING "Linux"
    #include <unistd.h>
    #include <pthread.h>
    #include <time.h>
#elif defined(_WIN32) || defined(_WIN64)
    #define PLATFORM_WINDOWS 1
    #define PLATFORM_STRING "Windows"
    #include <windows.h>
#elif defined(__APPLE__)
    #define PLATFORM_MACOS 1
    #define PLATFORM_STRING "macOS"
    #include <unistd.h>
    #include <pthread.h>
    #include <time.h>
#elif defined(STM32_TARGET)
    #define PLATFORM_STM32 1
    #define PLATFORM_STRING "STM32"
#elif defined(QUALCOMM_TARGET)
    #define PLATFORM_QUALCOMM 1
    #define PLATFORM_STRING "Qualcomm"
#else
    #define PLATFORM_UNKNOWN 1
    #define PLATFORM_STRING "Unknown"
#endif

// Time functions
static inline void delay_ms(uint32_t ms) {
    #if defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
        usleep(ms * 1000);
    #elif defined(PLATFORM_WINDOWS)
        Sleep(ms);
    #elif defined(STM32_TARGET)
        HAL_Delay(ms);
    #else
        volatile uint32_t i;
        for (i = 0; i < ms * 1000; i++);
    #endif
}

static inline uint32_t get_tick_ms(void) {
    #if defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return (ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);
    #elif defined(PLATFORM_WINDOWS)
        return GetTickCount();
    #elif defined(STM32_TARGET)
        return HAL_GetTick();
    #else
        static uint32_t tick = 0;
        return tick++;
    #endif
}

// Error codes
typedef enum {
    ERR_OK = 0,
    ERR_GENERAL = -1,
    ERR_TIMEOUT = -2,
    ERR_INVALID_PARAM = -3,
    ERR_HARDWARE = -4,
    ERR_NOT_INITIALIZED = -5,
    ERR_BUSY = -6,
    ERR_OUT_OF_MEMORY = -7,
    ERR_NOT_FOUND = -8,
    ERR_PERMISSION = -9
} error_code_t;

// Boot stages
typedef enum {
    BOOT_STAGE_POWER_ON = 0,
    BOOT_STAGE_PERIPHERAL_INIT,
    BOOT_STAGE_DRIVER_LOAD,
    BOOT_STAGE_SIMULATION,
    BOOT_STAGE_SYSTEM_READY,
    BOOT_STAGE_FAILED
} boot_stage_t;

// System state structure
typedef struct {
    boot_stage_t stage;
    error_code_t last_error;
    uint32_t uptime_ms;
    bool simulation_mode;
    uint32_t heap_used;
    uint32_t heap_total;
} system_state_t;

extern system_state_t g_system_state;

// Helper functions
const char* error_to_string(error_code_t err);
const char* boot_stage_to_string(boot_stage_t stage);

#endif // PLATFORM_H
