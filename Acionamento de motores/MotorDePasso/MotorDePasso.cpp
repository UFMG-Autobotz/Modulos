/*  MotorDePasso.cpp
 *
 *  Classe para facilitar o acionamento de motores de passo em códigos .ino
 *  
 *  Os passos ocorrem em segundo plano, e o processamento pode
 *  seguir normalmente enquanto o motor gira.
 */

#include "MotorDePasso.h"

MotorDePasso::MotorDePasso() : vel(0), ppr(48), task_id(-1) {}

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

  setupTaskScheduler(4, 2);
  startSchedulerTicking();
}

void MotorDePasso::passos(int num_passos)
{
  unscheduleTimer1Task(task_id);
  
  if(vel == 0)
    return;
  
  pulso = 30000/(vel*ppr);
  contagem = abs(num_passos);
  dir = (num_passos == 0) ? 0 : (num_passos > 0) ? 1 : -1;

  if(dir)
    meio_passo_1(this);
  else
    soltar();
}

int MotorDePasso::passoAtual()
{
  return i % ppr;
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

void MotorDePasso::velocidade(float veloc)
{
  vel = max(0,veloc);
}

float MotorDePasso::velocidade()
{
  return vel;
}

void MotorDePasso::soltar()
{  
  digitalWrite(pinos[0],LOW);
  digitalWrite(pinos[1],LOW);
  digitalWrite(pinos[2],LOW);
  digitalWrite(pinos[3],LOW);

  task_id = -1;
}

void MotorDePasso::meio_passo_2(MotorDePasso* mot)
{
  digitalWrite(mot->pinos[mot->i % 4], LOW);
  (mot->dir == 1) ? ++mot->i : --mot->i;
  
  if(--mot->contagem > 0)
    mot->task_id = scheduleTimer1Task(meio_passo_1, mot, mot->pulso);
  else
    mot->soltar();
}

void MotorDePasso::meio_passo_1(MotorDePasso* mot)
{
  digitalWrite(mot->pinos[((mot->dir == 1) ? (mot->i+1) : (mot->i-1)) % 4], HIGH);

  mot->task_id = scheduleTimer1Task(meio_passo_2, mot, mot->pulso);
}
