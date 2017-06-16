//PERSISTENCE OF VISION PROJECT
//AUTHOR: JESÚS VICENTE PINPANPLOT@GMAIL.COM 2017

/////////////////////////
//CONNECTIONS:
//////////////////////////
//HALL SENSOR IN D2 (RESISTOR 10K BETWEEN SIGNAL AND 5V)
//ARDUINO NANO AND UNO: APA102 (or similar) 'DATA' TO MOSI (D11) AND 'CLOCK' TO SCK (D13) 
//ARDUINO MEGA: APA102 (or similar) 'DATA' TO MOSI (D51) AND 'CLOCK' TO SCK (D52) 
//BE CAREFULLY!! NOT CONNECT APA102 REVERSED
//GND ARDUINO AND GND APA102 MUST BE CONNECTED
//POWER SUPPLY OF APA102 IS EXTERNAL FROM ARDUINO
//BLUETOOTH: TX in RX (D0) AND RX in TX(D1) BAUD RATE 9800
//A2 CONNECT TO RESET

#include <EEPROM.h>
#include "EEPROMAnything.h"
////////////////////////////////////////////////////////////
/*EEPROMAnything.h is a file program (you must add it) with the next source:
/////////////////////////////////////////////////////////////
#include <EEPROM.h>
#include <Arduino.h>  // for type definitions
template <class T> int EEPROM_writeAnything(int ee, const T& value)
{
    const byte* p = (const byte*)(const void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
          EEPROM.write(ee++, *p++);
    return i;
}
template <class T> int EEPROM_readAnything(int ee, T& value)
{
    byte* p = (byte*)(void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
          *p++ = EEPROM.read(ee++);
    return i;
}
/////////////////////////////////////////////
*/
struct escala_t
{
  uint8_t nombre; //0 defoult por programa// 1 recoger valores de memoria
  //String chipled = cero para "apa102" y uno "ws2812b";
  uint8_t chipled;
  uint8_t numstrip;
  uint8_t numradios;
  uint8_t radio1directo; //1 direct 0 inverso
  uint8_t radio2directo;
  uint8_t radio2180;//0 empieza en cero 1 empieza en 180
  uint8_t offset1;
  uint8_t offset2;
  uint8_t brillo;
  uint8_t animate;
  uint8_t num_leds;
  unsigned int numpasos;
  unsigned int angreducido;
  unsigned int sizePolarRedu;
  uint8_t PolarRedu[850];
} persistence;

#include "FastLED.h"
CRGB leds[141];
int angulo;
unsigned int numled;
unsigned int k = 0;
bool pasa = false;
bool cambiaLed = false;
long tiempoDibujo = 0;
long periodo = 0;
long periodoini = 0;
long previoustime = 0;
long tiempo = 0;
long contaseconds = 0;
unsigned int ang = 360;
long tvariable = 0;
int tiempoescritura = 700;
byte LedColour = 0;
byte vred = 0;
byte vgreen = 0;
byte vblue = 0;
int angAux = 0;
int anginicio = 360;
int contaang = 0;
long tiempoanimate = 0;
String input = "";

void setup() {
  //Para resetear
  pinMode(A2, INPUT);
  digitalWrite(A2, LOW);

  Serial.begin(9800);
  EEPROMI2C();

  FastLED.addLeds<APA102>(leds, persistence.num_leds + persistence.offset1);
 
  FastLED.setBrightness(persistence.brillo);
  attachInterrupt(digitalPinToInterrupt(2), pasaIman, RISING); 
  memset(leds, 0, 141 * 3);
  FastLED.show();

}
void loop() {
  cargaSerial();
  if (pasa == true) {
    pasa = false;
    pasa = false;
    tiempo = micros();
    periodoini = tiempo - previoustime;
    periodo = tiempo - previoustime - tvariable ;

    //periodo teorico
    tiempoDibujo = periodo / 360;
    if (tiempoDibujo < 0) tiempoDibujo = 0;
    previoustime = tiempo;
    contaang = 0;
    k = 0;
    angulo = persistence.PolarRedu[k];
    if (k / 3 >= persistence.angreducido) {
      angulo += 255;
    };
    for (ang = 0; ang < 360 ; ang++) {
      contaang++;
      cambiaLed = false;
      while (angulo == ang) {
        cambiaLed = true;
        if (persistence.radio1directo == 1) {
          numled =  persistence.PolarRedu[k + 1] - 1 + persistence.offset1;
        } else {
          //inverso es cero
          numled = persistence.num_leds -  persistence.PolarRedu[k + 1] + persistence.offset1;
        }
        LedColour =  persistence.PolarRedu[k + 2];
        //color option 0
        vred = 0;
        vgreen = 0;
        vblue = 0;
        if (LedColour == 4 || LedColour == 6 || LedColour == 7 || LedColour == 1) {
          vblue = 255;
        }
        if (LedColour == 3 || LedColour == 5 || LedColour == 6 || LedColour == 1) {
          vgreen = 255;
        }
        if (LedColour == 2 || LedColour == 5 || LedColour == 7 || LedColour == 1) {
          vred = 255;
        }
        leds[numled].r = vred;
        leds[numled].g = vgreen;
        leds[numled].b = vblue;

        k += 3;
        if (k >= persistence.sizePolarRedu) {
          angulo = 999;
        }else{
          angulo = persistence.PolarRedu[k];
          if (k / 3 >= persistence.angreducido) {
            angulo += 255;
          }
        }
      }

      if (cambiaLed == true) {
        FastLED.show();
        if (tiempoDibujo > tiempoescritura) {
          delayMicroseconds(tiempoDibujo - tiempoescritura);
        }
      } else {
        if (tiempoDibujo > tiempoescritura) {
          delayMicroseconds(tiempoDibujo + tiempoescritura * persistence.numpasos / (360 - persistence.numpasos));
        } else {
          delayMicroseconds(tiempoDibujo * 360 / (360 - persistence.numpasos));
        }
      }

      if (pasa == true) {
        tvariable += (micros() - previoustime) * 360 / contaang - periodoini;
        if (tvariable > 500000 || tvariable < -500000) {
          tvariable = 0;
        }
        return;
      }
    }
    tvariable += (micros() - previoustime) - periodoini;
    if (tvariable > 500000 || tvariable < -500000) {
      tvariable = 0;
    }
  }
}
void pasaIman () {
  pasa = true;
};

void  EEPROMI2C() {
  EEPROM_readAnything(0, persistence);
  if (persistence.nombre != 1) {
    memset(leds, 0, 141 * 3);
  }
};

void  cargaSerial() {
  unsigned int vint = 0;
  int index = 0;
  if (Serial.available()) {
    noInterrupts();
    Serial.println("READING");
    detachInterrupt(digitalPinToInterrupt(2));
    input = "";
    input  = Serial.readStringUntil(';');
    vint = input.toInt();
    persistence.chipled = vint;

    while (!Serial.available()) { };

    input  = Serial.readStringUntil(';');
    vint = input.toInt();
    persistence.numradios = vint;

    while (!Serial.available()) { };

    input  = Serial.readStringUntil(';');
    vint = input.toInt();
    persistence.numstrip = vint;
    if (vint == 0) {
      persistence.nombre = 0;
    } else {
      persistence.nombre = 1;
    }

    while (!Serial.available()) { };

    input  = Serial.readStringUntil(';');
    vint = input.toInt();
    persistence.radio1directo = vint;

    while (!Serial.available()) { };

    input  = Serial.readStringUntil(';');
    vint = input.toInt();
    persistence.radio2directo = vint;


    while (!Serial.available()) { };


    input  = Serial.readStringUntil(';');
    vint = input.toInt();
    persistence.radio2180 = vint;

    while (!Serial.available()) { };

    input  = Serial.readStringUntil(';');
    vint = input.toInt();
    persistence.offset1 = vint;

    while (!Serial.available()) { };

    input  = Serial.readStringUntil(';');
    vint = input.toInt();
    persistence.offset2 = vint;

    while (!Serial.available()) { };

    input  = Serial.readStringUntil(';');
    vint = input.toInt();
    persistence.brillo = vint;

    while (!Serial.available()) { };


    input  = Serial.readStringUntil(';');
    vint = input.toInt();
    persistence.animate = vint;

    while (!Serial.available()) { };

    input  = Serial.readStringUntil(';');
    vint = input.toInt();
    persistence.numpasos = vint;

    while (!Serial.available()) { };

    input  = Serial.readStringUntil(';');
    vint = input.toInt();
    persistence.num_leds = vint;

    while (!Serial.available()) { };

    input  = Serial.readStringUntil(';');
    vint = input.toInt();
    persistence.angreducido = vint;
    index = 0;

    while (!Serial.available()) { };

    input  = Serial.readStringUntil(';');
    vint = input.toInt();
    persistence.sizePolarRedu = vint;
    index = 0;

    while (!Serial.available()) { };

    while (1) {

      while (!Serial.available()) { };

      input  = Serial.readStringUntil(';');

      if (input == "DESCONECTA" ) {
        Serial.println("RECORDING");
        EEPROM_writeAnything(0, persistence);
        Serial.println("DESCONECTANDO");
        delay(5);
        pinMode(A2, OUTPUT);
        analogWrite(A2, 255);
        delay(5000);
        Serial.println("XXX");
        return;
      }
      vint = input.toInt();
      persistence.PolarRedu[index] = vint;
      index++;
    }
    interrupts();
  }
};
