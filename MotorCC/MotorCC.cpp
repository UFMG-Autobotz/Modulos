/*  MotorCC.cpp
 *  Autor: Daniel Leite Ribeiro
 *         Autobotz UFMG
 *
 *  Classe para facilitar o acionamento de motores CC em códigos .ino
 */

#include "MotorCC.h"
#include <TimerScheduler.h>

static void pararMotor(MotorCC* mot);

MotorCC::MotorCC() : task_id(-1), tipo_parada(soft), ok(false) {}

MotorCC::MotorCC(int enable, int H1, int H2) : MotorCC()
{
  pinagem(enable, H1, H2);
}

void MotorCC::pinagem(int pino_enable, int pino_H1, int pino_H2)
{
  if(pino_enable == HAMS)
    is_hams = true;
  else
  {
    is_hams = false;
    enable = pino_enable;
    pinMode(enable,OUTPUT);
  }
  
  H1 = pino_H1;
  H2 = pino_H2;
  pinMode(H1,OUTPUT);
  pinMode(H2,OUTPUT);

  setupTaskScheduler(6, 10);
  startSchedulerTicking();

  ok = true;
}

void MotorCC::mover(int vel_pwm)
{
  if(!ok)
    return;
  
  if(is_hams)
  {
    if(vel_pwm <= 0)
      digitalWrite(H1,HIGH);
    else
      digitalWrite(H1,LOW);
      
    analogWrite(H2,abs(vel_pwm));
  }
  else
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
    }
  }
  
  unscheduleTimer1Task(task_id);
  task_id = -1;
}

void MotorCC::mover(int vel_pwm, unsigned long duracao)
{
  if(!ok)
    return;
    
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
