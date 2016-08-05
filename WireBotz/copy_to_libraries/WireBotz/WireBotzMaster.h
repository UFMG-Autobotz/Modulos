#ifndef WIREBOTZ_MASTER_H
	#define WIREBOTZ_MASTER_H

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

//compilation options:
#ifndef WIREBOTZ_LARGE_MSG
	#define WIREBOTZ_LARGE_MSG 0
#endif
#ifndef WIREBOTZ_ERR_CHECK
	#define WIREBOTZ_ERR_CHECK 0
#endif

//
#include <Arduino.h>

class Master_t
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

//declare Master object
extern Master_t Master;

#endif