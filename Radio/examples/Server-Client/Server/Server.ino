/* Este sketch tem como objetivo testar a funcionalidade do protocolo Servidor/Cliente
 * da biblioteca Radio.h. Deve ser carregado para o Arduíno que contém o Servidor.
 * 
 * Funcionamento: Recebe do Cliente um número e oferece a ele o serviço de converter
 * esse número para binário, retornando a ele o resultado como resposta.
 */

#include "Radio.h"

void conv_binario(char* req, char* resp); // Declaração da função serviço

void setup()
{
  Serial.begin(9600);   // Inicializa serial

  Radio::config("gato");  // Define o endereço do rádio deste Arduino como "gato"
  
  // Estabelece a comunicação com o outro Arduino, que é um Cliente cujo endereço é
  // "cao", armazenando-o na variavel my_client. A mensagem trocada terá 13 bytes.
  // Esta linha não pode vir antes da função Radio::config().
  Radio::Client my_client ("cao",13);

  my_client.addService(conv_binario); // Define o serviço que será requisitado
}

void loop()
{
  // Não precisa fazer nada, a função conv_binario sempre é chamada automaticamente
  // quando o Cliente envia uma requisição. 
}

void conv_binario(char* req, char* resp)  // Definição do serviço a ser fornecido
{
  int n = atoi(req);  // Converte a string recebida em número
  itoa(n, resp, 2);   // Converte o número em string, na base 2, armazenando em resp
                      // OBS: não é preciso alocar memória para resp
}
