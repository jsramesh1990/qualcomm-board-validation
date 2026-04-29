#include <iostream>
#include <thread>
#include <chrono>
using namespace std;
int main() {
    cout << "Hardware Simulation Started" << endl;
    for(int i=0; i<5; i++) {
        cout << "Virtual Sensor Update: " << i << endl;
        this_thread::sleep_for(chrono::seconds(1));
    }
    return 0;
}

