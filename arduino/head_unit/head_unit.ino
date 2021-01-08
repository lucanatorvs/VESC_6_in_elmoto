/**
   Author:          Luca van Straaten (based in part on the VESC_LCD_EBIKE project by Tom Stanton)
   Created:         22.09.2020
   Compiler:        Arduino 1.8.12
   ISO/IEC 14882:   arduino c
   File:            head_unit.ino
   discription:     a control system for the head unit of a eBike
**/

#include <SoftwareSerial.h>

#include <VescUart.h>
#include <SimpleKalmanFilter.h>

//Library for the Display
#include <Wire.h>
#include "rgb_lcd.h"

rgb_lcd lcd;

/** Initiate VescUart class */
VescUart UART;

int rpm;
float voltage;
float current;
int power;
float amphour;
float tach;
float distance;
float velocity;
float watthour;
float batpercentage;

SimpleKalmanFilter Filter1(2, 2, 0.01);

SoftwareSerial softSerial(10, 11); // RX, TXe

////////// switches //////////

const int Lbrake = 3;
const int Rbrake = 4;
const int light = 5;
const int horn = 2;

bool lastlight = LOW;

float powerfiltered;


////////// Custom Characters //////////
byte ph[] = {
  B11100,
  B10100,
  B11100,
  B10000,
  B10101,
  B00111,
  B00101,
  B00101
};

byte mi[] = {
  B10001,
  B11011,
  B10101,
  B10001,
  B00100,
  B00000,
  B00100,
  B00110
};

byte km[] = {
  B01000,
  B01010,
  B01100,
  B01010,
  B10001,
  B11011,
  B10101,
  B10001
};


byte ah[] = {
  B00100,
  B01010,
  B01110,
  B01010,
  B00000,
  B01010,
  B01110,
  B01010
};
byte percent[] = {
  B11001,
  B11001,
  B00010,
  B00100,
  B01000,
  B10011,
  B10011,
  B00000
};

byte m[] = {
  B10001,
  B11011,
  B10101,
  B10001,
  B10001,
  B00000,
  B00000,
  B00000
};

byte k[] = {
  B01000,
  B01010,
  B01100,
  B01010,
  B01010,
  B00000,
  B00000,
  B00000
};

byte bat[] = {
  B01110,
  B11011,
  B10001,
  B10001,
  B10001,
  B11111,
  B11111,
  B11111
};

byte sprk[] = {
  B00000,
  B01000,
  B00100,
  B00010,
  B00100,
  B01000,
  B00100,
  B00010
};

boolean array_cmp(char *a, char *b, int len_a, int len_b) {
  int n;

  // if their lengths are different, return false
  if (len_a != len_b) return false;

  // test each element to be the same. if not, return false
  for (n = 0; n < len_a; n++) if (a[n] != b[n]) return false;

  //ok, if we have not returned yet, they are equal :)
  return true;
}

void getdataandwritetolcd () {
  ////////// Filter //////////
  // calculate the estimated value with Kalman Filter
  powerfiltered = Filter1.updateEstimate(power);

  ////////// Read values //////////
  if ( UART.getVescValues() ) {

    rpm = (UART.data.rpm) / 27;        // The '27' is the number of pole pairs in the motor.
    voltage = (UART.data.inpVoltage);
    current = (UART.data.avgInputCurrent);
    power = voltage * current;
    amphour = (UART.data.ampHours);
    watthour = amphour * voltage;
    tach = (UART.data.tachometerAbs) / 162;        // The '162' is the number of motor poles multiplied by 3
    distance = ((tach * 3.142) / 1000) * 0.64; // (Motor RPM x Pi) / meters in a  km) x Wheel diameter
    velocity = ((rpm * 3.142 * 60) / 1000) * 0.64; // ((Motor RPM x Pi x seconds in a minute) / meters in a km) x Wheel diameter
    batpercentage = ((voltage - 42) / 13) * 100;    // ((Battery voltage - minimum voltage) / number of cells) x 100


    if (velocity < 0.5 && velocity > -0.5) {
      velocity = 0;
    }

    ////////// LCD //////////

    // First line
    if (velocity < 10 && velocity >= 0) {
      lcd.setCursor(0, 0);
      lcd.print("  ");
      lcd.print(velocity, 0);
    }
    else if (velocity < 100 && velocity >= 10 || velocity > -10 && velocity < 0) {
      lcd.setCursor(0, 0);
      lcd.print(" ");
      lcd.print(velocity, 0);
    }
    else if (velocity < 10 && velocity >= 0 || velocity > -100 && velocity <= 10) {
      lcd.setCursor(0, 0);
      lcd.print(velocity, 0);
    }
    lcd.createChar(3, k);          // Change 'm' to 'k' for kilometers
    lcd.setCursor(3, 0);
    lcd.write(3);
    lcd.createChar(4, ph);
    lcd.setCursor(4, 0);
    lcd.write(4);

    lcd.setCursor(7, 0);
    lcd.print(voltage, 0);
    lcd.setCursor(9, 0);
    lcd.print("V");

    lcd.setCursor(11, 0);
    lcd.print(distance, 2);
    lcd.createChar(5, km);          // Change 'mi' to 'km' for kilometers
    lcd.setCursor(15, 0);
    lcd.write(5);

    // Second line
    if (powerfiltered < 10 && powerfiltered >= 0) {
      lcd.setCursor(0, 1);
      lcd.print("   ");
      lcd.print(powerfiltered, 0);
    }
    else if (powerfiltered >= 10 && powerfiltered < 100 || powerfiltered > -10 && powerfiltered < 0) {
      lcd.setCursor(0, 1);
      lcd.print("  ");
      lcd.print(powerfiltered, 0);
    }
    else if (powerfiltered >= 100 && powerfiltered < 1000 || powerfiltered > -100 && powerfiltered <= -10) {
      lcd.setCursor(0, 1);
      lcd.print(" ");
      lcd.print(powerfiltered, 0);
    }
    else if (powerfiltered >= 1000 || powerfiltered > -1000 && powerfiltered <= -100) {
      lcd.setCursor(0, 1);
      lcd.print(powerfiltered, 0);
    }

    lcd.setCursor(4, 1);
    lcd.print("W");

    if (batpercentage < 100) {
      lcd.setCursor(7, 1);
      lcd.print(batpercentage, 0);
    }
    else {
      lcd.setCursor(6, 1);
      lcd.print(batpercentage, 0);
    }
    lcd.createChar(6, percent);
    lcd.setCursor(9, 1);
    lcd.write(6);

    lcd.setCursor(11, 1);
    lcd.print(amphour, 2);
    lcd.createChar(7, ah);
    lcd.setCursor(15, 1);
    lcd.write(7);

    lcd.println();

  }
  else {
    lcd.println("Failed to get data!");
  }
}





void setup() {

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  lcd.setCursor(3, 0);
  lcd.print("LUCA EBIKE");          // Startup screen (can't be more than 10 characters)
  lcd.createChar(1, bat);
  lcd.setCursor(1, 1);
  lcd.write(1);
  lcd.setCursor(14, 1);
  lcd.write(1);
  lcd.createChar(2, sprk);
  lcd.setCursor(1, 0);
  lcd.write(2);
  lcd.setCursor(14, 0);
  lcd.write(2);

  pinMode(Lbrake, INPUT);
  pinMode(Rbrake, INPUT);
  pinMode(light, INPUT_PULLUP);
  pinMode(horn, INPUT_PULLUP);

  /** Setup Serial port to display data */
  Serial.begin(115200);

  /** Setup UART port */
  softSerial.begin(9600);

  while (!Serial) {
    ;
  }

  /** Define which ports to use as UART */
  UART.setSerialPort(&Serial);

  delay(5000);
  lcd.clear();

  pinMode(LED_BUILTIN, OUTPUT);

}






long loop_timer;

int loop_counter = 0;
byte totransmit = 0;
byte latsransmit;

void loop() {
  loop_timer = millis() + 200; //5hz update rate.
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW

  if (loop_counter >= 2) {
    loop_counter = 0;
    getdataandwritetolcd();
  }
  loop_counter++;

  // stuff to do with the buttons and the serial comunication
  bool readingLbrake = digitalRead(Lbrake);
  bool readingRbrake = digitalRead(Rbrake);
  bool readinglight = digitalRead(light);
  bool readinghorn = digitalRead(horn);

  char str[4];

  if (readingLbrake == LOW || powerfiltered < -100) {
    str[0] = 1;
  } else {
    str[0] = 0;
  }

  if (readingRbrake == LOW) {
    str[1] = 1;
  } else {
    str[1] = 0;
  }

  if (readinglight == LOW && lastlight == HIGH) {
    str[2] = 1;
  } else {
    str[2] = 0;
  }

  if (readinghorn == LOW) {
    str[3] = 1;
  } else {
    str[3] = 0;
  }

  totransmit = 0;

  for (int i = 0; i < 4; i++) {
    if (str[i] == 1) {
      bitSet(totransmit, i);
    }
  }

  //totransmit = 5;
  if (totransmit != latsransmit) {
    softSerial.write(totransmit);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(20);
  }

  latsransmit = totransmit;

  lastlight = readinglight;

  delay(20);

  //while (millis() - loop_timer > 0) {
  //}
}
