#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

// Board identification
#define BOARD_NAME "Qualcomm SA8155P"
#define BOARD_REVISION "2.0"
#define BOARD_SERIAL "EMB-123-001"

// Clock configuration
#define SYSTEM_CLOCK_HZ 800000000
#define CAN_CLOCK_HZ 50000000
#define I2C_CLOCK_HZ 400000

// Memory configuration
#define RAM_SIZE (4 * 1024 * 1024)  // 4MB RAM
#define FLASH_SIZE (32 * 1024 * 1024) // 32MB Flash

// Peripheral mapping
#define PMIC_I2C_BUS 0
#define PMIC_I2C_ADDR 0x48
#define INA231_I2C_BUS 1
#define INA231_I2C_ADDR 0x40
#define CAN_CONTROLLER_0 0
#define CAN_CONTROLLER_1 1

// Pin mapping
#define LED_STATUS_PIN 13
#define LED_ERROR_PIN 14
#define SD_CARD_DETECT_PIN 25
#define USB_OTG_ID_PIN 26

// Feature flags
#define ENABLE_SIMULATION 1
#define ENABLE_WATCHDOG 1
#define ENABLE_POWER_MONITOR 1
#define ENABLE_TELEMETRY 1

// Boot configuration
#define BOOT_TIMEOUT_MS 5000
#define WATCHDOG_TIMEOUT_MS 3000
#define POST_ENABLED 1

#endif // BOARD_CONFIG_H
