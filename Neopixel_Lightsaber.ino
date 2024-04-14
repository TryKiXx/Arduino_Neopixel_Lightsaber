/* 
  Arduino Neopixel Lightsaber

  Author: TryKIXx
  
  Version 1.3, 27.03.2024
*/

// Speicher-Funktion.
#include <EEPROM.h>
// DFPlayerMini.
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
// NeoPixel
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>  // Required for 16 MHz Adafruit Trinket
#endif

SoftwareSerial mySerial(3, 2);  // RX, TX für die Verbindung zum DFPlayer Mini.
DFRobotDFPlayerMini player;

#define KYBER_PIN 5     // Pin, an dem der Data-In des WS2812B angeschlossen ist
#define KYBER_PIXELS 1  // Anzahl der LEDs
Adafruit_NeoPixel kyber = Adafruit_NeoPixel(KYBER_PIXELS, KYBER_PIN, NEO_GRB + NEO_KHZ800);

#define BLADE_PIN 6      // Pin, an dem der Data-In des WS2812B angeschlossen ist
#define BLADE_PIXELS 41  // Anzahl der LEDs
Adafruit_NeoPixel blade = Adafruit_NeoPixel(BLADE_PIXELS, BLADE_PIN, NEO_GRB + NEO_KHZ800);

const int buttonPin = 4;
const int led1Pin = 13;

int led1State = LOW;

int buttonState;
int lastButtonState = LOW;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;
unsigned long oneSecPress = 1000;
unsigned long twoSecPress = 2000;    // NEU Dauer für den langen Tastendruck auf 2 Sekunden erhöht
unsigned long threeSecPress = 3000;  // NEU Dauer für den langen Tastendruck auf 2 Sekunden erhöht
unsigned long pressStartTime = 0;
int currentFunction = 0;

unsigned long previousMillis = 0;

int color = 1;
int sound = 1;
int volume = 1;

bool isPlaying = false;  // Zustand der Wiedergabe.
bool isBladeOn = false;

void setup() {
  // DFPlayer Wiedergabe.
  mySerial.begin(9600);
  player.begin(mySerial);
  delay(1000);

  player.EQ(DFPLAYER_EQ_BASS);
  player.volume(10);  // Lautstärke auf 25 setzen.
  player.play(1);     // Dritte MP3-Datei abspielen.

  // Textausgabe Monitor.
  Serial.begin(9600);
  Serial.println("Welcome!");
  Serial.println("Lightsaber v1.0");
  Serial.println("Power On");

  pinMode(buttonPin, INPUT);
  pinMode(led1Pin, OUTPUT);

  loadColor(0);  //Lese Color aus EEPROM @ address 0
  kyber.begin();
  blade.begin();
}

void save(int address, int value) {
  byte byte_1 = (value & 0xFF);
  byte byte_2 = ((value >> 8) & 0xFF);
  EEPROM.write(address, byte_1);
  EEPROM.write(address + 1, byte_2);

  Serial.print("Write to EEPROM (Address: ");
  Serial.print(address);
  Serial.print(",");
  Serial.print(address + 1);
  Serial.print("):");
  Serial.println(value);
}

int loadColor(int address) {
  long byte_1 = EEPROM.read(address);
  long byte_2 = EEPROM.read(address + 1);
  int val = (byte_2 << 8) | byte_1;

  Serial.print("Read from EEPROM (Address: ");
  Serial.print(address);
  Serial.print(",");
  Serial.print(address + 1);
  Serial.print("):");
  Serial.println(val);

  color = val;

  return val;
}

int loadSound(int address) {
  long byte_1 = EEPROM.read(address);
  long byte_2 = EEPROM.read(address + 1);
  int val = (byte_2 << 8) | byte_1;

  Serial.print("Read from EEPROM (Address: ");
  Serial.print(address);
  Serial.print(",");
  Serial.print(address + 1);
  Serial.print("):");
  Serial.println(val);

  sound = val;

  return val;
}

int loadVolume(int address) {
  long byte_1 = EEPROM.read(address);
  long byte_2 = EEPROM.read(address + 1);
  int val = (byte_2 << 8) | byte_1;

  Serial.print("Read from EEPROM (Address: ");
  Serial.print(address);
  Serial.print(",");
  Serial.print(address + 1);
  Serial.print("):");
  Serial.println(val);

  volume = val;

  return val;
}

// Funktion zum Einschalten des LED-Streifens
void bladeOn(uint32_t color, int interval) {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    for (int i = 0; i < BLADE_PIXELS; i++) {
      blade.setPixelColor(i, color);  // Setze die Farbe auf Weiß
      blade.show();
      delay(interval);  // Wartezeit zwischen den LEDs
    }
  }
}

// Funktion zum Ausschalten des LED-Streifens
void bladeOff(int interval) {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    for (int i = BLADE_PIXELS - 1; i >= 0; i--) {
      blade.setPixelColor(i, 0, 0, 0);  // Setze die Farbe auf Schwarz
      blade.show();
      delay(interval);  // Wartezeit zwischen den LEDs
    }
  }
}

void set_color() {
  if (color >= 7) {
    color = 0;
  }

  color++;

  save(0, color);  //Schreibe Color in EEPROM @ address 0
  switch (color) {
    case 1:
      Serial.println("Change Bladecolor.");
      Serial.println("Color: Green.");
      Serial.print("Current Bladecolor: ");
      Serial.println(color);
      break;
    case 2:
      Serial.println("Change Bladecolor.");
      Serial.println("Color: Blue.");
      Serial.print("Current Bladecolor: ");
      Serial.println(color);
      break;
    case 3:
      Serial.println("Change Bladecolor.");
      Serial.println("Color: Red.");
      Serial.print("Current Bladecolor: ");
      Serial.println(color);
      break;
    case 4:
      Serial.println("Change Bladecolor.");
      Serial.println("Color: Purple.");
      Serial.print("Current Bladecolor: ");
      Serial.println(color);
      break;
    case 5:
      Serial.println("Change Bladecolor.");
      Serial.println("Color: Yellow.");
      Serial.print("Current Bladecolor: ");
      Serial.println(color);
      break;
    case 6:
      Serial.println("Change Bladecolor.");
      Serial.println("Color: Orange.");
      Serial.print("Current Bladecolor: ");
      Serial.println(color);
      break;
    case 7:
      Serial.println("Change Bladecolor.");
      Serial.println("Color: White.");
      Serial.print("Current Bladecolor: ");
      Serial.println(color);
      break;
  }
}

void get_color() {
  switch (color) {
    case 1:  // Green
      bladeOn(blade.Color(0, 255, 0), 2);  // Green
      break;
    case 2:  // Blue
      bladeOn(blade.Color(0, 0, 255), 2);  // Blue
      break;
    case 3:  // Red
      bladeOn(blade.Color(255, 0, 0), 2);  // Red
      break;
    case 4:  // Purple
      bladeOn(blade.Color(255, 0, 255), 2);  // Purple
      break;
    case 5:  // Yellow
      bladeOn(blade.Color(255, 255, 0), 2);  // Yellow
      break;
    case 6:  // Orange
      bladeOn(blade.Color(255, 255, 255), 2);  // Orange
      break;
    case 7:  // White
      bladeOn(blade.Color(255, 255, 255), 2);  // White
      break;
  }
}

void get_kyber() {
  switch (color) {
    case 1:  // Green
      for (int i = 0; i < KYBER_PIXELS; i++) {
        int flicker = random(155, 255);
        int flicker2 = random(0, 15);
        kyber.setPixelColor(i, flicker2, flicker, flicker2);
      }
      kyber.show();
      delay(10);
      break;
    case 2:  // Blue
      for (int i = 0; i < KYBER_PIXELS; i++) {
        int flicker = random(200, 255);
        int flicker2 = random(0, 10);
        kyber.setPixelColor(i, flicker2, flicker2, flicker);
      }
      kyber.show();
      delay(20);
      break;
    case 3:  // Red
      for (int i = 0; i < KYBER_PIXELS; i++) {
        int flicker = random(200, 255);
        int flicker2 = random(0, 10);
        kyber.setPixelColor(i, flicker, flicker2, flicker2);
      }
      kyber.show();
      delay(30);
      break;
    case 4:  // Purple
      for (int i = 0; i < KYBER_PIXELS; i++) {
        int flicker = random(200, 255);
        int flicker2 = random(0, 10);
        kyber.setPixelColor(i, flicker, flicker2, flicker);
      }
      kyber.show();
      delay(40);
      break;
    case 5:  // Yellow
      for (int i = 0; i < KYBER_PIXELS; i++) {
        int flicker = random(200, 255);
        int flicker2 = random(0, 10);
        kyber.setPixelColor(i, flicker, flicker, flicker2);
      }
      kyber.show();
      delay(50);
      break;
    case 6:  // Orange
      for (int i = 0; i < KYBER_PIXELS; i++) {
        int flicker = random(200, 255);
        int flicker2 = random(0, 10);
        kyber.setPixelColor(i, flicker, 50, flicker2);
      }
      kyber.show();
      delay(60);
      break;
    case 7:  // White
      for (int i = 0; i < KYBER_PIXELS; i++) {
        int flicker = random(200, 255);
        int flicker2 = random(0, 10);
        kyber.setPixelColor(i, flicker, flicker, flicker);
      }
      kyber.show();
      delay(60);
      break;
  }
}

void get_flame() {
  switch (color) {
    case 1:  // Green
      for (int i = 0; i < BLADE_PIXELS; i++) {
        int flicker = random(155, 255);
        int flicker2 = random(0, 15);
        blade.setPixelColor(i, flicker2, flicker, flicker2);
      }
      blade.show();
      delay(10);
      break;
    case 2:  // Blue
      for (int i = 0; i < BLADE_PIXELS; i++) {
        int flicker = random(200, 255);
        int flicker2 = random(0, 10);
        blade.setPixelColor(i, flicker2, flicker2, flicker);
      }
      blade.show();
      delay(20);
      break;
    case 3:  // Red
      for (int i = 0; i < BLADE_PIXELS; i++) {
        int flicker = random(200, 255);
        int flicker2 = random(0, 10);
        blade.setPixelColor(i, flicker, flicker2, flicker2);
      }
      blade.show();
      delay(30);
      break;
    case 4:  // Purple
      for (int i = 0; i < BLADE_PIXELS; i++) {
        int flicker = random(200, 255);
        int flicker2 = random(0, 10);
        blade.setPixelColor(i, flicker, flicker2, flicker);
      }
      blade.show();
      delay(40);
      break;
    case 5:  // Yellow
      for (int i = 0; i < BLADE_PIXELS; i++) {
        int flicker = random(200, 255);
        int flicker2 = random(0, 10);
        blade.setPixelColor(i, flicker, flicker, flicker2);
      }
      blade.show();
      delay(50);
      break;
    case 6:  // Orange
      for (int i = 0; i < BLADE_PIXELS; i++) {
        int flicker = random(200, 255);
        int flicker2 = random(0, 10);
        blade.setPixelColor(i, flicker, 50, flicker2);
      }
      blade.show();
      delay(60);
      break;
    case 7:  // White
      for (int i = 0; i < BLADE_PIXELS; i++) {
        int flicker = random(200, 255);
        int flicker2 = random(0, 10);
        blade.setPixelColor(i, flicker, flicker, flicker);
      }
      blade.show();
      delay(60);
      break;
  }
}

void set_sound() {
  if (sound >= 2) {
    sound = 0;
  }

  sound++;

  save(2, sound);  //Schreibe Sound in EEPROM @ address 1

  switch (sound) {
    case 1:
      Serial.println("Change Sound.");
      Serial.println("Sound: Default.");
      Serial.print("Current Sound: ");
      Serial.println(sound);
      break;
    case 2:
      Serial.println("Change Sound.");
      Serial.println("Sound: Cal Kestis.");
      Serial.print("Current Sound: ");
      Serial.println(sound);
      break;
  }
}

void get_on_sound() {
  switch (sound) {
    case 1:
      player.play(2);
      break;
    case 2:
      player.play(4);
      break;
  }
}
void get_off_sound() {
  switch (sound) {
    case 1:
      player.play(3);
      break;
    case 2:
      player.play(5);
      break;
  }
}

void set_volume() {
  if (volume >= 7) {
    volume = 0;
  }

  volume++;

  save(4, volume);  //Schreibe volume in EEPROM @ address 2

  switch (volume) {
    case 1:
      Serial.println("Change Volume.");
      Serial.println("Volume: Stufe 1.");
      Serial.print("Current Volume: ");
      Serial.println(volume);
      break;
    case 2:
      Serial.println("Change Volume.");
      Serial.println("Volume: Stufe 2.");
      Serial.print("Current Volume: ");
      Serial.println(volume);
      break;
    case 3:
      Serial.println("Change Volume.");
      Serial.println("Volume: Stufe 3.");
      Serial.print("Current Volume: ");
      Serial.println(volume);
      break;
    case 4:
      Serial.println("Change Volume.");
      Serial.println("Volume: Stumm.");
      Serial.print("Current Volume: ");
      Serial.println(volume);
      break;
  }
}

void get_volume() {
  switch (volume) {
    case 1:
      player.volume(5);  // Volume 1
      break;
    case 2:
      player.volume(10);  // Volume 2
      break;
    case 3:
      player.volume(15);  // Volume 3
      break;
    case 4:
      player.volume(20);  // Volume 4
      break;
    case 5:
      player.volume(25);  // Volume 5
      break;
    case 6:
      player.volume(30);  // Volume 6
      break;
    case 7:
      player.volume(0);  // Volume 7
      break;
  }
}

void loop() {
  get_kyber();

  int reading = digitalRead(buttonPin);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == HIGH) {
        pressStartTime = millis();
      } else {
        long pressDuration = millis() - pressStartTime;

        if (pressDuration < oneSecPress) {
          if (led1State == LOW) {
            led1State = HIGH;
            Serial.println("Blade On.");

            loadColor(0);   //Lese Color aus EEPROM @ address 0
            loadSound(2);   //Lese Sound aus EEPROM @ address 2
            loadVolume(4);  //Lese Volume aus EEPROM @ address 4
            get_color();
            get_volume();
            get_on_sound();
            isPlaying = true;
            isBladeOn = true;
          } else {
            led1State = LOW;
            Serial.println("Blade Off.");
            get_off_sound();
            isPlaying = false;
            bladeOff(25);
            isBladeOn = false;
          }
        } else {
          if (led1State == HIGH) {
            if (pressDuration >= threeSecPress) {
              currentFunction = 3;  // set_volume.
            } else if (pressDuration >= twoSecPress) {
              currentFunction = 2;  // set_sound.
            } else {
              currentFunction = 1;  // set_color.
            }
          }
        }
      }
    }
  }

  if (isBladeOn) {
    get_flame();
  }

  digitalWrite(led1Pin, led1State);

  // Jetzt führe die entsprechende Funktion basierend auf currentFunction aus.
  switch (currentFunction) {
    case 1:
      set_color();
      get_color();
      break;
    case 2:
      set_sound();
      get_on_sound();
      break;
    case 3:
      set_volume();
      get_volume();
      break;
    default:
      // Keine Funktion ausführen.
      break;
  }

  currentFunction = 0;  //Zurücksetzen auf keine Funktion nach der Ausführung.
  lastButtonState = reading;
}
