/*  MotorDePasso.h
 *
 *  Classe para facilitar o acionamento de motores de passo em códigos .ino
 */

#ifndef MotorDePasso_h
#define MotorDePasso_h

#include "Arduino.h"

class MotorDePasso
{
private:

  byte pinos[4];
  int i = 0, j = 0;

public:

  int PassosPorRevolucao;
  int Velocidade;

  void Pinagem(int p1, int p2, int p3, int p4);
  void Passo(int dir);
};

#endif
