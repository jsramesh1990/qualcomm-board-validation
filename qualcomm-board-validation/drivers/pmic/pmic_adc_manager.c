#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ADC_CHANNELS 8

typedef struct {
    int channel_id;
    float voltage;
    int enabled;
} ADCChannel;

ADCChannel channels[ADC_CHANNELS];

void init_adc_channels() {
    srand(time(NULL));
    for (int i = 0; i < ADC_CHANNELS; i++) {
        channels[i].channel_id = i;
        channels[i].voltage = 0.0;
        channels[i].enabled = 1;
    }
}

void read_adc_values() {
    for (int i = 0; i < ADC_CHANNELS; i++) {
        if (channels[i].enabled) {
            channels[i].voltage = (rand() % 500) / 100.0;
            printf("ADC[%d] Voltage = %.2fV
", i, channels[i].voltage);
        }
    }
}

int main() {
    init_adc_channels();
    read_adc_values();
    return 0;
}
