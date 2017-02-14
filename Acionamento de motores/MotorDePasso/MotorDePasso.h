/*  MotorDePasso.h
 *
 *  Classe para facilitar o acionamento de motores de passo em códigos .ino
 */

#ifndef MotorDePasso_h
#define MotorDePasso_h

#include <Arduino.h>
#include <TimerScheduler.h>

class MotorDePasso
{
  volatile uint8_t i, task_id;
  uint8_t dir, pinos[4];
  unsigned int pulso;

  int ppr, contagem;
  float vel;

  static MotorDePasso *atual;
  static void meio_passo_1(MotorDePasso*), meio_passo_2(MotorDePasso*);

  void soltar();

public:

  MotorDePasso();
  MotorDePasso(int p1, int p2, int p3, int p4, int ppr_);
  
  void pinagem(int p1, int p2, int p3, int p4);
  
  int passosPorRevolucao();
  float velocidade();

  void passosPorRevolucao(int passos);
  void velocidade(float veolcidade);

  void passos(int num);  // Gira "num" passos para frente (se positivo) ou para trás (se negativo)
  int passoAtual();
};

#endif
