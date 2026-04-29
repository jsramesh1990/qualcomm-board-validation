#include <iostream>
using namespace std;
class INA231 {
public:
    void readSensor() {
        cout << "Voltage: 5.02V" << endl;
        cout << "Current: 0.42A" << endl;
        cout << "Power: 2.11W" << endl;
    }
};
