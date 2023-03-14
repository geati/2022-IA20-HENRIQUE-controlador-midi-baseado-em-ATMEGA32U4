// Define Connections to 74HC165

// PL pin 1
int load = 7;
// CE pin 15
int clockEnablePin = 4;
// Q7 pin 7
int dataIn = 5;
// CP pin 2
int clockIn = 6;


void setup() {
  // put your setup code here, to run once:
  
  // Setup Serial Monitor
  Serial.begin(9600);

  // Setup 74HC165 connections
  pinMode(load, OUTPUT);
  pinMode(clockEnablePin, OUTPUT);
  pinMode(clockIn, OUTPUT);
  pinMode(dataIn, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

  // Write pulse to load pin
  digitalWrite(load, LOW);
  delayMicroseconds(5);
  digitalWrite(load, HIGH);
  delayMicroseconds(5);

  // Get data from 74HC165
  digitalWrite(clockIn, HIGH);
  digitalWrite(clockEnablePin, LOW);
  byte incoming = shiftIn(dataIn, clockIn, LSBFIRST);
  digitalWrite(clockEnablePin, HIGH);

  // Print to serial monitor
  Serial.print("Pin States:\r\n");
  Serial.println(incoming, BIN);
  delay(200);
  

}
