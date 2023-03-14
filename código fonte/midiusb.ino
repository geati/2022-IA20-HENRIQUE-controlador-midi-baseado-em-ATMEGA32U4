#include "MIDIUSB.h"

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

void setup() {
  Serial.begin(115200);
  for (int i = 2; i <= 8; i++){
    pinMode(i, OUTPUT);
  }
}

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}

int note[] = {0, 0, 60, 62, 64, 65, 67, 69, 71};
boolean stateBefore[] = {false, false, false, false, false, false, false, false, false};

boolean before = false;

void loop() {
//  boolean now = digitalRead(2) == HIGH;
//  if (before != now) {
//      now 
//         ? noteOn(0, 60, 64)
//         : noteOff(0, 60, 64);
//      MidiUSB.flush();    
//  }
//  before = now;

  for (int i = 2; i <= 8; i++){
    boolean noteState = digitalRead(i) == HIGH;
    if ((digitalRead(i) == HIGH) != stateBefore[i]) {

      if(noteState){
        noteOn(0, note[i], 64);  
        MidiUSB.flush();
        
      } else {
        noteOff(0, note[i], 64);  
        MidiUSB.flush();
        
      }
    }
    stateBefore[i] = noteState;
}
//  switch
//  for (int i = 2; i < 8; i++){
//    if (digitalRead(i) == HIGH) {
//      Serial.println("The state changed from LOW to HIGH");
//      
//      noteOn(0, 48, 64);   // Channel 0, middle C, normal velocity
//      MidiUSB.flush();
//      delay(150);
//     
//      noteOff(0, 48, 64);  // Channel 0, middle C, normal velocity
//      MidiUSB.flush();
//      delay(150);
//    } else
//  }

  
}
