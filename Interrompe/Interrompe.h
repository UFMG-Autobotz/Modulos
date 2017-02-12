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

using voidFuncPtr = void(*)();

namespace Interrompe
{
  
bool habilita(const uint8_t pino, voidFuncPtr isr_ptr, const byte tipo);
bool desabilita(const uint8_t pino);

bool modifica(const uint8_t pino, const uint8_t tipo);
bool modifica(const uint8_t pino, voidFuncPtr isr_ptr);

void permitirImediatas(bool permitir);

void agendar(voidFuncPtr isr, unsigned int ms);

} // namespace Interrompe

#endif  // Interrompe_h
