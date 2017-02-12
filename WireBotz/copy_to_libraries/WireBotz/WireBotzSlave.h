#ifndef WIREBOTZ_SLAVE_H
	#define WIREBOTZ_SLAVE_H

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

//
#include <Arduino.h>

class Slave_t
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

private:
	static uint8_t*  RXBuffer;
	static uint8_t*  TXBuffer;
	static int  RXSize;   //0 if no buffer registered
	static int  TXSize;   //0 if no buffer registered

	static bool  hasNewMessage;  //diz se recebeu uma menssagem nova

	
	//callback functions to handle master requests
	static void  sendData();    //answer master
	static void  receiveData(int n);  //get data from master
};

//declare Slave object
extern Slave_t Slave;

#endif
