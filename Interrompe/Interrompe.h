/* Traz para alto nível as interrupções do processador do ATmega328P
 *  
 * Versão 0.1: Apenas interrupções por mudança de nível
 * 
 * Autor: Daniel Leite Ribeiro
 *        Autobotz UFMG
 */

#ifndef Interrompe_h
#define Interrompe_h

#include <MsTimer2.h>
#include <Arduino.h>

namespace Interrompe
{

uint8_t conf_borda_sub[3], conf_borda_desc[3];
uint8_t PORTB_anterior, PORTC_anterior, PORTD_anterior;

bool limpar_flag = false;

typedef void (*voidFuncPtr)();
voidFuncPtr isr_personalizada_B[8], isr_personalizada_C[7], isr_personalizada_D[8];

struct TimerISR
{
  long ms;          // Momento (milissegundos) em que a função deverá ser chamada
  voidFuncPtr isr;  // Callback
  TimerISR* prox;
} inicio {0, NULL, NULL};

bool habilita(const uint8_t pino, voidFuncPtr isr_ptr, const byte tipo)
{
  if(digitalPinToPCICR(pino) == NULL) // Pino inválido
    return false;
  
 if(digitalPinToInterrupt(pino) != NOT_AN_INTERRUPT)    // O ATmega328P possui registradores de interrupção separados para os pinos PD2 e
  {                                                     // PD3, que apresentam ISRs próprias e têm prioridade em relação aos outros pinos
    if(limpar_flag)
      bitSet(EIFR, digitalPinToInterrupt(pino));
      
    attachInterrupt(digitalPinToInterrupt(pino), isr_ptr, tipo);    // Para esses pinos já existe função de alto nível
    return true;
  }

 uint8_t bit_grupo = digitalPinToPCICRbit(pino);
  uint8_t bit_mascara = digitalPinToPCMSKbit(pino);

  switch(bit_grupo)
  {
  case 0:
    isr_personalizada_B[bit_mascara] = isr_ptr;
    break;

  case 1:
    isr_personalizada_C[bit_mascara] = isr_ptr;
    break;

  case 2:
    isr_personalizada_D[bit_mascara] = isr_ptr;
  }
  
  if(limpar_flag)
    bitSet(PCIFR, bit_grupo);
  
  if(tipo == RISING || tipo == CHANGE)
    bitSet(conf_borda_sub[bit_grupo], bit_mascara);

  if(tipo == FALLING || tipo == CHANGE)
    bitSet(conf_borda_desc[bit_grupo], bit_mascara);
  
  bitSet(PCICR, bit_grupo);   // Habilita o grupo de pinos (são três)
  bitSet(*digitalPinToPCMSK(pino), bit_mascara);  // Habilita o pino dentro do grupo

  return true;
}

bool desabilita(const uint8_t pino)
{
  if(digitalPinToPCICR(pino) == NULL) // Pino inválido
    return false;
  
 if(digitalPinToInterrupt(pino) != NOT_AN_INTERRUPT)
  {
    detachInterrupt(digitalPinToInterrupt(pino));
    return true;
  }

  uint8_t bit_grupo = digitalPinToPCICRbit(pino);
  uint8_t bit_mascara = digitalPinToPCMSKbit(pino);

  bitClear(conf_borda_sub[bit_grupo], bit_mascara);
  bitClear(conf_borda_desc[bit_grupo], bit_mascara);
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

  bitClear(conf_borda_sub[bit_grupo], bit_mascara);
  bitClear(conf_borda_desc[bit_grupo], bit_mascara);

  if(tipo == RISING || tipo == CHANGE)
    bitSet(conf_borda_sub[bit_grupo], bit_mascara);

  if(tipo == FALLING || tipo == CHANGE)
    bitSet(conf_borda_desc[bit_grupo], bit_mascara);
}

bool modifica(const uint8_t pino, voidFuncPtr isr_ptr)
{
  if(digitalPinToPCICR(pino) == NULL) // Pino inválido
    return false;

  uint8_t bit_grupo = digitalPinToPCICRbit(pino);
  uint8_t bit_mascara = digitalPinToPCMSKbit(pino);  
  
  switch(bit_grupo)
  {
  case 0:
    isr_personalizada_B[bit_mascara] = isr_ptr;
    break;

  case 1:
    isr_personalizada_C[bit_mascara] = isr_ptr;
    break;

  case 2:
    isr_personalizada_D[bit_mascara] = isr_ptr;
  }
}

void permitirImediatas(bool permitir)
{
  limpar_flag = !permitir;
}

#define chamar(qual_bit,qual_isr) if(validos & qual_bit) ISR_PERS[qual_isr]()
#define ISR_PERS isr_personalizada_B

ISR(PCINT0_vect)
{
  uint8_t validos = (PORTB & ~PORTB_anterior & conf_borda_sub[0]) | (~PORTB & PORTB_anterior & conf_borda_desc[0]);
  
  chamar(0x01,0);
  chamar(0x02,1);
  chamar(0x04,2);
  chamar(0x08,3);
  chamar(0x10,4);
  chamar(0x20,5);
//chamar(0x40,6);	// No Arduino Uno, esses dois pinos estão
//chamar(0x80,7);	// ligados aos terminais do cristal oscilador
  
  PORTB_anterior = PORTB;
}

#define ISR_PERS isr_personalizada_C

ISR(PCINT1_vect)
{
  uint8_t validos = (PORTC & ~PORTC_anterior & conf_borda_sub[1]) | (~PORTC & PORTC_anterior & conf_borda_desc[1]);

  chamar(0x01,0);
  chamar(0x02,1);
  chamar(0x04,2);
  chamar(0x08,3);
  chamar(0x10,4);	
  chamar(0x20,5);
//chamar(0x40,6); // PC6 é o pino de Reset.
  
  PORTC_anterior = PORTC;
}

#define ISR_PERS isr_personalizada_D

ISR(PCINT2_vect)
{
  uint8_t validos = (PORTD & ~PORTD_anterior & conf_borda_sub[2]) | (~PORTD & PORTD_anterior & conf_borda_desc[2]);
  
  chamar(0x01,0);
  chamar(0x02,1);
//chamar(0x04,2); // PD2 e PD3 têm suas próprias ISR
//chamar(0x08,3);
  chamar(0x10,4);
  chamar(0x20,5);
  chamar(0x40,6);
  chamar(0x80,7);
  
  PORTD_anterior = PORTD;
}

#undef ISR_PERS
#undef chamar

void timer_isr()
{
  long tempo = millis();
  
  inicio.prox->isr();
  MsTimer2::stop();

  TimerISR* seguinte = inicio.prox->prox;
  delete inicio.prox;

  inicio.prox = seguinte;

  if(seguinte != NULL)
  {
    MsTimer2::set(inicio.prox->ms - millis(), timer_isr);
    MsTimer2::start();
  }
}

void agendar(voidFuncPtr isr, long ms)
{
  TimerISR *novo = new TimerISR;
  novo->ms = ms + millis();
  novo->isr = isr;
  novo->prox = NULL;

  if(inicio.prox == NULL)
    inicio.prox = novo;

  TimerISR* i;
  for(i = &inicio; (i->prox != NULL) && (novo->ms < i->prox->ms); i = i->prox);

  novo->prox = i->prox;
  i->prox = novo;

  MsTimer2::stop();
  MsTimer2::set(inicio.prox->ms - millis(), timer_isr);
  MsTimer2::start();
}

} // namespace Iterrompe

#endif  // Interrompe_h
