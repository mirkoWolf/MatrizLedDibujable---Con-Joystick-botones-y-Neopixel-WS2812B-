#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

//PINES
#define LED_PIN 18
#define JOYSTICK_X_PIN 15
#define JOYSTICK_Y_PIN 2
#define POT_PIN 4
#define BOTON_PRENDER 5

//Timer Vals
#define DELAYVAL 1000
hw_timer_t *My_timer = NULL;

//States
#define ON 1
#define OFF 0
#define PREFAB 2
int estado = ON;

//Mov States
#define SUMA 0
#define SUMA_CONT 1
#define RESTA 0
#define RESTA_CONT 1

//LED vars
#define NUMPIXELS 300 //NO CAMBIAR
#define NUM_X 30
#define NUM_Y 10
bool ledState[NUM_Y][NUM_X];
int color[NUM_Y][NUM_X];
#define COLOR_MAX 65000

//movement vars
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

void IRAM_ATTR onTimer() {

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
  if (false) {
    Serial.println(String("[") + x_pos + String("][") + y_pos + String("]"));
  }
}

Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif

  pinMode(BOTON_PRENDER, INPUT_PULLUP);
  Serial.begin(115200);
  pixels.begin(); // IMPORTANTE

  My_timer = timerBegin(0, 80, true);
  timerAttachInterrupt(My_timer, &onTimer, true);
  timerAlarmWrite(My_timer, DELAYVAL, true);
  timerAlarmEnable(My_timer); //Just Enable
}

void loop() {
  checkEstado();
  movementX();
  movementY();
  cursor();

  pixels.show();   // IMPORTANTE
}

void checkEstado() {
  switch (estado) {
    case ON:
      int i;
      int a;

      for (i = 0; i < NUM_Y; i++) {
        for (a = 0; a < NUM_X; a++) {
          if (ledState[i][a]) {
            pixels.setPixelColor(i * NUM_X + a, pixels.ColorHSV(color[i][a], 255, 255));
          } else {
            pixels.setPixelColor(i * NUM_X + a, 0, 0, 0);
          }
        }
      }

      return;

    case OFF:
      pixels.clear();
      for (i = 0; i < NUM_Y; i++) {
        for (a = 0; a < NUM_X; a++) {
          pixels.setPixelColor(i * NUM_X + a, pixels.ColorHSV(0, 0, 0));
          ledState[i][a] = false;
        }
      }
      return;

    case PREFAB:

      return;
  }
}
void movementX() {
  x = analogRead(JOYSTICK_X_PIN);

  if (x >= 3200 && x_pos != (NUM_X - 1) / 2) {
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

  } else if (x <= 700 && x_pos != 0) {
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
void movementY() {
  y = analogRead(JOYSTICK_Y_PIN);

  if (y >= 3200 && y_pos != NUM_Y - 1) {
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

  } else if ( y <= 700 &&  y_pos != 0) {
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

void cursor() {
  int valMap = map(analogRead(POT_PIN), 0, 4095, 0, COLOR_MAX);
  int posActual = y_pos * NUM_X + x_pos * 2;


  if (maquinaBoton(BOTON_PRENDER)) {
      ledState[y_pos][x_pos * 2 ] = true;
      ledState[y_pos][x_pos * 2 + 1] = true;
      color[y_pos][x_pos * 2] = valMap;
      color[y_pos][x_pos * 2 + 1] = valMap;
  }
  switch (cursorState) {
    case ON:
      pixels.setPixelColor( posActual , pixels.ColorHSV(valMap));
      pixels.setPixelColor( posActual + 1, pixels.ColorHSV(valMap));

      if (contCursor >= 500) {
        cursorState = OFF;
        contCursor = 0;
      }
      break;
    case OFF:
      pixels.setPixelColor( posActual, 0, 0, 0);
      pixels.setPixelColor( posActual + 1, 0, 0, 0);

      if (contCursor >= 250) {
        cursorState = ON;
        contCursor = 0;
      }
      break;
    case TIMEOUT: // TODO
      pixels.setPixelColor( posActual , pixels.ColorHSV(valMap));
      pixels.setPixelColor( posActual + 1, pixels.ColorHSV(valMap));

      if (contCursor >= 1000) {
        cursorState = OFF;
        contCursor = 0;
      }
      break;
  }
}

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
