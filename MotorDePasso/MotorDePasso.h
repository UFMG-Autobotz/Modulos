/*  MotorDePasso.h
 *  Autor: Daniel Leite Ribeiro
 *         Autobotz UFMG
 *
 *  Classe para facilitar o acionamento de motores de passo em códigos .ino
 */

#ifndef MotorDePasso_h
#define MotorDePasso_h

#include <Arduino.h>

class MotorDePasso
{
public:

  MotorDePasso();
  MotorDePasso(int p1, int p2, int p3, int p4, int ppr);
  
  void pinagem(int p1, int p2, int p3, int p4);
  
  int passosPorRevolucao() { return ppr; }
  void passosPorRevolucao(int passos) { ppr = max(0,passos); }
  
  float velocidade() { return vel; }
  void velocidade(float veolcidade) { vel = max(0,vel); }
    
  int passoAtual() { return i % ppr; }

  void passos(int num);                 // Gira "num" passos para frente (se positivo) ou para trás (se negativo)
  void irPara(int passo, int dir = 0);  // Posiciona-se no passo indicado

private:

  volatile int task_id, contagem;
  volatile bool continuo;
  volatile uint8_t i;
  unsigned int pulso;
  int pinos[4], dir, ppr;
  float vel;
  bool ok;
  
  static void meio_passo_1(MotorDePasso*), meio_passo_2(MotorDePasso*);
  void soltar();
};

#endif
