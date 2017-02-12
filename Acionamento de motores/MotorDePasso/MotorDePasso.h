/*  MotorDePasso.h
 *
 *  Classe para facilitar o acionamento de motores de passo em códigos .ino
 */

#ifndef MotorDePasso_h
#define MotorDePasso_h

#include "Arduino.h"

class MotorDePasso
{
  uint8_t i, dir, pinos[4];
  unsigned int pulso;

  int ppr, vel, contagem;

  static MotorDePasso *atual;
  static void meio_passo_1(MotorDePasso*), meio_passo_2(MotorDePasso*);

public:

  MotorDePasso();
  MotorDePasso(int p1, int p2, int p3, int p4);
  MotorDePasso(int p1, int p2, int p3, int p4, int passos);
  
  void pinagem(int p1, int p2, int p3, int p4);
  
  int passosPorRevolucao();
  int velocidade();

  void passosPorRevolucao(int passos);
  void velocidade(int veolcidade);

  void passos(int num);  // Gira "num" passos para frente (se positivo) ou para trás (se negativo)
};

#endif
