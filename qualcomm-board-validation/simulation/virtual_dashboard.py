import time
print('=== Qualcomm Virtual Dashboard ===')
for i in range(5):
    print(f'CAN Frames: {100 + i*10}')
    print('USB: Connected | SD: Mounted | LED: ON')
    print('Temp: 34.2 C | INA231 Power: 2.11W')
    time.sleep(1)


