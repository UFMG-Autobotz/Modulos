/*  MotorCC.cpp
 *
 *  Classe para facilitar o acionamento de motores CC em códigos .ino
 */

#include "MotorCC.h"

void MotorCC::Pinagem(int pino_enable, int pino_H1, int pino_H2)
{
   enable = pino_enable;
   H1 = pino_H1;
   H2 = pino_H2;
   
   pinMode(enable,OUTPUT);
   pinMode(H1,OUTPUT);
   pinMode(H2,OUTPUT);
}

void MotorCC::Mover(int vel_pwm)
{
   if(vel_pwm > 0)
   {
      analogWrite(enable,vel_pwm);
      digitalWrite(H1,HIGH);
      digitalWrite(H2,LOW);
   }
   else if(vel_pwm < 0)
   {
      analoglWrite(enable,-vel_pwm);
      digitalWrite(H1,LOW);
      digitalWrite(H2,HIGH);
   }
   else
   {
      digitalWrite(enable,LOW);
      digitalWrite(H1,LOW);
      digitalWrite(H2,LOW);
   }
}
