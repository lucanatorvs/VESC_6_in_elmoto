#include<Wire.h>
#include <SoftwareSerial.h>

const int mainerelay = 2;
const int brakelight = 6;
const int horn = 3;
const int headlight = 4;
const int taillight = 5;


bool lights = HIGH;
bool lastlight;

char str[3];

byte inbyte;


SoftwareSerial softSerial(19, 18); // RX, TX


void setup() {


  pinMode(headlight, OUTPUT);
  pinMode(taillight, OUTPUT);
  pinMode(horn, OUTPUT);
  pinMode(brakelight, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(mainerelay, OUTPUT);

  digitalWrite(horn, HIGH);
  digitalWrite(brakelight, HIGH);
  digitalWrite(headlight, HIGH);
  digitalWrite(taillight, HIGH);

  Serial.begin(9600);

  softSerial.begin(9600);
  softSerial.listen();

  delay(6000);

  digitalWrite(mainerelay, HIGH);

  for (int x = 0; x < 4; x++) {
    str[x] = 0;
  }

  Serial.println("test1");
}



long loop_timer;

void loop() {

  if (softSerial.available()) {
    inbyte = softSerial.read();
    Serial.println(inbyte);
  }

  for (int i = 0; i < 4; i++) {
    if (bitRead(inbyte, i)){
      str[i] = 1;
    }
    
  }

  // head light
  if (bitRead(inbyte, 2) == 1 && lastlight == 0) {
    lights = !lights;
  }

  digitalWrite(headlight, lights);
  digitalWrite(taillight, lights);

  lastlight = bitRead(inbyte, 2);

  // horn
  digitalWrite(horn, !bitRead(inbyte, 3));

  //brake light
  if (bitRead(inbyte, 0) || bitRead(inbyte, 1)) {
    digitalWrite(brakelight, LOW);
  } else {
    digitalWrite(brakelight, HIGH);
  }

  delay(20);
}
