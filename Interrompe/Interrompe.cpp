/* Traz para alto nível as interrupções do processador do ATmega328P
 *  
 * Versão 0.2: Adicionadas interrupções por timer
 * 
 * Autor: Daniel Leite Ribeiro
 *        Autobotz UFMG
 */

#include <Arduino.h>
#include "Interrompe.h"

namespace Interrompe
{

uint8_t borda_sub[3], borda_desc[3];
uint8_t PORTB_anterior, PORTC_anterior, PORTD_anterior;

bool limpar_flag = false;

voidFuncPtr isr_personalizada_B[8], isr_personalizada_C[7], isr_personalizada_D[8];

void *arg_B[8], *arg_C[7], *arg_D[8];

bool habilita(const uint8_t pino, const byte tipo, voidFuncPtr isr_ptr, void* arg = NULL)
{
  if(digitalPinToPCICR(pino) == NULL) // Pino inválido
    return false;
  
  uint8_t bit_grupo = digitalPinToPCICRbit(pino);
  uint8_t bit_mascara = digitalPinToPCMSKbit(pino);

  switch(bit_grupo)
  {
  case 0:
    isr_personalizada_B[bit_mascara] = isr_ptr;
    arg_B[bit_mascara] = arg;
    break;

  case 1:
    isr_personalizada_C[bit_mascara] = isr_ptr;
    arg_C[bit_mascara] = arg;
    break;

  case 2:
    isr_personalizada_D[bit_mascara] = isr_ptr;
    arg_D[bit_mascara] = arg;
  }
  
  if(limpar_flag)
    bitSet(PCIFR, bit_grupo);
  
  if(tipo == RISING || tipo == CHANGE)
    bitSet(borda_sub[bit_grupo], bit_mascara);

  if(tipo == FALLING || tipo == CHANGE)
    bitSet(borda_desc[bit_grupo], bit_mascara);
  
  bitSet(PCICR, bit_grupo);   // Habilita o grupo de pinos (são três)
  bitSet(*digitalPinToPCMSK(pino), bit_mascara);  // Habilita o pino dentro do grupo

  return true;
}

bool desabilita(const uint8_t pino)
{
  if(digitalPinToPCICR(pino) == NULL) // Pino inválido
    return false;

  uint8_t bit_grupo = digitalPinToPCICRbit(pino);
  uint8_t bit_mascara = digitalPinToPCMSKbit(pino);

  bitClear(borda_sub[bit_grupo], bit_mascara);
  bitClear(borda_desc[bit_grupo], bit_mascara);
  bitClear(*digitalPinToPCMSK(pino), bit_mascara);  // Desabilita o pino dentro do grupo

  if(*digitalPinToPCMSK(pino) == 0) // Se não tiver sobrado nenhum pino nesse grupo,
    bitClear(PCICR, bit_grupo);     // desabilita o grupo

  return true;
}

bool modifica(const uint8_t pino, const uint8_t tipo)
{
  if(digitalPinToPCICR(pino) == NULL) // Pino inválido
    return false;

  uint8_t bit_grupo = digitalPinToPCICRbit(pino);
  uint8_t bit_mascara = digitalPinToPCMSKbit(pino);

  bitClear(borda_sub[bit_grupo], bit_mascara);
  bitClear(borda_desc[bit_grupo], bit_mascara);

  if(tipo == RISING || tipo == CHANGE)
    bitSet(borda_sub[bit_grupo], bit_mascara);

  if(tipo == FALLING || tipo == CHANGE)
    bitSet(borda_desc[bit_grupo], bit_mascara);
}

bool modifica(const uint8_t pino, voidFuncPtr isr_ptr, void* arg)
{
  if(digitalPinToPCICR(pino) == NULL) // Pino inválido
    return false;

  uint8_t bit_grupo = digitalPinToPCICRbit(pino);
  uint8_t bit_mascara = digitalPinToPCMSKbit(pino);  
  
  switch(bit_grupo)
  {
  case 0:
    isr_personalizada_B[bit_mascara] = isr_ptr;
    arg_B[bit_mascara] = arg;
    break;

  case 1:
    isr_personalizada_C[bit_mascara] = isr_ptr;
    arg_C[bit_mascara] = arg;
    break;

  case 2:
    isr_personalizada_D[bit_mascara] = isr_ptr;
    arg_D[bit_mascara] = arg;
  }
}

void permitirImediatas(bool permitir)
{
  limpar_flag = !permitir;
}

// Gambiarra para evitar repetições
#define EXEC_ISR_(bit,isr,grupo) if(validos & bit) isr_personalizada_ ## grupo[isr](arg_ ## grupo[isr])
#define exec_isr(bit,isr) EXEC_ISR_(bit,isr,B)

ISR(PCINT0_vect)
{
  uint8_t validos = (PORTB & ~PORTB_anterior & borda_sub[0]) | (~PORTB & PORTB_anterior & borda_desc[0]);
  
  exec_isr(0x01,0);
  exec_isr(0x02,1);
  exec_isr(0x04,2);
  exec_isr(0x08,3);
  exec_isr(0x10,4);
  exec_isr(0x20,5);
//exec_isr(0x40,6);	// No Arduino Uno, esses dois pinos estão
//exec_isr(0x80,7);	// ligados aos terminais do cristal oscilador
  
  PORTB_anterior = PORTB;
}

#define exec_isr(bit,isr) EXEC_ISR_(bit,isr,C)

ISR(PCINT1_vect)
{
  uint8_t validos = (PORTC & ~PORTC_anterior & borda_sub[1]) | (~PORTC & PORTC_anterior & borda_desc[1]);

  exec_isr(0x01,0);
  exec_isr(0x02,1);
  exec_isr(0x04,2);
  exec_isr(0x08,3);
  exec_isr(0x10,4);	
  exec_isr(0x20,5);
//exec_isr(0x40,6); // PC6 é o pino de Reset.
  
  PORTC_anterior = PORTC;
}

#define exec_isr(bit,isr) EXEC_ISR_(bit,isr,D)

ISR(PCINT2_vect)
{
  uint8_t validos = (PORTD & ~PORTD_anterior & borda_sub[2]) | (~PORTD & PORTD_anterior & borda_desc[2]);
  
  exec_isr(0x01,0);
  exec_isr(0x02,1);
  exec_isr(0x04,2);
  exec_isr(0x08,3);
  exec_isr(0x10,4);
  exec_isr(0x20,5);
  exec_isr(0x40,6);
  exec_isr(0x80,7);
  
  PORTD_anterior = PORTD;
}

#undef ISR_PERS
#undef exec_isr

} // namespace Interrompe
