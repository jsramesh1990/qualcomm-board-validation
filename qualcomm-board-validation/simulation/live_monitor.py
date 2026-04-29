import time
import random

class LiveMonitor:
    def __init__(self):
        self.can_frames = 0

    def run(self):
        print('===== Qualcomm Hardware Monitor =====')
        for _ in range(20):
            self.can_frames += random.randint(5, 20)
            temp = round(random.uniform(30.0, 42.0), 2)
            current = round(random.uniform(0.30, 0.60), 2)
            print(f'CAN Frames: {self.can_frames}')
            print(f'Temp: {temp} C')
            print(f'INA231 Current: {current} A')
            print('-' * 40)
            time.sleep(1)

if __name__ == '__main__':
    LiveMonitor().run()
