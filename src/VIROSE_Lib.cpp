#include "VIROSE_Lib.h"

Sound::Sound(uint8_t pin, float vRef) {
    this->pin = pin;
    this->vRef = vRef;
    this->adcRes = 4095;  // Resolusi ADC ESP32 (12 bit = 4095 decimal)
    this->adcMax = this->vRef;

    // Konfigurasi pin sebagai input.
    pinMode(pin, INPUT);

    // Inisialisasi sampel-sampel tegangan dengan 0.
    for (int i = 0; i < SAMPLE_COUNT; i++) {
        this->samples[i] = 0;
    }
    this->index = 0;
}

float Sound::readVoltage() {
    // Baca nilai dari ADC.
    int adcValue = analogRead(pin);

    // Konversi nilai ADC ke dalam tegangan (dalam Volt).
    return adcValue * adcMax / adcRes;
}

float Sound::getFilteredVoltage() {
    // Baca nilai tegangan dari sensor suara.
    float voltage = readVoltage();

    // Update sampel-sampel tegangan dengan nilai tegangan yang baru.
    updateSamples(voltage);

    // Hitung rata-rata dari sampel-sampel tegangan.
    float sum = 0;
    for (int i = 0; i < SAMPLE_COUNT; i++) {
        sum += samples[i];
    }
    return sum / SAMPLE_COUNT;
}

uint8_t Sound::getDigitalValue() {
    // Baca nilai tegangan yang telah disaring dengan filter moving average.
    float voltage = getFilteredVoltage();

    // Konversi nilai tegangan ke dalam nilai digital.
    if (voltage > 0.5) {
        return 1;
    } else {
        return 0;
    }
}

void Sound::updateSamples(float voltage) {
    // Update sampel-sampel tegangan dengan nilai tegangan yang baru.
    samples[index] = voltage;

    // Increment indeks sampel.
    index++;
    if (index >= SAMPLE_COUNT) {
        index = 0;
    }
}

PWMTask::PWMTask(uint8_t pin, uint8_t pwmChannel) {
    this->pin = pin;
    this->pwmChannel = pwmChannel;

    // configure LED PWM functionalitites
    ledcSetup(this->pwmChannel, 5000, 8);

    // attach the channel to the GPIO to be controlled
    ledcAttachPin(this->pin, this->pwmChannel);

    // Start task.
    xTaskCreate(
        [](void *obj) -> void {
            static_cast<PWMTask *>(obj)->Task(obj);
        },
        String("PWMTask" + pin).c_str(),
        1000,
        this,
        1,
        NULL);
}

void PWMTask::Task(void *pvParameters) {
    for (;;) {
        switch (mode) {
            case FADE:
                for (int j = 0; j <= brightness; j++) {
                    ledcWrite(pwmChannel, j);
                    vTaskDelay((onTime / brightness) / portTICK_PERIOD_MS);
                }
                for (int j = brightness; j >= 0; j--) {
                    ledcWrite(pwmChannel, j);
                    vTaskDelay((offTime / brightness) / portTICK_PERIOD_MS);
                }
                break;

            case FLICKER:
                ledcWrite(pwmChannel, 255);
                vTaskDelay(onTime / portTICK_PERIOD_MS);
                ledcWrite(pwmChannel, 0);
                vTaskDelay(offTime / portTICK_PERIOD_MS);
                break;

            case ON:
                ledcWrite(pwmChannel, 255);
                vTaskDelay(500 / portTICK_PERIOD_MS);
                break;

            case OFF:
                ledcWrite(pwmChannel, 0);
                vTaskDelay(500 / portTICK_PERIOD_MS);
                break;

            default:
                vTaskDelay(500 / portTICK_PERIOD_MS);
                break;
        }
    }
}

void PWMTask::setFade(uint16_t fadeInTime, uint16_t fadeOutTime, uint8_t brightness) {
    mode = FADE;
    onTime = fadeInTime;
    offTime = fadeOutTime;
    this->brightness = brightness;
}

void PWMTask::setFlicker(uint16_t onTime, uint16_t offTime) {
    mode = FLICKER;
    this->onTime = onTime;
    this->offTime = offTime;
}

void PWMTask::turnOn() {
    mode = ON;
}

void PWMTask::turnOff() {
    mode = OFF;
}