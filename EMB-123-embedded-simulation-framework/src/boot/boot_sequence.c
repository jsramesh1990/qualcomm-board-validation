#include "boot_sequence.h"
#include "logger.h"
#include "watchdog.h"
#include "pmic_adc.h"
#include "thermistor.h"
#include "usb_host.h"
#include "can_bus.h"
#include "sd_card.h"
#include "led_ctrl.h"
#include "ina231.h"
#include "sensor_sim.h"

static error_code_t power_on_self_test(void) {
    LOG_INFO("Running POST...");
    
    // Test memory
    volatile uint32_t test_pattern = 0xDEADBEEF;
    volatile uint32_t* test_addr = (uint32_t*)malloc(sizeof(uint32_t));
    if (!test_addr) return ERR_OUT_OF_MEMORY;
    
    *test_addr = test_pattern;
    if (*test_addr != test_pattern) {
        free((void*)test_addr);
        return ERR_HARDWARE;
    }
    free((void*)test_addr);
    
    // Test CPU registers (simulated)
    uint32_t reg_test = 0x12345678;
    __asm__ volatile("" : "+r"(reg_test));
    if (reg_test != 0x12345678) return ERR_HARDWARE;
    
    LOG_INFO("POST completed successfully");
    return ERR_OK;
}

static error_code_t peripheral_init(void) {
    LOG_INFO("Initializing peripherals...");
    
    // PMIC initialization
    if (pmic_init() != ERR_OK) {
        LOG_WARN("PMIC init failed - using defaults");
    }
    
    // USB initialization
    usb_config_t usb_cfg = {
        .mode = USB_MODE_HOST,
        .speed = USB_SPEED_HIGH,
        .power_current = 500
    };
    if (usb_init(&usb_cfg) != ERR_OK) {
        LOG_WARN("USB init failed");
    }
    
    return ERR_OK;
}

static error_code_t driver_load(void) {
    LOG_INFO("Loading drivers...");
    
    // CAN driver
    can_config_t can_cfg = {
        .controller_id = 0,
        .bitrate = 250000,
        .loopback = g_system_state.simulation_mode
    };
    if (can_init(&can_cfg) != ERR_OK) {
        return ERR_HARDWARE;
    }
    
    // SD Card driver
    sd_config_t sd_cfg = {
        .speed_mode = SD_HIGH_SPEED,
        .simulation = g_system_state.simulation_mode
    };
    if (sd_init(&sd_cfg) != ERR_OK) {
        LOG_WARN("SD card init failed - continuing without storage");
    }
    
    // LED driver
    led_config_t led_cfg = {
        .pattern = LED_PATTERN_HEARTBEAT,
        .brightness = 128
    };
    led_init(&led_cfg);
    
    // INA231 power monitor
    ina231_config_t ina_cfg = {
        .i2c_addr = 0x40,
        .shunt_resistor = 0.01f,
        .max_current = 10.0f
    };
    if (ina231_init(&ina_cfg) != ERR_OK) {
        LOG_WARN("INA231 init failed");
    }
    
    return ERR_OK;
}

static error_code_t simulation_init(void) {
    if (!g_system_state.simulation_mode) {
        LOG_INFO("Hardware mode - skipping simulation init");
        return ERR_OK;
    }
    
    LOG_INFO("Initializing simulation engine...");
    
    // Sensor simulation
    sensor_sim_config_t sensor_cfg = {
        .noise_level = 0.05f,
        .update_rate_hz = 100
    };
    sensor_sim_init(&sensor_cfg);
    
    return ERR_OK;
}

static error_code_t system_ready(void) {
    LOG_INFO("System ready - all components initialized");
    
    // Start heartbeat LED
    led_set_pattern(LED_PATTERN_HEARTBEAT);
    
    // Enable watchdog
    watchdog_enable(5000);
    
    return ERR_OK;
}

error_code_t boot_sequence(void) {
    uint32_t boot_start = get_tick_ms();
    
    typedef error_code_t (*stage_func_t)(void);
    stage_func_t stages[] = {
        power_on_self_test,
        peripheral_init,
        driver_load,
        simulation_init,
        system_ready
    };
    const char* stage_names[] = {
        "POST",
        "Peripheral Init",
        "Driver Load",
        "Simulation Init",
        "System Ready"
    };
    
    for (int i = 0; i < 5; i++) {
        g_system_state.stage = (boot_stage_t)i;
        LOG_INFO("Stage %d/%d: %s", i+1, 5, stage_names[i]);
        
        error_code_t err = stages[i]();
        if (err != ERR_OK) {
            LOG_ERROR("Boot failed at stage: %s", stage_names[i]);
            g_system_state.stage = BOOT_STAGE_FAILED;
            g_system_state.last_error = err;
            return err;
        }
        
        watchdog_feed();
    }
    
    g_system_state.uptime_ms = get_tick_ms() - boot_start;
    LOG_INFO("Boot sequence completed in %u ms", g_system_state.uptime_ms);
    
    return ERR_OK;
}
