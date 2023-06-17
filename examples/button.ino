#include "VIROSE.h"

// Membuat objek Button pada pin 2 dengan resistor pullup.
Button button(2);

void setup() {
  // Inisialisasi serial.
  Serial.begin(9600);
}

void loop() {
  // Memeriksa apakah tombol ditekan atau tidak.
  if (button.isPressed()) {
    Serial.println("Button is pressed!");
  } else {
    Serial.println("Button is not pressed.");
  }
}
