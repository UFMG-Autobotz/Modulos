/* Traz para alto nível as interrupções do processador do ATmega328P
 *  
 * Versão 0.2: Adicionadas interrupções por timer
 * 
 * Autor: Daniel Leite Ribeiro
 *        Autobotz UFMG
 */

#ifndef Interrompe_h
#define Interrompe_h
  
#include <Arduino.h>


namespace Interrompe
{
  
using voidFuncPtr = void(*)(void*);

bool habilita(const uint8_t pino, const byte tipo, voidFuncPtr isr_ptr, void* arg = NULL);
bool desabilita(const uint8_t pino);

bool modifica(const uint8_t pino, const uint8_t tipo);
bool modifica(const uint8_t pino, voidFuncPtr isr_ptr);

void permitirImediatas(bool permitir);

} // namespace Interrompe

#endif  // Interrompe_h
