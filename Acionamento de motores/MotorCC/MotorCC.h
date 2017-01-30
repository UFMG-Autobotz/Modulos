/*  MotorCC.h
 *
 *  Classe para facilitar o acionamento de motores CC em códigos .ino
 */

#ifndef MotorCC_h
#define MotorCC_h

#include "Arduino.h"

class MotorCC
{
private:
	
  byte enable, H1, H2;
  
public:

  void Pinagem(int enable, int H1, int H2);
  void Mover(int vel_pwm);
};

#endif
