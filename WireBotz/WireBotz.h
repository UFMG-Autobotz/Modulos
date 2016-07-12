#ifndef _WIREBOTZ_H_
	#define _WIREBOTZ_H_

// A biblioteca WireBotz adiciona funcionalidades a Wire.h
//
// Ha duas classes: master e slave. Cada arduino deve usar apenas 
// 					uma delas de acordo com seu papel no bus
//
// Essa biblioteca eventualmente sera capaz de:
//    - Configurar o bus facilmente
//    - Lidar com o envio e recebimento de menssagens
//    - Oferecer conferencia de erros
//    - Envio e recebimento de menssagens maiores que 32 bytes
//

#include <Arduino.h>

// Library parameters
#define MASTER_MSG_SIZE 20  //size of message sent by master (bytes)
#define SLAVE_MSG_SIZE 20   //size of message 


class Master
{
public:
	void begin();
	//	Configura o Arduino como mestre no bus I2C

	int write(byte addr, uint8_t msg[], int size);
	//	Manda menssagem para um escravo
	//
	//Params:
	//	addr -> endereco do escravo (1 a 127)
	//	msg  -> buffer com a menssagem a ser enviada
	//	size -> numero de bytes a serem enviados
	//
	//Retorna:
	//	0  -> sucesso
	// -1  -> erro

	int read(byte addr, uint8_t msg[], int maxSize);
	//	Pede para que um escravo envie uma menssagem e armazena
	//	ela em msg.
	//
	//Params:
	//	addr -> endereco do escravo (1 a 127)
	//	msg  -> buffer onde a menssagem recebida será colocada
	//	maxSize -> tamanho do buffer em bytes
	//	
	//Retorna:
	//	numero de bytes lidos, caso a leitura tenha sido bem sucedida
	//	-1 caso um erro tenha acontecido
	//
};

class Slave
{
public:
	void begin(byte address);
	//	Configura o Arduino como escravo no bus I2C
	//
	//	A inicialização completa da biblioteca requer a chamada
	//	a setTXBuffer e/ou setRXBuffer, dependendo de se o seu
	//	escravo receberá ou transmitira dados ou ambos
	//
	//Parametros:
	//	address  -> o escravo tera esse endereco (1 a 127)

	void setTXBuffer(uint8_t buffPtr[], int size);
	//	Sempre que o mestre pedir, a biblioteca enviara o conteudo
	//	do buffer de transmissão (TX).  
	//	
	//Params:
	//	buffPtr -> ponteiro para a primeira posição do buffer
	//	size    -> tamanho do buffer em bytes	

	void setRXBuffer(uint8_t buffPtr[], int size);
	//	Sempre que o mestre enviar uma menssagem ela sera registrada
	//	no buffer de recepçao (RX).  
	//	
	//Params:
	//	buffPtr -> ponteiro para a primeira posição do buffer
	//	size    -> tamanho do buffer em bytes	

	bool newMessage();
	//	Indica se recebeu uma menssagem nova desde a última 
	//	chamada a essa funcao, retornando verdadeiro caso
	//	o mestre tenha enviado uma menssagem.

	void quiet(bool q);
	//  Ativa ou desativa a resposta ao mestre
	//	Quando a resposta esta desativada o escravo forcara um
	//	erro ao inves de mandar uma resposta valida
	//
	//Params:
	//	q -> true: desativa a resposta (stays quiet)
	//	     false: reativa a resposta

private:
	static uint8_t*  RXBuffer;
	static uint8_t*  TXBuffer;
	static int  RXSize;   //0 if no buffer registered
	static int  TXSize;   //0 if no buffer registered

	static bool  stayQuiet;    //diz se pode ou nao responder o mestre
	static bool  hasNewMessage;  //diz se recebeu uma menssagem nova

	
	//callback functions to handle master requests
	static void  sendData();    //answer master
	static void  receiveData(int );  //get data from master
};

#endif
