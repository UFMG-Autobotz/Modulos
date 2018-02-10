/*  MotorDePasso.cpp
 *  Autor: Daniel Leite Ribeiro
 *         Autobotz UFMG
 *
 *  Classe para facilitar o acionamento de motores de passo em códigos .ino
 *  
 *  Os passos ocorrem em segundo plano, e o processamento pode
 *  seguir normalmente enquanto o motor gira.
 */

#include "MotorDePasso.h"
#include <TimerScheduler.h>

MotorDePasso::MotorDePasso() : vel(0), ppr(48), task_id(-1), ok(false) {}

MotorDePasso::MotorDePasso(int p1, int p2, int p3, int p4, int PPR = 48) : MotorDePasso()
{
  pinagem(p1,p2,p3,p4);
  ppr = PPR;
}

void MotorDePasso::pinagem(int p1, int p2, int p3, int p4)
{
  pinos[0] = p1;
  pinos[1] = p2;
  pinos[2] = p3;
  pinos[3] = p4;

  pinMode(pinos[0],OUTPUT);
  pinMode(pinos[1],OUTPUT);
  pinMode(pinos[2],OUTPUT);
  pinMode(pinos[3],OUTPUT);

  setupTaskScheduler(6, 10);
  startSchedulerTicking();

  ok = true;
}

void MotorDePasso::passos(int num_passos)
{
  if(!ok)
    return;
  
  unscheduleTimer1Task(task_id);
  
  if(vel == 0 || ppr == 0)
    return;
  
  pulso = 30000/(vel*ppr);
  contagem = abs(num_passos);
  continuo = false;
  dir = (num_passos == 0) ? 0 : (num_passos > 0) ? 1 : -1;

  if(dir)
  {
    digitalWrite(pinos[i % 4], HIGH);
    meio_passo_1(this);
  }
  else
    soltar();
}

void MotorDePasso::irPara(int passo, int dir)
{
  if(!ok)
    return;
    
  if(passo >= ppr)
  {
    passos(1);
    continuo = true;
    return;
  }
  else if(passo < 0)
  {
    passos(-1);
    continuo = true;
    return;
  }
  
  int diferenca = passo - passoAtual();
  
  if(dir > 0)
    passos(diferenca > 0 ? diferenca : diferenca + ppr);
  else if(dir < 0)
    passos(diferenca < 0 ? diferenca : diferenca - ppr);
  else
    if(diferenca > ppr/2)
      passos(diferenca - ppr);
    else if(diferenca < -ppr/2)
      passos(diferenca + ppr);
    else
      passos(diferenca);
}

void MotorDePasso::soltar()
{  
  unscheduleTimer1Task(task_id);
  task_id = -1;
  
  digitalWrite(pinos[0],LOW);
  digitalWrite(pinos[1],LOW);
  digitalWrite(pinos[2],LOW);
  digitalWrite(pinos[3],LOW);
}

void MotorDePasso::meio_passo_1(MotorDePasso* mot)
{
  digitalWrite(mot->pinos[abs(mot->i + mot->dir) % 4], HIGH);

  mot->task_id = scheduleTimer1Task(meio_passo_2, mot, mot->pulso);
}

void MotorDePasso::meio_passo_2(MotorDePasso* mot)
{
  digitalWrite(mot->pinos[mot->i % 4], LOW);
  mot->i += mot->dir;

  if(!mot->continuo)
    --mot->contagem;
    
  if(mot->contagem > 0)
    mot->task_id = scheduleTimer1Task(meio_passo_1, mot, mot->pulso);
  else
    mot->soltar();
}
