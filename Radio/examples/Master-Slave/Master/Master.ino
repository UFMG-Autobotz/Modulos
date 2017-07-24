/* Este sketch tem como objetivo testar a funcionalidade do protocolo Mestre/Escravo
 * da biblioteca Radio.h. Deve ser carregado para o Arduíno que contém o Mestre.
 * 
 * Funcionamento: Lê um valor do serial e envia para o Escravo este valor como duração
 * em milissegundos dos tempos em nível alto e baixo de um LED que ele mantémpiscando.
 * Além disso, caso se insira 'P' no serial, o mestre requisita do escravo o valor da
 * tensão em um potenciômetro conectado ao seu pino analógico.
 */

#include "Radio.h"

void setup()
{
  Serial.begin(9600); // Iniciaiza serial

  Radio::config("mar"); // Define o endereço do rádio deste Arduino como "mar"
}

// Estabelece a comunicação com o outro Arduino, que é um Escravo cujo endereço é
// "rio", armazenando-o na variavel my_slave. A mensagem trocada terá 5 bytes.
// Esta linha não pode vir antes da função Radio::config().
Radio::Slave my_salve ("rio",5);

void loop() {}
