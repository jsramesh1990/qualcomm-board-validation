#!/bin/bash
gcc drivers/pmic/config_pmic.c -o pmic_test
gcc drivers/usb/usb_driver.c -o usb_test
gcc drivers/can/can_driver.c -o can_test
g++ drivers/ina231/ina231_driver.cpp -o ina231_test
g++ drivers/sdcard/sdcard_driver.cpp -o sd_test
g++ drivers/led/led_driver.cpp -o led_test
g++ simulation/hardware_simulator.cpp -o simulator

echo 'Build complete'
