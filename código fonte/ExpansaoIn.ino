#include "MIDIUSB.h"

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}

int alturaOitava = 2;
const int notasPorOitava = 13;

int oitavas[][notasPorOitava] = {
  {48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60},
  {60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72}, 
  {72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84}, 
  {84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96},
  {96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108}
};

/***********************************************************************\
   Programa Expansão de Entradas do Arduino, utilizando Shift Register
   CI utilizado: 74HC165
   Por: João Sérgio Caciola  -  Eletrônica Projetos e Treinamentos
   Data: 09/04/2017
   Créditos: Baseado no playground.arduino.cc
  \***********************************************************************/

// Definições de Labels
#define nCIs  2               //Registra o número de CIs cascateados
#define nExtIn  nCIs * 8      //Registra o número de novas entradas externas
#define TempoDeslocamento 50  //Registra o tempo de queverá ter o pulso para leitura e gravação, (milesegundos)
#define Atraso  200           //Registra o atraso de segurança entre leituras, (milesegundos)
#define BYTES_VAL_T unsigned int  //Altera de int para long, se o a quantidade de CIs cascateados for maior que 2


// Declaração de variáveis globais
const int ploadPin        = 7;    //Conecta ao pino 1 do 74HC165 (LH/LD - asynchronous parallel load input)(PL)
const int clockEnablePin  = 4;    //Conecta ao pino 15 do 74HC165 (CE - Clock Enable Input)(CE)
const int dataPin         = 5;   //Conecta ao pino 9 do 74HC165 (Q7 - serial output from the last stage)(Q7)
const int clockPin        = 6;   //Conecta ao pino 2 do 74HC165 (CP - Clock Input)(CP)

BYTES_VAL_T pinValues;
BYTES_VAL_T oldPinValues;


//Função para definir um rotina shift-in, lê os dados do 74HC165
BYTES_VAL_T read_shift_regs()
{
  long bitVal = 0;
  BYTES_VAL_T bytesVal = 0;

  digitalWrite(clockEnablePin, HIGH);
  digitalWrite(ploadPin, LOW);
  delayMicroseconds(TempoDeslocamento);
  digitalWrite(ploadPin, HIGH);
  digitalWrite(clockEnablePin, LOW);

  // Efetua a leitura de um bit da saida serial do 74HC165
  for (int i = 0; i < nExtIn; i++)
  {
    bitVal = digitalRead(dataPin);

    //Realiza um shift left e armazena o bit correspondente em bytesVal
    bytesVal |= (bitVal << ((nExtIn - 1) - i));

    //Lança um pulso de clock e desloca o próximo bit
    digitalWrite(clockPin, HIGH);
    delayMicroseconds(TempoDeslocamento);
    digitalWrite(clockPin, LOW);
  }

  return (bytesVal);
}

//Mostra os dados recebidos
void display_pin_values()
{
  Serial.print("Estado das entradas:\r\n");

  for (int i = 0; i < nExtIn; i++)
  {
    Serial.print("  Pin0-");
    Serial.print(i);
    Serial.print(": ");

    if ((pinValues >> i) & 1) {
      Serial.print("ALTO");

      //Diminui uma oitava
      if (i == 13) {
        alturaOitava = alturaOitava > 0 ? alturaOitava - 1 : 0;
      }

      //Aumenta uma oitava
      if (i == 14) {
        alturaOitava = alturaOitava < sizeof(oitavas)/sizeof(oitavas[0]) - 1 ? alturaOitava + 1 : alturaOitava;
      }
      
      if (i <= 12) {
        noteOn(0, oitavas[alturaOitava][i], 64);
        MidiUSB.flush();
      }

    } else {
      Serial.print("BAIXO");
      if (i <= 12) {
        noteOff(0, oitavas[alturaOitava][i], 64);
        MidiUSB.flush();
      }
    }
    Serial.print("\r\n");
  }

  Serial.print("\r\n");
}

// Configuração do Programa
void setup()
{
  //Avita a comunicação via monitor serial
  Serial.begin(9600);

  //Inicializa e configura os pinos
  pinMode(ploadPin, OUTPUT);
  pinMode(clockEnablePin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, INPUT);

  digitalWrite(clockPin, HIGH);
  digitalWrite(ploadPin, HIGH);

  //Lê e mostra o estado dos pinos
  pinValues = read_shift_regs();
  display_pin_values();
  oldPinValues = pinValues;

}

//Função do loop principal
void loop()
{
  //Lê todos as portas externas
  pinValues = read_shift_regs();

  //Se houver modificação no estado dos pinos, mostra o estado atual
  if (pinValues != oldPinValues)
  {
    Serial.print("*Alteracao detectada*\r\n");
    display_pin_values();
    oldPinValues = pinValues;
  }

  delay(Atraso);
}
