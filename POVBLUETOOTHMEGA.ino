
#include <avr/pgmspace.h>
#include <EEPROM.h>
#include "EEPROMAnything.h"

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
  uint8_t PolarRedu[3800];
} persistence;


#include "FastLED.h"
CRGB leds[141];
CRGB leds2[141];
#define DATA_PIN 6
#define DATA_PIN2 7
#define DATA_PIN3 10
#define CLOCK_PIN3 11
int angulo;
int angulo2;
unsigned int numled;
unsigned int k = 0;
unsigned int k2 = 0;
bool pasa = false;
bool cambiaLed = false;
bool cambiaLed2 = false;
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
int anginicio2 = 180;
int kinicial = 0;
int kinicial2 = 0;
int contaang = 0;
long tiempoanimate = 0;
String input = "";
//byte arrayang[180];
//byte arrayang2[180];

void setup() {
  //Para resetear

  //pinMode(A2, INPUT);
  //digitalWrite(A2, LOW);

  pinMode(A2, INPUT);
  digitalWrite(A2, LOW);
  //pinMode(A2, INPUT_PULLUP);
  //
  Serial.begin(19200);
  EEPROMI2C();
  //pinMode(2, INPUT);
  //digitalWrite(2, LOW);
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  pinMode(3, OUTPUT);
  digitalWrite(3, LOW);

  //FastLED.addLeds<APA102>(leds, persistence.num_leds+ persistence.offset1);
  //pinMode(18, OUTPUT);
  //digitalWrite(18, LOW);

  //Serial.println(persistence.sizePolarRedu);
  //Serial.println(persistence.brillo);

  if (persistence.chipled == 0) { //apa102
    //if(persistence.numstrip==1){siempre quesea apa102
    if (persistence.numradios == 1) {
      FastLED.addLeds<APA102>(leds, persistence.num_leds + persistence.offset1);
    }
    if (persistence.numradios == 2) {
      FastLED.addLeds<APA102>(leds, persistence.num_leds+persistence.num_leds+persistence.offset1 + persistence.offset2);
    }
    //}
    if (persistence.numstrip == 2) {
      //FastLED.addLeds<APA102,10, 11, RGB,  DATA_RATE_MHZ(24)>(leds2, 0,80);
      if (persistence.numradios == 1) {
        FastLED.addLeds<APA102, DATA_PIN3, CLOCK_PIN3, RGB,  DATA_RATE_MHZ(24)>(leds2, persistence.num_leds + persistence.offset1);
      }
      if (persistence.numradios == 2) {
        FastLED.addLeds<APA102, DATA_PIN3, CLOCK_PIN3, RGB,  DATA_RATE_MHZ(24)>(leds2, persistence.num_leds + persistence.num_leds + persistence.offset1 + persistence.offset2);
      }
    }
  }
  if (persistence.chipled == 1) { //ws2812b
    //if(persistence.numstrip==1){siempre quesea WS2812B
    if (persistence.numradios == 1) {
      FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, persistence.num_leds + persistence.offset1);
    }
    if (persistence.numradios == 2) {
      FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, persistence.num_leds + persistence.num_leds + persistence.offset1 + persistence.offset2);
    }
    //}
    if (persistence.numstrip == 2) {
      if (persistence.numradios == 1) {
        FastLED.addLeds<WS2812B, DATA_PIN2, RGB>(leds2, persistence.num_leds + persistence.offset1);
      }
      if (persistence.numradios == 2) {
        FastLED.addLeds<WS2812B, DATA_PIN2, RGB>(leds2, persistence.num_leds + persistence.num_leds + persistence.offset1 + persistence.offset2);
      }
    }
  }

  FastLED.setBrightness(persistence.brillo);
  attachInterrupt(digitalPinToInterrupt(2), pasaIman, RISING);
  //FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, 0,40);
  //FastLED.addLeds<APA102, DATA_PIN2, CLOCK_PIN2, RGB, DATA_RATE_MHZ(24)>(leds, 0,80);
  //FastLED.addLeds<APA102,51, 52, RGB,  DATA_RATE_MHZ(24)>(leds2, 0,80);
  //FastLED.addLeds<APA102>(leds2, 0,80);
  //FastLED.addLeds<WS2812B, DATA_PIN2, RGB>(leds2, persistence.num_leds + persistence.offset1);
  memset(leds, 0, 141*3);
  memset(leds2, 0, 141*3);
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

    //Se prepara la k para que entre en el primer registro que contenga  anguloinicio (360 - anginicio) o si no existe el primero mayor
    if (persistence.animate > 0) {
      if ((previoustime - tiempoanimate) > 360 / persistence.animate * 1000000) {
        tiempoanimate = previoustime;
      }
      anginicio = floor((previoustime - tiempoanimate) * persistence.animate / 1000000);

      if (anginicio >= 360 || anginicio == 0) {
        anginicio = 360;
      }
    }


    if (anginicio == 360) { //angulo inicial cero (360 - anginicio)
      k = 0;
    } else {
      k = kinicial;
      //lectura de PROGMEM
      angulo = persistence.PolarRedu[k];
      if (k / 3 >= persistence.angreducido) {
        angulo += 255;
      };
      //En este caso se considera que esoy con un angulo2 por encima de cero y el ÃƒÆ’Ã†â€™Ãƒâ€šÃ‚Â¡ngulo de inicio (360 - anginicio) estÃƒÆ’Ã†â€™Ãƒâ€šÃ‚Â¡ por debajo de cero
      //entonces tengo que ponerlo desde el ÃƒÆ’Ã†â€™Ãƒâ€šÃ‚Âºltimo registro para que pase por el siguiente while
      if (angulo < 360 - anginicio ) {
        k = persistence.sizePolarRedu - 3;
        angulo = persistence.PolarRedu[k];
        if (k / 3 >= persistence.angreducido) {
          angulo += 255;
        };
      }

      while (angulo >= 360 - anginicio) {
        k -= 3;
        angulo = persistence.PolarRedu[k];
        if (k / 3 >= persistence.angreducido) {
          angulo += 255;
        };
      }

      //tengo que dejarlo en el primer registro del angulo inicial (360 - anginicio) o si no hay para ese angulo, el primero del siguiente
      k += 3;

      if (k >= persistence.sizePolarRedu) {
        k = 0;
      };

    }
    angulo = persistence.PolarRedu[k];
    if (k / 3 >= persistence.angreducido) {
      angulo += 255;
    };
    kinicial = k;



    //Se prepara la k2 para que entre en el primer registro que contenga ese inicial  (360 - anginicio2)o si no existe el primero mayor
    if (persistence.numstrip == 2 || persistence.radio2180 == 1) {
      anginicio2 = anginicio - 180;

      if (anginicio2 < 0) {
        anginicio2 += 360;
      }
      if (anginicio2 == 360) { //angulo inicial2 cero (360 - anginicio2)
        k2 = 0;
        angulo2 = persistence.PolarRedu[k2];
        if (k2 / 3 >= persistence.angreducido) {
          angulo2 += 255;
        };
      } else {
        k2 = kinicial2;
        angulo2 = persistence.PolarRedu[k2];
        if (k2 / 3 >= persistence.angreducido) {
          angulo2 += 255;
        };
        //En este caso se considera que esoy con cerca de cero con un angulo2 por encima de cero y el ÃƒÆ’Ã†â€™Ãƒâ€šÃ‚Â¡ngulo de inicio2 estÃƒÆ’Ã†â€™Ãƒâ€šÃ‚Â¡ por debajo de cero
        //entonces tengo que ponerlo desde el ÃƒÆ’Ã†â€™Ãƒâ€šÃ‚Âºltimo registro para que pase por el siguiente while
        if (angulo2 < 360 - anginicio2 ) {
          k2 = persistence.sizePolarRedu - 3;
          angulo2 = persistence.PolarRedu[k2];
          if (k2 / 3 >= persistence.angreducido) {
            angulo2 += 255;
          };
        }

        while (angulo2 >= 360 - anginicio2 ) {
          k2 -= 3;
          angulo2 = persistence.PolarRedu[k2];
          if (k2 / 3 >= persistence.angreducido) {
            angulo2 += 255;
          };
        }
        //tengo que dejarlo en el primer registro del angulo inicial (360 - anginicio2) o si no hay para ese angulo, el primero del siguiente
        k2 += 3;
        if (k2 >= persistence.sizePolarRedu) {
          k2 = 0;
        };

      }
      angulo2 = persistence.PolarRedu[k2];
      if (k2 / 3 >= persistence.angreducido) {
        angulo2 += 255;
      };
      kinicial2 = k2;
    }


    //Se barren todos los ÃƒÆ’Ã†â€™Ãƒâ€šÃ‚Â¡ngulos a ver si hay info en la mtariz de cambio de led
contaang=0;

    for (angAux = -anginicio; angAux < 360 - anginicio; angAux++) {
      contaang++;
      if (angAux < 0) {
        ang = 360 + angAux;
      } else {
        ang = angAux;
      }

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
        if (LedColour == 8) {
          vred = 255 *  persistence.PolarRedu[k + 1] / persistence.num_leds;
          vblue = 255 - 255 *  persistence.PolarRedu[k + 1] / persistence.num_leds;
        }
        if (LedColour == 9) {
          vblue = 255 *  persistence.PolarRedu[k + 1] / persistence.num_leds;
          vgreen = 255 - 255 * persistence.PolarRedu[k + 1] / persistence.num_leds;
        }
        if (LedColour == 10) {
          vgreen = 255 *  persistence.PolarRedu[k + 1] / persistence.num_leds;
          vred = 255 - 255 *  persistence.PolarRedu[k + 1] / persistence.num_leds;
        }
        leds[numled].r = vred;
        leds[numled].g = vgreen;
        leds[numled].b = vblue;
        if (persistence.numradios == 2 ) {
          if (persistence.radio2directo == 1) {
            numled = persistence.PolarRedu[k + 1] - 1 + persistence.offset1 + persistence.num_leds + persistence.offset2;
            
          } else {
            //inverso es cero
            numled = persistence.offset1 + 2 * persistence.num_leds + persistence.offset2 - persistence.PolarRedu[k + 1];
          }
          if (persistence.radio2180 == 0) {
          
        leds[numled].r = vred;
        leds[numled].g = vgreen;
        leds[numled].b = vblue;
          }
          if (persistence.numstrip == 2 && persistence.radio2180 == 1) {
        leds2[numled].r = vred;
        leds2[numled].g = vgreen;
        leds2[numled].b = vblue;
          }
        }
        k += 3;
        if (k >= persistence.sizePolarRedu) {
          k = 0;
        };
        angulo = persistence.PolarRedu[k];
        if (k / 3 >= persistence.angreducido) {
          angulo += 255;
        };
      }


      if (persistence.numstrip == 2 || persistence.radio2180 == 1) {//dos srtrip o numradios2 y el segundo a 180

        ang += 180;
        while (ang >= 360) {
          ang -= 360;
        }
        cambiaLed2 = false;
        

        
        while (angulo2 == ang) {

          cambiaLed2 = true;
          if (persistence.radio1directo == 1) {
            numled = persistence.PolarRedu[k2 + 1] - 1 + persistence.offset1;
          } else {
            //inverso es cero
            numled = persistence.num_leds - persistence.PolarRedu[k2 + 1] + persistence.offset1;
          }
          LedColour = persistence.PolarRedu[k2 + 2];
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
          if (LedColour == 8) {
            vred = 255 * numled / persistence.num_leds;
            vblue = 255 - 255 * numled / persistence.num_leds;
          }
          if (LedColour == 9) {
            vblue = 255 * numled / persistence.num_leds;
            vgreen = 255 - 255 * numled / persistence.num_leds;
          }
          if (LedColour == 10) {
            vgreen = 255 * numled / persistence.num_leds;
            vred = 255 - 255 * numled / persistence.num_leds;
          }
          if ( persistence.numstrip == 2) {
        leds2[numled].r = vred;
        leds2[numled].g = vgreen;
        leds2[numled].b = vblue;
          }
          if (persistence.numradios == 2 ) {
            if (persistence.radio2directo == 1) {
              numled = persistence.PolarRedu[k2 + 1] - 1 + persistence.offset1 + persistence.num_leds + persistence.offset2;
            } else {
              //inverso es cero
              numled = + persistence.offset1 + 2 * persistence.num_leds + persistence.offset2 - persistence.PolarRedu[k2 + 1];
            }
            if ( persistence.numstrip == 2 && persistence.radio2180 == 0) {
        leds2[numled].r = vred;
        leds2[numled].g = vgreen;
        leds2[numled].b = vblue;
            }
            if (persistence.radio2180 == 1) {
        leds[numled].r = vred;
        leds[numled].g = vgreen;
        leds[numled].b = vblue;
            }
          }

          k2 += 3;

          if (k2 >= persistence.sizePolarRedu) {
            k2 = 0;
          };
          angulo2 = persistence.PolarRedu[k2];
          if (k2 / 3 >= persistence.angreducido) {
            angulo2 += 255;
          };
        }
        
      }



      if (cambiaLed == true || cambiaLed2 == true ) {

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
        //para que se sume a tvariable algo que se supone positivo
        tvariable += (micros() - previoustime) * 360 /contaang - periodoini;
        if (tvariable > 500000 || tvariable < -500000) {
          tvariable = 0;
        }
        return;
      }
    }
    //para que se sume a tvariable algo negativo
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

  memset(leds, 0, 141*3);
  memset(leds2, 0, 141*3);
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
    ///Serial.println(persistence.chipled);
    //0 "apa102";
    //1 "ws2812b";
    while (!Serial.available()) { };

    input  = Serial.readStringUntil(';');
    vint = input.toInt();
    persistence.numradios = vint;

    while (!Serial.available()) { };

    input  = Serial.readStringUntil(';');
    vint = input.toInt();
    persistence.numstrip = vint;
    if (vint == 0) {
      persistence.nombre = 0; //provoca que no se lea de lo que se ha grabado en adelante hasta q se vuelva a grabar. Ahora se mostrarÃƒÂ­a lo de la memoria interna
      //Lo suyo serÃƒÂ­a un Reset fÃƒÂ­sico
    } else {
      persistence.nombre = 1;
    }
    //Serial.println(persistence.numstrip);
    //Serial.println(persistence.nombre);

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
        ////Serial.println("recibido;");
        // pinMode(A2, OUTPUT);
        //analogWrite(A2, 255);
        Serial.println("RECORDING");
        EEPROM_writeAnything(0, persistence);
        Serial.println("DESCONECTANDO");
        delay(5);
        //attachInterrupt(digitalPinToInterrupt(2), pasaIman, RISING);
        //aquÃƒÂ­ deberÃƒÂ­a haber un resetfÃƒÂ­sico
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






