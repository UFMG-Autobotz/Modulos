/*  MotorDePasso.cpp
 *
 *  Classe para facilitar o acionamento de motores de passo em códigos .ino
 *  
 *  Os passos ocorrem em segundo plano, e o processamento pode
 *  seguir normalmente enquanto o motor gira.
 */

#include "MotorDePasso.h"
#include "TimerScheduler.h"

MotorDePasso::MotorDePasso(){}

MotorDePasso::MotorDePasso(int p1, int p2, int p3, int p4)
{
  pinagem(p1,p2,p3,p4);
}

MotorDePasso::MotorDePasso(int p1, int p2, int p3, int p4, int passos)
{
  pinagem(p1,p2,p3,p4);
  ppr = passos;
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

  if(tickIntervalMs == 0)
  {
    setupTaskScheduler(4, 150);
    startSchedulerTicking();
  }
}

void MotorDePasso::passos(int num_passos)
{
  pulso = 30000/(vel*ppr);
  contagem = abs(num_passos);
  dir = (num_passos == 0) ? 0 : (num_passos > 0) ? 1 : -1;

  if(dir)
    meio_passo_1(this);
  else
  {
    digitalWrite(pinos[0],LOW);
    digitalWrite(pinos[1],LOW);
    digitalWrite(pinos[2],LOW);
    digitalWrite(pinos[3],LOW);
  }
}

void MotorDePasso::passosPorRevolucao(int PPR)
{
  if(PPR > 0)
    ppr = PPR;
}

int MotorDePasso::passosPorRevolucao()
{
  return ppr;
}

void MotorDePasso::velocidade(int veloc)
{
  if(veloc > 0)
    vel = veloc;
}

int MotorDePasso::velocidade()
{
  return vel;
}

void MotorDePasso::meio_passo_2(MotorDePasso* mot)
{
  digitalWrite(mot->pinos[mot->i % 4], LOW);
  (mot->dir == 1) ? ++mot->i : --mot->i;
  
  if(--mot->contagem)
    scheduleTimer1Task(meio_passo_1, mot, mot->pulso);
  else
  {
    digitalWrite(mot->pinos[0],LOW);
    digitalWrite(mot->pinos[1],LOW);
    digitalWrite(mot->pinos[2],LOW);
    digitalWrite(mot->pinos[3],LOW);
  }
}

void MotorDePasso::meio_passo_1(MotorDePasso* mot)
{
  digitalWrite(mot->pinos[((mot->dir == 1) ? (mot->i+1) : (mot->i-1)) % 4], HIGH);

  scheduleTimer1Task(meio_passo_2, mot, mot->pulso);
}

