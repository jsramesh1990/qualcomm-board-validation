#!/bin/bash
./pmic_test
./usb_test
./can_test
./ina231_test
./sd_test
./led_test
./simulator
python3 simulation/virtual_dashboard.py
