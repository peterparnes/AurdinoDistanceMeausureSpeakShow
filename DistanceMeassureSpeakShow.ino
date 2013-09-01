// Parallax Ping: signal to 7 
// Emic: rx to 2 and tx to 3 
// NeoPixel bar signal to 8 
// plus some +5 and gnd :) 
//
// by Tovah Parnes (tovah@parnes.com) and Peter Parnes (peter@parnes.com)

#include <NewPing.h>  // https://code.google.com/p/arduino-new-ping/
#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>


#define TRIGGER_PIN  7  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     7  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
#define rxPin 2    // Serial input (connects to Emic 2 SOUT)
#define txPin 3    // Serial output (connects to Emic 2 SIN)
#define neoPin 8
#define numberPixels 8

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

// set up a new serial port
SoftwareSerial emicSerial =  SoftwareSerial(rxPin, txPin);

Adafruit_NeoPixel strip = Adafruit_NeoPixel(numberPixels, neoPin, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  emicSerial.begin(9600);
  
  /*
    When the Emic 2 powers on, it takes about 3 seconds for it to successfully
    intialize. It then sends a ":" character to indicate it's ready to accept
    commands. If the Emic 2 is already initialized, a CR will also cause it
    to send a ":"
  */
  emicSerial.print('\n');             // Send a CR in case the system is already up
  while (emicSerial.read() != ':');   // When the Emic 2 has initialized and is ready, it will send a single ':' character, so wait here until we receive it
  delay(10);                          // Short delay
  
  emicSerial.flush();                 // Flush the receive buffer
  emicSerial.print("N0");
  emicSerial.print("\n");
  emicSerial.print("V10");
  emicSerial.print("\n");
  
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  unsigned int uS = sonar.ping(); // Send ping, get ping time in microseconds (uS).
  Serial.print("Ping: ");
  Serial.print(uS / US_ROUNDTRIP_CM); // Convert ping time to distance in cm and print result (0 = outside set distance range)
  Serial.println("cm");
  sayDistance(uS / US_ROUNDTRIP_CM);
  showDistance(uS / US_ROUNDTRIP_CM);
  while (emicSerial.read() != ':'); 
}

void sayDistance(int dist) {
  emicSerial.print("S");
  emicSerial.print(dist);
  emicSerial.print("\n");
}

void showDistance(int dist) {
   for(uint16_t i=0; i<dist; i++) {
      strip.setPixelColor(i, strip.Color(0, 255, 0));
      strip.show();
   }
   for(uint16_t i=dist; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
      strip.show();
   }
   
}
