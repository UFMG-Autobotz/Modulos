/* Este sketch tem como objetivo testar a funcionalidade do protocolo Servidor/Cliente
 * da biblioteca Radio.h. Deve ser carregado para o Arduíno que contém o Cliente.
 * 
 * Funcionamento: Lê do serial um número inteiro de 0 a 4095 e o envia, via rádio, ao
 * Servidor, que realiza o serviço de convertê-lo para binário e devolver como resposta
 * o resultado da conversão, que é exibido de volta no monitor serial.
 */

#include "Radio.h"

void setup()
{
  Serial.begin(9600);   // Inicializa serial

  Radio::config("cao");  // Define o endereço do rádio deste Arduino como "cao"
}

// Estabelece a comunicação com o outro Arduino, que é um Servidor cujo endereço é
// "gato", armazenando-o na variavel my_client. A mensagem trocada terá 13 bytes.
// Esta linha não pode vir antes da função Radio::config().
Radio::Server my_server ("gato",13);

void loop()
{
  if(Serial.available())  // Caso haja nova entrada no serial
  {
    int n = Serial.parseInt();  // Lê o inteiro da entrada

    if(n >= 0 && n < 4096)  // Confere se a entrada é válida
    {
      char req[5];  // String que conterá a mensagens de requisição
      char *resp;   // Ponteiro para a string que conterá a mensagem de resposta
                    // OBS: Não é necessário alocar memória para esta string
      
      itoa(n, req, 10);  // Converte o inteiro em string (base 10)

      resp = my_server << req;    // Envia a requisição para o servidor e guarda a resposta
   // resp = my_server.send(req); // Formato equivalente

      Serial.println(resp);   // Imprime a resposta no serial
    }
    else
      Serial.println("Valor inválido");
  }
}
