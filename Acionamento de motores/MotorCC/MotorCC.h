/*  MotorCC.h
 *
 *  Classe para facilitar o acionamento de motores CC em códigos .ino
 */

#ifndef MotorCC_h
#define MotorCC_h

#include "Arduino.h"

class MotorCC
{
public:

  enum Parada {soft, hard};

  MotorCC();
  MotorCC(int enable, int H1, int H2);

  void pinagem(int enable, int H1, int H2);
  void mover(int vel_pwm);
  void mover(int vel_pwm, unsigned long duracao);
  void tipoParada(Parada tipo);

private:
  
  uint8_t enable, H1, H2, task_id;
  Parada tipo_parada;
};

#endif

