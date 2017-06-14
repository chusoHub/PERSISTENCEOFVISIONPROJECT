/*Programa para configurar el bluetooth JY-MCU HC-06 
 * por primera vez y asignarle nombre baud y Pin por USB
 * 
 *Cuando el monitor serial se comunique hay que escribir:
 *comando         respuesta esperada
 * AT              Ok
 * AT+VERSION      OklinvarV1.8 (o lo que sea)
 * AT+BAUD4     Ok9600  (es la banda que le definimos se puede poner otra)
 * AT+NAMEX        ...  no hay que poner X, en la posición de X se pone el nombre del Bluetooth
 * AT+PINXXXX      ...  no hay que poner XXXX, en la posición de XXXX se pone el pin que por defecto es 1234


   1 --> 1200 baudios
   2 --> 2400 baudios
   3 --> 4800 baudios
   4 --> 9600 baudios (por defecto)
   5 --> 19200 baudios
   6 --> 38400 baudios
   7 --> 57600 baudios
   8 --> 115200 baudios
    */
#include <SoftwareSerial.h>

SoftwareSerial BT(10,11); //11 RX, 110 TX de Arduino que van a su opuesto del bluetooth
 
void setup()
{ 
    BT.begin(9600); //Velocidad del puerto del módulo Bluetooth
  Serial.begin(9600); //Abrimos la comunicación serie con el PC y establecemos velocidad
}
 
void loop()
{
  if(  Serial.available())
  {
    BT.write(  Serial.read());
  }
 
  if(BT.available())
  {
       Serial.write(BT.read());
  }
}


