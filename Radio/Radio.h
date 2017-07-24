/* Radio.h
 * v0.1
 *
 * Classes para comunicação entre Arduínos via rádio
 *
 * Autor: Daniel Leite Ribeiro
 *        Autobotz UFMG
 */

#ifndef RADIO_H
#define RADIO_H

namespace Radio
{
// --------------------------------- FUNÇÕES COMUNS ---------------------------------

// Configura este Arduino como um rádio
void config(const uint8_t* addr);

// Opcional: muda a pinagem padão do Mirf (CE=8, CSN=7, IRQ=2)
void pinout(int ce, int csn, int irq);

// Função executada automaticamente a cada interrupção. Não é necessário chamá-la.
void spin();
// ----------------------------------------------------------------------------------

// Tipo do ponteiro para as funções usadas como serviço pelo Server
using srv_t = void(*)(uint8_t*,uint8_t*);

// Classe abstrata da qual todas as outras derivam.
// Representa um dispositivo de radio com um qual este Arduino se comunica
class Radio
{
public:

  // Confere se a conexão está ativa
  virtual bool connected();

protected:

  Radio(byte* address, unsigned int payload, bool listener = false);
  Radio(Radio&) = delete;
  ~Radio();
  
  byte addr[5];
  const uint8_t payload;
  uint8_t *txbuf, *rxbuf;
  volatile unsigned long rx_cnt, tx_cnt;
  volatile bool ok, esperando_resposta;

  void send();
  void requestAddress();
  virtual void lerMsg() = 0;
  static void cmd();
  friend void spin();
};

// ------------------------------------- TALKER -------------------------------------

// Apenas envia mensagens para este Arduino (comunicação unilateral)
// Este Arduíno não manda mensagens para o Talker
// Os buffers são atualizados automaticamente a cada mensagem
class Talker : public virtual Radio
{
public:

  // Construído informando-se o endereço e o tamanho da mensagem
  Talker(uint8_t* addr, int payload);
  ~Talker();
  
  // Determina 'buf' como um dos buffers de recebimento (preenchidos automaticamente quando
  // o Talker enviar uma mensagem). Os buffers são numerados sequencialmente de acordo com
  // a ordem em que forem adicionados. A função retorna o número (ID) do buffer adicionado
  int addBuffer(uint8_t* &buf);

  // Retorna a quantidade de buffers adicionados até o momento
  int numBuffers() { return qtde_buffers; }

  // Se o Talker tiver enviado uma nova mensagem, retorna o número (ID) do buffer
  // no qual a mensagem foi preenchida. Se não houver nova mensagem, retorna -1.
  int newMessage();

  // Retorna a quantidade de mensagens enviadas pelo Talker até o momento
  int rxCount() { return rx_cnt; }
  
  // Vetor com os buffers adicionados
  uint8_t** buffers;

private:
  void disconnect();
  unsigned int qtde_buffers;
  int nova_msg;

protected:
  void lerMsg();
};

// ------------------------------------ LISTENER ------------------------------------

// Apenas recebe mensagens deste Arduino (comunicação unilateral)
// Este Arduíno não recebe mensagens do Listener
class Listener : public virtual Radio
{
  class Stream; // Classe auxiliar para os operadores [] e <<

public:

  // Construído informando-se o endereço e o tamanho da mensagem
  Listener(uint8_t* addr, int payload) : Radio(addr, payload, true) {};
  
  // Envia a mensagem 'msg' para o buffer 'buf_num' (padrão: 0) do Listener
  void send(uint8_t* msg, int buf_num = 0);

  // Retorna a quantidade de mensagens que já foram enviadas até agora
  int txCount() { return tx_cnt; }
  
  // list[n] << msg; envia a mensagem 'msg' para o buffer 'n' do Listener 'list'
  Stream operator [] (int n) { return {*this, n}; }
  friend Stream& operator << (const Stream& lhs, uint8_t* msg);

private:

  void lerMsg() {};
  
  class Stream
  {
    const Listener& list;
    int n;
  
  public:
    Stream(const Listener& list, int n = 0) : list(list), n(n) {}
    friend Stream& operator << (const Stream& lhs, uint8_t* msg);
  };
};

// ------------------------------------ BILATERAL ------------------------------------

// Comunicação bilateral: pode enviar e receber mensagens deste Arduino
// Mesmo princípio de funcionamento do Talker e do Listener combinados
class Bilateral : public Talker, public Listener
{
public:

  // Construído informando-se o endereço e o tamanho da mensagem
  Bilateral(uint8_t* addr, int payload);

  /* 
   * Compartilha de todas as funções do Talker e do Listener, incluindo operadores
   */

private:
  void lerMsg() { Talker::lerMsg(); }
};

// -------------------------------------- MASTER -------------------------------------

// Mestre: envia mensagens para este Arduíno e lê dele quando quiser
// Os buffers de recebimento são atualizados automaticamente a cada nova mensagem
// O conteúdo dos buffers de envio é enviado automaticamente quando solicitado
class Master : public Radio
{
public:

  // Construído informando-se o endereço e o tamanho da mensagem
  Master(uint8_t* addr, int payload);
  ~Master();

  // Determinam 'buf' como um dos buffers de recebimento ou de envio, respectivamente.
  // Retorna o número (ID) do buffer adicionado (numerados sequencialmente)
  int addRxBuffer(uint8_t* &buf);
  int addTxBuffer(uint8_t* &buf);

  // Retornam a quantidade de buffers de recebimento e de envio, respectivamente
  int numRxBuffers() { return qtde_rx_buffers; }
  int numTxBuffers() { return qtde_tx_buffers; }

  // Se o mestre tiver enviado uma nova mensagem, retorna o número (ID) do buffer
  // no qual a mensagem foi preenchida. Se não houver nova mensagem, retorna -1.
  int newMessage();

  // Retornam a quanditade de mensagens recebidas e enviadas, respectivamente
  int rxCount() { return rx_cnt; }
  int txCount() { return tx_cnt; }
  
  // Vetores com os buffers adicionados (de rebebimento e envio, respectivamente) 
  uint8_t **rx_buffers, **tx_buffers;

private:
  void lerMsg();
  void disconnect();
  unsigned int qtde_rx_buffers, qtde_tx_buffers;
  int nova_msg;
};

// -------------------------------------- SLAVE --------------------------------------

// Escravo: recebe mensagens deste Arduino e envia somente quando requisitado
class Slave : public Radio
{
  class Stream; // Classe auxiliar para os operadores [] e <<
  
public:

  // Construído informando-se o endereço e o tamanho da mensagem
  Slave(uint8_t* addr, int payload);
  ~Slave();

  // Envia a mensagem 'msg' para o rx-buffer 'buf_num' (padrão: 0) do escravo
  void send(uint8_t* msg, int buf_num = 0);

  // Lê a mensagem contida no tx-buffer 'buf_num' (padrão: 0) do escravo
  uint8_t* read(int buf_num = 0);  

  // Retornam a quanditade de mensagens enviadas e recebidas, respectivamente
  int txCount() { return tx_cnt; }
  int rxCount() { return rx_cnt; }
  
  // slav[n] << msg; envia a mensagem 'msg' para o rx-buffer 'n' do escravo 'slav'
  // slav[n] >> buf; lê a mensagem do tx-buffer 'n' e armazena em 'buf'
  Stream operator [] (int n) { return {*this, n}; }
  friend Stream& operator << (const Stream& lhs, uint8_t* msg);
  friend Stream& operator >> (const Stream& lhs, uint8_t* &msg);

private:

  void lerMsg();
  void disconnect();

  struct lookup
  {
    uint8_t n;
    uint8_t* buf;
    lookup* next;
  }
  *lookup_head;
  
  class Stream
  {
    const Slave& slav;
    int n;
  
  public:
    Stream(const Slave& slav, int n = 0) : slav(slav), n(n) {}
    friend Stream& operator << (const Stream& lhs, uint8_t* msg);
    friend Stream& operator >> (const Stream& lhs, uint8_t* &msg);
  };
};

// -------------------------------------- SERVER -------------------------------------

// Servidor: fornece um serviço para este Arduino (o cliente).
// Este Arduino manda uma mensagem (request), o servidor a processa e
// responde com o resultado do serviço (response)
class Server : public Radio
{
  class Stream; // Classe auxiliar para os operadores [] e <<
  
public:

  // Construído informando-se o endereço e o tamanho da mensagem
  Server(uint8_t* addr, int payload);
  ~Server();

  // Requisita o processamento da mensagem 'req' e retorna a resposta do servidor.
  // O parâmetro 'srv_num' indica número (ID) do serviço desejado (padrão: 0)
  uint8_t* request(uint8_t* req, int srv_num = 0);

  // Retorna a quantidade de serviços respondidos até agora
  int rxCount() { return rx_cnt; }

  // resp = serv[n] << req; envia a requisição 'req' para ser processada pelo 
  // serviço 'n' do servidor 'serv', e retorna a resposta
  Stream operator [] (int n) { return {*this, n}; }
  friend uint8_t* operator << (const Stream& lhs, uint8_t* msg);

private:

  void lerMsg();
  void disconnect();
  
  struct lookup
  {
    uint8_t n;
    uint8_t* buf;
    lookup* next;
  }
  *lookup_head;
  
  class Stream
  {
    const Server& serv;
    int n;
  
  public:
    Stream(const Server& serv, int n = 0) : serv(serv), n(n) {}
    friend uint8_t* operator << (const Stream& lhs, uint8_t* msg);
  };
};

// -------------------------------------- CLIENT -------------------------------------

// Cliente: requisita serviços deste Arduino (o servidor).
// O serviço é processado e enviado automaticamente ao ser requisitado pelo cliente.
class Client : public Radio
{
public:

  // Construído informando-se o endereço e o tamanho da mensagem
  Client(uint8_t* addr, int payload);
  ~Client();

  // Adiciona uma função do tipo 'void(uint8_t* req, uint8_t* resp)' que processa a
  // mensagem 'req' e armazena a resposta em 'resp'. Retorna o ID sequencial do serviço
  int addService(srv_t srv);

  // Retorna a quantidade de serviços adicionados
  int numServices() { return qtde_srv; }

  // Retorna o número de serviços requisitados
  int txCount() { return tx_cnt; }

  // Vetor com os serviços adiconados
  srv_t* services;

private:
  void lerMsg();
  void disconnect();
  int qtde_srv;
};

// ----------------------------------------------------------------------------------
}

#endif // RADIO_H
