
# Qualcomm Board Validation Advanced Modular Simulation Project

![C](https://img.shields.io/badge/C-Low_Level_Drivers-blue)
![C++](https://img.shields.io/badge/C++-Simulation_Logic-orange)
![Python](https://img.shields.io/badge/Python-Automation_&_Dashboard-green)
![Embedded](https://img.shields.io/badge/Embedded-Qualcomm_Workflow-purple)
![Status](https://img.shields.io/badge/Status-Active_Development-success)

## Overview
A large-scale modular Qualcomm board validation and simulation framework designed to model embedded hardware bring-up, driver validation, and automated testing workflows.

This repository mixes **C**, **C++**, and **Python** to simulate realistic embedded development patterns:
- **C** → low-level hardware drivers
- **C++** → device abstraction and simulation engines
- **Python** → automation, dashboards, logs, and test orchestration

## Architecture Flow
```text
Boot Sequence
   ↓
Peripheral Initialization
   ↓
Driver Layer Bring-up
   ├── PMIC ADC / Thermistor
   ├── USB Host/Device
   ├── CAN Bus
   ├── SD Card
   ├── LED Control
   └── INA231 Power Monitor
   ↓
Simulation Engine
   ├── Sensor Simulation
   ├── Event Injection
   ├── Dashboard Rendering
   └── Log Collection
   ↓
Automated Testing
   ├── Unit Tests
   ├── Integration Tests
   └── Regression Tests
   ↓
Reports & Outputs
```

## Features
- Modular embedded driver architecture
- PMIC ADC multi-channel simulation
- Thermistor monitoring
- USB hotplug and enumeration simulation
- CAN traffic generator and loopback validation
- SD card insert/remove and filesystem simulation
- LED blink pattern and PWM simulation
- INA231 voltage/current/power monitoring
- Virtual dashboard console output
- Automated test runners and reports

## Example Virtual Output
```text
[BOOT] Qualcomm board initialized
[USB] Device connected -> enumeration success
[SD] Card inserted -> mounted /dev/mmcblk0p1
[CAN] RX Frames/sec = 142
[PMIC] ADC0=3.31V ADC1=1.81V TEMP=36.4C
[INA231] Voltage=5.02V Current=0.43A Power=2.16W
[LED] Blink pattern HEARTBEAT active
```

## Build & Run
### Build all modules
```bash
./scripts/build_all.sh
```

### Run simulation
```bash
./scripts/run_simulation.sh
```

### Run tests
```bash
./scripts/run_tests.sh
```

## Directory Highlights
- `drivers/` → embedded peripheral drivers
- `simulation/` → virtual hardware and dashboards
- `tests/` → unit, integration, regression tests
- `outputs/` → logs, reports, screenshots
- `docs/` → architecture and learning guides

## Learning Outcomes
- Embedded driver lifecycle
- Qualcomm board bring-up concepts
- Hardware abstraction layer design
- Interrupt and event-driven systems
- Validation automation pipelines
- Mixed-language embedded project organization

## Future Improvements
- UART console simulator
- I2C transaction viewer
- SPI peripheral simulation
- Linux DTS parser
- Interactive web dashboard

## License
MIT License
