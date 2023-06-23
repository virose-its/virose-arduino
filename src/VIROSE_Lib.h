/*
  VIROSE.h - Library for the All Sensors in the VIROSE Robot Main Board
  Created by Faris and the VIROSE Team, 2022
  Released into the virose github private repository
*/

#ifndef VIROSE_h
#define VIROSE_h

#include "Arduino.h"

#define SAMPLE_COUNT 1200  // Jumlah sampel yang digunakan untuk filter moving average.

enum LEDState {
    FADE,
    FLICKER,
    ON,
    OFF_AFTER,
    OFF
};

enum MODE {
    START = 51,
    STOP,
    RESUME,
    PAUSE,
    RESET,
    TORQUE,
    REQUEST_INDEX,
    RECEIVE_INDEX,
    INDEX_CHANGED,
};

enum SERVO {
    MOTION_DONE = 71,
    XL_TOTAL_JOINT,
    MX_TOTAL_JOINT,
    ON_OFF_TORQUE,
    PLAY_MOTION,
    STOP_MOTION,
    RESUME_MOTION,
    MOTION_SERIES,
    MOTION_SKIP
};

/* CLASS PROTOTYPE */
class Sound {
   public:
    /**
     * @brief Konstruktor untuk kelas Sound.
     *
     * @param pin Pin yang terhubung ke sensor suara.
     * @param vRef Nilai referensi tegangan (dalam Volt) yang digunakan oleh ADC.
     *             Biasanya bernilai 3.3V atau 5V.
     */
    Sound(uint8_t, float);

    /**
     * @brief Fungsi untuk membaca nilai tegangan (dalam Volt) yang dihasilkan oleh sensor suara.
     *
     * @return Nilai tegangan yang dihasilkan oleh sensor suara.
     */
    float readVoltage();

    /**
     * @brief Fungsi untuk membaca nilai tegangan yang telah disaring dengan filter moving average.
     *
     * @return Nilai tegangan yang telah disaring dengan filter moving average.
     */
    float getFilteredVoltage();

    /**
     * @brief Fungsi untuk membaca nilai digital yang dihasilkan oleh sensor suara.
     *
     * @return Nilai digital yang dihasilkan oleh sensor suara.
     */
    uint8_t getDigitalValue();

   private:
    uint8_t pin;                  // Pin yang terhubung ke sensor suara.
    float vRef;                   // Nilai referensi tegangan yang digunakan oleh ADC.
    uint16_t adcRes;              // Resolusi ADC (dalam bit).
    float adcMax;                 // Nilai maksimum yang bisa dibaca oleh ADC.
    float samples[SAMPLE_COUNT];  // Sampel-sampel tegangan yang digunakan untuk filter moving average.
    int index;                    // Indeks sampel yang sedang aktif.

    /**
     * Fungsi untuk mengupdate nilai sampel-sampel tegangan.
     *
     * @param voltage Nilai tegangan yang baru.
     */
    void updateSamples(float);
};

class PWMTask {
   public:
    /**
     * Konstruktor untuk kelas PWMTask.
     *
     * @param pin pin yang digunakan sebagai output PWM.
     * @param pwmChannel pwmChannel Channel PWM yang digunakan untuk mengontrol transistor.
     */
    PWMTask(uint8_t pin, uint8_t pwmChannel);

    /**
     * @brief Fungsi untuk mengaktifkan komponen dengan mode fading.
     *
     * @param fadeInTime Durasi (dalam milidetik) untuk menyala semakin terang.
     * @param fadeOutTime Durasi (dalam milidetik) untuk padam semakin redup.
     * @param brightness Nilai maksimum brightness yang diinginkan.
     */
    void setFade(uint16_t fadeInTime, uint16_t fadeOutTime, uint8_t brightness);

    /**
     * Fungsi untuk mengaktifkan komponen dengan mode flicker.
     *
     * @param onTime Durasi (dalam milidetik) untuk menyala.
     * @param offTime Durasi (dalam milidetik) untuk padam.
     */
    void setFlicker(uint16_t onTime, uint16_t offTime);

    /**
     * @brief Fungsi untuk menyalakan komponen.
     */
    void turnOn();

    /**
     * @brief Fungsi untuk menyalakan komponen sesaat
     *
     * @param onTime Durasi (dalam milidetik) untuk menyala.
     */
    void turnOffAfter(uint16_t onTime);

    /**
     * @brief Fungsi untuk mematikan komponen.
     */
    void turnOff();

   private:
    uint8_t pin;
    uint8_t pwmChannel;
    uint8_t mode = LEDState::ON;
    uint8_t brightness = 255;
    uint16_t onTime = 100;
    uint16_t offTime = 100;

    // FreeRTOS task untuk mengontrol PWM.
    void startTask();
    void Task(void *pvParameters);
};

#endif