#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <TimerOne.h>

#define vMax 1023

#define CE_PIN   9
#define CSN_PIN 10

const byte slaveAddress[5] = {'R', 'x', 'A', 'A', 'A'};


RF24 radio(CE_PIN, CSN_PIN); // Create a Radio

char dataToSend[10];

unsigned long txIntervalMillis = 250 ; // TODO: Reduce data interval
unsigned long mils = 0;

//DESCRIPTIONS
#define COLOR_POTE 'P'
#define POT_X 'X'
#define POT_Y 'Y'
#define BOT_OFF 'F'
#define BOT_ON 'O'

//LEDS
#define NUM_X 30
#define NUM_Y 10

//PINES
#define POT_COLOR A0
#define JOYSTICK_X_PIN A1
#define JOYSTICK_Y_PIN A2
#define BOTON_PRENDER 5
#define BOTON_APAGAR 6

//Mov States
#define SUMA 0
#define RESTA 0
#define SUMA_CONT 1
#define RESTA_CONT 1

//MOVEMENT VARS
#define CONT_DELAY 450
int x_pos;
int y_pos;
int x;
int y;
int estadoResta_X = RESTA;
int estadoSuma_X = SUMA;
int estadoResta_Y = RESTA;
int estadoSuma_Y  = SUMA;
bool contState_X;
bool contState_Y;
int cont_X = 0;
int cont_Y = 0;
int auxCont_X = false;
int auxCont_Y = false;
int posAnterior;

//cursor vals
#define TIMEOUT 2
int cursorState = false;
int contCursor = 0;

//Button vars
#define BOTON_PRESIONADO 1
#define BOTON_LIBERADO 2
#define BOTON_CONFIRMACION 3
int estadoBoton = BOTON_LIBERADO;
bool estadoContadorBoton = false;
int msBoton = 0;



void timer() {
  if (contState_X) {
    cont_X = cont_X + 1;
  }
  if (contState_Y) {
    cont_Y = cont_Y + 1;
  }
  contCursor = contCursor + 1;
  if (estadoContadorBoton) {
    msBoton++;
  }
  mils = mils + 1;
  if (mils >= txIntervalMillis ) {
    processData(analogRead(A0), COLOR_POTE); send();
    processData(x_pos, POT_X); send();
    processData(y_pos, POT_Y); send();
    mils = 0;
  }
}


void setup() {
  pinMode(BOTON_PRENDER, INPUT_PULLUP);
  pinMode(BOTON_APAGAR, INPUT_PULLUP);
  Serial.begin(9600);

  Serial.println("Starting RF_Module");

  radio.begin();
  radio.setDataRate( RF24_250KBPS );
  radio.setRetries(3, 5); // delay, count
  radio.openWritingPipe(slaveAddress);

  Timer1.initialize(1000);
  Timer1.attachInterrupt(timer);
}

//===================================================================================================================================

void loop() {
  movementX();
  Serial.print("X= ");
  Serial.println(x);
  movementY();

}

//===================================================================================================================================

void send() {

  bool rslt;
  rslt = radio.write( &dataToSend, sizeof(dataToSend) );

  Serial.print("Data Sent ");
  Serial.print(dataToSend);

  if (rslt) {
    Serial.println("  Acknowledge received");
  }
  else {
    Serial.println("  Tx failed");
  }
}

//===================================================================================================================================

void processData(int numero, char type) {
  char charVal[10]; String numStr;
  numStr = type + String(numero);
  numStr.toCharArray(charVal, 10);

  for (int i = 0; i < sizeof(dataToSend); i++)
  {
    dataToSend[i] = charVal[i];
  }
}

//===================================================================================================================================

void movementX() {
  x = analogRead(JOYSTICK_X_PIN);

  if (x >= (vMax * 0.7) && x_pos != (NUM_X - 1) / 2) {
    switch (estadoSuma_X) {
      case SUMA:
        x_pos = x_pos + 1;
        estadoSuma_X = SUMA_CONT;
        contState_X = true;
        return;
      case SUMA_CONT:
        if (cont_X >= CONT_DELAY && !auxCont_X) {
          cont_X = 0;
          x_pos = x_pos + 1;
          auxCont_X = true;
        }
        if (cont_X >= CONT_DELAY / 2 && auxCont_X) {
          cont_X = 0;
          x_pos = x_pos + 1;
        }
        return;
    }

  } else if (x <= (vMax * 0.3) && x_pos != 0) {
    switch (estadoResta_X) {
      case RESTA:
        x_pos = x_pos - 1;
        estadoResta_X = RESTA_CONT;
        contState_X = true;
        return;
      case RESTA_CONT:
        if (cont_X >= CONT_DELAY && !auxCont_X) {
          cont_X = 0;
          x_pos = x_pos - 1;
          auxCont_X = true;
        }
        if (cont_X >= CONT_DELAY / 2 && auxCont_X) {
          cont_X = 0;
          x_pos = x_pos - 1;
        }
        return;
    }
  } else {
    estadoSuma_X = SUMA;
    estadoResta_X = RESTA;
    contState_X = false;
    auxCont_X = false;
    cont_X = 0;
  }
}

//===================================================================================================================================

void movementY() {
  y = analogRead(JOYSTICK_Y_PIN);

  if (y >= (vMax * 0.8) && y_pos != NUM_Y - 1) {
    switch (estadoSuma_Y) {
      case SUMA:
        y_pos = y_pos + 1;
        estadoSuma_Y = SUMA_CONT;
        contState_Y = true;
        return;
      case SUMA_CONT:
        if (cont_Y >= CONT_DELAY && !auxCont_Y) {
          cont_Y = 0;
          y_pos = y_pos + 1;
          auxCont_Y = true;
        }
        if (cont_Y >= CONT_DELAY / 2 && auxCont_Y) {
          cont_Y = 0;
          y_pos = y_pos + 1;
        }
        return;
    }

  } else if ( y <= (vMax * 0.3) &&  y_pos != 0) {
    switch (estadoResta_Y) {
      case RESTA:
        y_pos = y_pos - 1;
        estadoResta_Y = RESTA_CONT;
        contState_Y = true;
        return;
      case RESTA_CONT:
        if (cont_Y >= CONT_DELAY && !auxCont_Y) {
          cont_Y = 0;
          y_pos = y_pos - 1;
          auxCont_Y = true;
        }
        if (cont_Y >= CONT_DELAY / 2 && auxCont_Y) {
          cont_Y = 0;
          y_pos = y_pos - 1;
        }
        return;
    }
  } else {
    estadoSuma_Y = SUMA;
    estadoResta_Y = RESTA;
    contState_Y = false;
    auxCont_Y = false;
    cont_Y = 0;
  }
}

//===================================================================================================================================

bool maquinaBoton(int boton) {
  switch (estadoBoton) {
    case BOTON_LIBERADO:
      if (digitalRead(boton) == LOW) {
        estadoBoton = BOTON_CONFIRMACION;
        estadoContadorBoton = true;
      } else {
        return false;
      }
      break;
    case BOTON_CONFIRMACION:
      if (msBoton > 15) {
        if (digitalRead(boton) == HIGH) {
          estadoBoton = BOTON_LIBERADO;
          estadoContadorBoton = false;
          msBoton = 0;
        } else {
          estadoBoton =  BOTON_PRESIONADO;
          msBoton = 0;
          estadoContadorBoton = false;
          return false;
        }
      }
      break;
    case BOTON_PRESIONADO:
      if (digitalRead(boton) == HIGH) {
        estadoBoton = BOTON_LIBERADO;
        return true;
      }
      break;
  }
}
