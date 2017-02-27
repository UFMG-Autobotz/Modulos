/*  MotorCC.cpp
 *
 *  Classe para facilitar o acionamento de motores CC em códigos .ino
 */

#include "MotorCC.h"
#include <TimerScheduler.h>

static void pararMotor(MotorCC* mot);

MotorCC::MotorCC() : task_id(-1), tipo_parada(soft) {}

MotorCC::MotorCC(int enable, int H1, int H2) : MotorCC()
{
  pinagem(enable, H1, H2);
}

void MotorCC::pinagem(int pino_enable, int pino_H1, int pino_H2)
{
   enable = pino_enable;
   H1 = pino_H1;
   H2 = pino_H2;
   
   pinMode(enable,OUTPUT);
   pinMode(H1,OUTPUT);
   pinMode(H2,OUTPUT);

   setupTaskScheduler(4, 150);
   startSchedulerTicking();
}

void MotorCC::mover(int vel_pwm)
{
   if(vel_pwm > 0)
   {
      analogWrite(enable,vel_pwm);
      digitalWrite(H1,HIGH);
      digitalWrite(H2,LOW);
   }
   else if(vel_pwm < 0)
   {
      analogWrite(enable,-vel_pwm);
      digitalWrite(H1,LOW);
      digitalWrite(H2,HIGH);
   }
   else
   {
      digitalWrite(enable,tipo_parada);
      digitalWrite(H1,LOW);
      digitalWrite(H2,LOW);

      task_id = -1;
   }
  
   unscheduleTimer1Task(task_id); 
}

void MotorCC::mover(int vel_pwm, unsigned long duracao)
{
   mover(vel_pwm);
   task_id = scheduleTimer1Task(pararMotor, this, duracao);
}

void MotorCC::tipoParada(Parada tipo)
{
   tipo_parada = tipo;
} 

static void pararMotor(MotorCC* mot)
{
   mot->mover(0);
}
