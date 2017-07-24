/* Radio.cpp
 * v0.1
 *
 * Classes para comunicação entre Arduínos via rádio
 *
 * Autor: Daniel Leite Ribeiro
 *        Autobotz UFMG
 */

#include <Mirf.h>
#include <MirfHardwareSpiDriver.h>
#include "Radio.h"

namespace Radio
{

static Radio* talker_vec[5];         // Ponteiros para os cinco rádios que comunicam com este Arduino
static Radio* last_sent;             // Ponteiro para o último rádio prara o qual se enviou mensagem
static bool podeEnviar();            // Verifica se não há outro rádio enviando
static uint8_t irqPin = 2;           // Pino de interrupção
static uint8_t EN_RX_atual;          // Valor atual do registrador que habilita os pipes
static byte this_addr[5];            // Endereço deste rádio
static int listener_cnt, talker_cnt; // Quantidade de talkers e de listeners
static uint32_t talker_addr_vec[5], *listener_addr_vec = new uint32_t[0]; // Vetores com os endereços dos talkers e listeners

enum : uint8_t  // Bytes de controle
{
  PIPENUM_REQUEST         = '&',
  PIPENUM_RESPONSE        = '*',
  NOT_MY_PIPE             = (uint8_t) -1,
  VALIDATION_REQUEST      = '?',
  VALIDATION_RESPONSE_YES = '!',
  VALIDATION_RESPONSE_NO  = '#',
  SLAVE_REQUEST           = (uint8_t) -1,
  SLAVE_RESPONSE_VALID    = '.',
  SLAVE_RESPONSE_INVALID  = ',',
  SERVER_RESPONSE_VALID   = '>',
  SERVER_RESPONSE_INVALID = '<'
};

void config(const uint8_t* addr)
{
  *(uint32_t*) this_addr = *(uint32_t*) addr;

  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.ceLow();

  for(int i = 5; i >= 0; --i)
  {
    this_addr[4] = i;
    Mirf.writeRegister(RX_ADDR_P0 + i, this_addr, mirf_ADDR_LEN);
  }
  
  Mirf.configRegister(EN_RXADDR, EN_RX_atual = bit(0));
  Mirf.ceHi();
  
  Mirf.payload = mirf_ADDR_LEN;
  Mirf.config();

  if(irqPin != -1)
    attachInterrupt(digitalPinToInterrupt(irqPin), spin, LOW);
}

void pinout(int ce, int csn, int irq = -1)
{
  Mirf.cePin = ce;
  Mirf.csnPin = csn;
  irqPin = irq;
}

bool podeEnviar()
{
  uint8_t cd_val;
  Mirf.readRegister(CD,&cd_val,1);
  
  return !cd_val;
}

void spin()
{
  uint8_t status = Mirf.getStatus();

  while(status & bit(RX_DR) || (status & 0b1110) != 0b1110)
  {
    if(int pipe = status & 0b1110 >> 1)
      talker_vec[pipe-1]->lerMsg();
    else
      Radio::cmd();

    status = Mirf.getStatus();
  }

  if(status & bit(TX_DS))
  {
    bitSet(status,TX_DS);

    Mirf.writeRegister(RX_ADDR_P0, this_addr, mirf_ADDR_LEN);
    Mirf.powerUpRx();

    if(last_sent)
      last_sent->ok = true;
  }

  if(status & bit(MAX_RT))
  {
    bitSet(status,MAX_RT);

    if(last_sent)
    {
      last_sent->ok = false;
      last_sent->esperando_resposta = false;
    }
  }
}

/* -------------------- RADIO -------------------- */

Radio::Radio(byte* address, unsigned int payload, bool listener) :
  payload (min(payload,31)),
  rx_cnt(0),
  tx_cnt(0),
  ok(false)
{
  if(talker_cnt >= 5)
    return;
    
  uint32_t* new_vec = new uint32_t[listener_cnt+1];

  for(int i = 0; i < listener_cnt; ++i)
    new_vec[i] = listener_addr_vec[i];

  new_vec[listener_cnt++] = *(uint32_t*) addr;
  
  delete[] listener_addr_vec;
  listener_addr_vec = new_vec;
  
  txbuf = new uint8_t[max(payload+1,mirf_ADDR_LEN)] + 1;
  rxbuf = new uint8_t[max(payload+1,mirf_ADDR_LEN)] + 1;

  *(uint32_t*) addr = *(uint32_t*) address;
  requestAddress();

  if(listener)
    return;
  
  talker_addr_vec[talker_cnt] = *(uint32_t*) address;
  talker_vec[talker_cnt++] = this;
  
  Mirf.configRegister(RX_PW_P0 + talker_cnt, payload);
  Mirf.configRegister(EN_RXADDR, bitSet(EN_RX_atual, talker_cnt));
}

Radio::~Radio()
{
  delete[] rxbuf;
}

void Radio::cmd()
{
  uint8_t cmd_buf[mirf_ADDR_LEN];
  uint32_t addr, i;
  byte tx_addr[5];

  Mirf.getData(cmd_buf);

  switch(cmd_buf[0])
  {
  case PIPENUM_REQUEST:

    addr = *(uint32_t*)(cmd_buf+1);
    for(i = 0; i < 5 && talker_addr_vec[i] != addr; ++i);

    cmd_buf[0] = PIPENUM_RESPONSE;
    cmd_buf[1] = (i < 5) ? i : NOT_MY_PIPE;

    *(uint32_t*) tx_addr = addr;
    tx_addr[4] = 0;

    Mirf.setTADDR(tx_addr);
    Mirf.payload = mirf_ADDR_LEN;
    Mirf.send(cmd_buf);

    if(i < 5)
      last_sent = talker_vec[i];
    else
      last_sent = NULL;

    break;

  case PIPENUM_RESPONSE:

    if(cmd_buf[1] != NOT_MY_PIPE)
    {
      last_sent->addr[4] = cmd_buf[1];
      last_sent->ok = true;
    }
    else
      last_sent->ok = false;

    last_sent->esperando_resposta = false;
    break;

  case VALIDATION_REQUEST:

    addr = *(uint32_t*)(cmd_buf+1);
    for(i = 0; i < listener_cnt && listener_addr_vec[i] != addr; ++i);

    cmd_buf[0] = (i < listener_cnt) ? VALIDATION_RESPONSE_YES : VALIDATION_RESPONSE_NO;

    *(uint32_t*) tx_addr = addr;
    tx_addr[4] = 0;

    Mirf.setTADDR(tx_addr);
    Mirf.payload = mirf_ADDR_LEN;
    Mirf.send(cmd_buf);

    last_sent = NULL;
    break;

  case VALIDATION_RESPONSE_YES:

    last_sent->ok = true;
    last_sent->esperando_resposta = false;
    break;

  case VALIDATION_RESPONSE_NO:

    last_sent->ok = false;
    last_sent->esperando_resposta = false;
  }
}

void Radio::send()
{
  Mirf.setTADDR(addr);
  Mirf.payload = payload; 
  Mirf.send(txbuf-1);
  last_sent = this;
}

void Radio::requestAddress()
{
  byte tx_addr[5];
  *(uint32_t*) tx_addr = *(uint32_t*) addr;
  tx_addr[4] = 0;

  *(uint32_t*) txbuf = *(uint32_t*) this_addr;
  txbuf[-1] = PIPENUM_REQUEST;

  Mirf.setTADDR(tx_addr);
  Mirf.payload = mirf_ADDR_LEN;
  Mirf.send(txbuf-1);
  last_sent = this;

  esperando_resposta = true;
  while(esperando_resposta);
}

bool Radio::connected()
{
  byte tx_addr[5];
  *(uint32_t*) tx_addr = *(uint32_t*) addr;
  tx_addr[4] = 0;

  *(uint32_t*) txbuf = *(uint32_t*) this_addr;
  txbuf[-1] = VALIDATION_REQUEST;

  Mirf.setTADDR(tx_addr);
  Mirf.payload = mirf_ADDR_LEN;
  Mirf.send(txbuf-1);
  last_sent = this;

  esperando_resposta = true;
  while(esperando_resposta);

  return ok;
}

/* -------------------- TALKER -------------------- */

Talker::Talker(uint8_t* addr, int payload) :
  Radio(addr, payload),
  buffers(new uint8_t*[0]),
  qtde_buffers(0),
  nova_msg(-1)
{
  connected();
}

Talker::~Talker()
{
  disconnect();
}

int Talker::addBuffer(uint8_t* &buf)
{
  uint8_t** new_buf_vec = new uint8_t*[qtde_buffers+1];
  buf = new uint8_t[payload];

  for(int i = 0; i < qtde_buffers; ++i)
    new_buf_vec[i] = buffers[i];

  new_buf_vec[qtde_buffers] = buf;
  
  delete[] buffers;
  buffers = new_buf_vec;

  return qtde_buffers++;
}

void Talker::lerMsg()
{
  Mirf.getData(rxbuf-1);
  uint8_t buf_num = rxbuf[-1];
  
  if(buf_num >= qtde_buffers)
    return;
  
  for(int i = 0; i < payload; ++i)
    buffers[buf_num][i] = rxbuf[i];

  ++rx_cnt;
  nova_msg = buf_num;
}

void Talker::disconnect()
{
  for(int i = 0; i < qtde_buffers; ++i)
    delete[] buffers[i];
  
  delete[] buffers;
  ok = false;
}

int Talker::newMessage()
{
  int ret = nova_msg;
  nova_msg = -1;
  return ret;
}

/* -------------------- LISTENER -------------------- */

void Listener::send(uint8_t* msg, int buf_num)
{
  if(!msg)
    return;
  
  txbuf[-1] = buf_num;

  for(int i = 0; i < payload; ++i)
    txbuf[i] = msg[i];

  Radio::send();

  if(ok)
    ++tx_cnt;
}

Listener::Stream& operator << (const Listener::Stream& lhs, uint8_t* msg)
{
  Listener::Stream& s = const_cast<Listener::Stream&>(lhs);
  s.list.send(msg, s.n++);
  return s;
}

/* -------------------- BILATERAL -------------------- */

Bilateral::Bilateral(uint8_t* addr, int payload) : 
  Talker(addr,payload),
  Listener(addr, payload),
  Radio(addr, payload)
{}

/* -------------------- SLAVE -------------------- */

Slave::Slave(uint8_t* addr, int payload) :
  Radio(addr,payload),
  lookup_head(NULL)
{}

Slave::~Slave()
{
  disconnect();
}

void Slave::disconnect()
{
  lookup *it = lookup_head, *next;

  while(it != NULL)
  {
    next = it->next;
    delete it;
    it = next;
  }

  ok = false;
}

void Slave::send(uint8_t* msg, int buf_num)
{
  if(!msg)
    return;
  
  txbuf[-1] = buf_num;

  for(int i = 0; i < payload; ++i)
    txbuf[i] = msg[i];

  Radio::send();

  if(ok)
    ++tx_cnt;
}

uint8_t* Slave::read(int buf_num)
{ 
  txbuf[-1] = SLAVE_REQUEST;
  txbuf[0] = buf_num;
  Radio::send();
  
  esperando_resposta = true;
  while(esperando_resposta);

  if(!ok || rxbuf[-1] != SLAVE_RESPONSE_VALID)
    return NULL;

  lookup* it;
  for(it = lookup_head; it != NULL && it->n != buf_num; it = it->next);
  
  if(it == NULL)
  {
    it = new lookup;
    it->n = buf_num;
    it->buf = new uint8_t[payload];
    it->next = NULL;
  }
  
  for(int i = 0; i < payload; ++i)
    it->buf[i] = rxbuf[i];

  ++rx_cnt;
  return it->buf;
}

Slave::Stream& operator << (const Slave::Stream& lhs, uint8_t* msg)
{
  Slave::Stream& s = const_cast<Slave::Stream&>(lhs);
  s.slav.send(msg, s.n++);

  return s;
}

Slave::Stream& operator >> (const Slave::Stream& lhs, uint8_t* &buf)
{
  Slave::Stream& s = const_cast<Slave::Stream&>(lhs);
  
  if(buf)
  {
    uint8_t* msg = s.slav.read(s.n);
    
    for(int i = 0; i < s.slav.payload; ++i)
      buf[i] = msg[i];
  }
    
  ++s.n;
  return s;
}

void Slave::lerMsg()
{
  Mirf.getData(rxbuf-1);
  esperando_resposta = false;
}

/* -------------------- MASTER -------------------- */

Master::Master(uint8_t* addr, int payload) :
  Radio(addr,payload),
  qtde_rx_buffers(0),
  qtde_tx_buffers(0),
  rx_buffers(new uint8_t*[0]),
  tx_buffers(new uint8_t*[0]),
  nova_msg(-1)
{
  connected();
}

Master::~Master()
{
  disconnect();
}

void Master::disconnect()
{
  for(int i = 0; i < qtde_rx_buffers; ++i)
    delete[] rx_buffers[i];

  for(int i = 0; i < qtde_tx_buffers; ++i)
    delete[] tx_buffers[i];
      
  delete[] rx_buffers;
  delete[] tx_buffers;
  
  ok = false;
}

int Master::addRxBuffer(uint8_t* &buf)
{
  uint8_t** new_buf_vec = new uint8_t*[qtde_rx_buffers+1];
  buf = new uint8_t[payload];

  for(int i = 0; i < qtde_rx_buffers; ++i)
    new_buf_vec[i] = tx_buffers[i];

  new_buf_vec[qtde_rx_buffers] = buf;
  
  delete[] rx_buffers;
  rx_buffers = new_buf_vec;

  return qtde_rx_buffers++;
}

int Master::addTxBuffer(uint8_t* &buf)
{
  uint8_t** new_buf_vec = new uint8_t*[qtde_tx_buffers+1];
  buf = new uint8_t[payload];

  for(int i = 0; i < qtde_tx_buffers; ++i)
    new_buf_vec[i] = rx_buffers[i];

  new_buf_vec[qtde_tx_buffers] = buf;
  
  delete[] tx_buffers;
  tx_buffers = new_buf_vec;

  return qtde_tx_buffers++;
}

int Master::newMessage()
{
  int ret = nova_msg;
  nova_msg = -1;
  return ret;
}

void Master::lerMsg()
{
  Mirf.getData(rxbuf-1);
  uint8_t buf_num = rxbuf[-1];
  
  if(buf_num == SLAVE_REQUEST)
  {
    buf_num = rxbuf[0];

    if(buf_num < qtde_tx_buffers)
    {
      for(int i = 0; i < payload; ++i)
        txbuf[i] = tx_buffers[buf_num][i];
  
      txbuf[-1] = SLAVE_RESPONSE_VALID;
      ++tx_cnt;
    }
    else
      txbuf[-1] = SLAVE_RESPONSE_INVALID;

    Radio::send();      
    return;
  }
  
  if(buf_num >= qtde_rx_buffers)
    return;
  
  for(int i = 0; i < payload; ++i)
    rx_buffers[buf_num][i] = rxbuf[i];

  ++rx_cnt;
  nova_msg = buf_num;
}

/* -------------------- SERVER -------------------- */

Server::Server(uint8_t* addr, int payload) :
  Radio(addr,payload),
  lookup_head(NULL)
{}

Server::~Server()
{
  disconnect();
}

void Server::disconnect()
{
  lookup *it = lookup_head, *next;

  while(it != NULL)
  {
    next = it->next;
    delete it;
    it = next;
  }

  ok = false;
}
  
uint8_t* Server::request(uint8_t* req, int srv_num)
{  
  if(!req)
    return NULL;
  
  txbuf[-1] = srv_num;

  for(int i = 0; i < payload; ++i)
    txbuf[i] = req[i];
  
  Radio::send();
  
  esperando_resposta = true;
  while(esperando_resposta);

  if(!ok || rxbuf[-1] != SERVER_RESPONSE_VALID)
    return NULL;

  lookup* it;
  for(it = lookup_head; it != NULL && it->n != srv_num; it = it->next);
  
  if(it == NULL)
  {
    it = new lookup;
    it->n = srv_num;
    it->buf = new uint8_t[payload];
    it->next = NULL;
  }
  
  for(int i = 0; i < payload; ++i)
    it->buf[i] = rxbuf[i];

  ++rx_cnt;
  return it->buf;
}

void Server::lerMsg()
{
  Mirf.getData(rxbuf-1);
  esperando_resposta = false;
}
  
uint8_t* operator << (const Server::Stream& lhs, uint8_t* req)
{
  return lhs.serv.request(req, lhs.n);
}

/* -------------------- CLIENT -------------------- */

Client::Client(uint8_t* addr, int payload) :
  Radio(addr,payload),
  qtde_srv(0),
  services(new srv_t[0])
{}

Client::~Client()
{
  disconnect();
}

void Client::disconnect()
{
  delete[] services;
  ok = false;
}

int Client::addService(srv_t srv)
{
  srv_t* new_srv_vec = new srv_t[qtde_srv+1];

  for(int i = 0; i < qtde_srv; ++i)
    new_srv_vec[i] = services[i];

  new_srv_vec[qtde_srv] = srv;
  
  delete[] services;
  services = new_srv_vec;

  return qtde_srv++;
}

void Client::lerMsg()
{
  Mirf.getData(rxbuf-1);
  unsigned int srv_num = rxbuf[-1];

  if(srv_num < qtde_srv)
  {
    services[srv_num](rxbuf,txbuf);
    txbuf[-1] = SERVER_RESPONSE_VALID;
    ++tx_cnt;
  }
  else
    txbuf[-1] = SERVER_RESPONSE_INVALID;

  Radio::send();
}

}
