/*  MotorDePasso.cpp
 *
 *  Classe para facilitar o acionamento de motores de passo em códigos .ino
 */

#include "MotorDePasso.h"

void MotorDePasso::Pinagem(int p1, int p2, int p3, int p4)
{
  pinos[0] = p1;
  pinos[1] = p2;
  pinos[2] = p3;
  pinos[3] = p4;

  pinMode(pinos[0],OUTPUT);
  pinMode(pinos[1],OUTPUT);
  pinMode(pinos[2],OUTPUT);
  pinMode(pinos[3],OUTPUT);
}

void MotorDePasso::Passo(int dir)
{
  // Gira um passo na direção indicada por "dir"

  if(dir == 1)
  {
    digitalWrite(pinos[j], HIGH);
    digitalWrite(pinos[i], HIGH);
    delay(30000/(Velocidade*PassosPorRevolucao));

    digitalWrite(pinos[j], LOW);
    delay(30000/(Velocidade*PassosPorRevolucao));

    j = i++;

    if(i == 4) i = 0;
    if(j == 4) j = 0;
  }

  else if(dir == -1)
  {
    digitalWrite(pinos[i], HIGH);
    digitalWrite(pinos[j], HIGH);
    delay(30000/(Velocidade*PassosPorRevolucao));

    digitalWrite(pinos[i], LOW);
    delay(30000/(Velocidade*PassosPorRevolucao));

    i = j--;

    if(i == -1) i = 3;
    if(j == -1) j = 3;
  }
  else if(dir == 0)
  {
    digitalWrite(pinos[0],LOW);
    digitalWrite(pinos[1],LOW);
    digitalWrite(pinos[2],LOW);
    digitalWrite(pinos[3],LOW);
  }
}
