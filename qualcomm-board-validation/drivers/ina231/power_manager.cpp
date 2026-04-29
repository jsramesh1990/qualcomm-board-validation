#include <iostream>
#include <vector>
using namespace std;

class PowerManager {
private:
    vector<float> currentSamples;
public:
    void addSample(float sample) {
        currentSamples.push_back(sample);
    }

    float averageCurrent() {
        float sum = 0;
        for (float s : currentSamples) sum += s;
        return currentSamples.empty() ? 0 : sum / currentSamples.size();
    }

    void printReport() {
        cout << "Average Current: " << averageCurrent() << " A" << endl;
        cout << "Estimated Power: " << averageCurrent() * 5.02 << " W" << endl;
    }
};

int main() {
    PowerManager pm;
    pm.addSample(0.41);
    pm.addSample(0.43);
    pm.addSample(0.39);
    pm.printReport();
    return 0;
}
