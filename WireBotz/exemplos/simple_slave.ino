///////////////////////////////////////////////////////////////////////////////
//
//  Exemplo de uso da biblioteca WireBotz
//  Esse arquivo funciona junto de simple_master.ino
//
//  VISAO GERAL e MONTAGEM
//
//  Estes programas implementam o exemplo clássico de um LED com o brilho
//  controlado por potenciômetro. A inovação é que um arduino (slave) lê o
//  valor do potenciômetro (0 a 1024) que é enviado ao outro arduino (master)
//  No master o valor é convertido para um inteiro entre 0 e 255 e enviado de
//  volta ao escravo, que finalmente ajusta o brilho do LED
//                          _________________               __________________
//      Potenciometro ====>|                 |     I2C     |                  |
//                         | Arduino (slave) |<----------->| Arduino (master) |
//           LED <=========|_________________|             |__________________|
//
//  O codigo assume os seguintes pinos na montagem:
//  Slave:
//     3: conectado ao anodo do LED
//    A0: conectado ao pino central do potenciomentro
//    A4: I²C SDA (linha de dados)
//    A5: I²C SDC (linha de clock)
//  Master:
//    A4: I²C SDA (linha de dados)
//    A5: I²C SDC (linha de clock)
//
//  Dizer que os pinos A4 do slave e do master estão conectados na linha SDA
//  significa que os pinos A4 do arduino serão conectados um ao outro. Além
//  disso um resistor de pull up (na faixa 1k ~ 10k) deve ser conectado a cada
//  uma das linhas (SDA e SCL).
//
//  FUNCIONAMENTO DA BIBLIOTECA (slave)
//
//	A biblioteca utiliza dois buffers (espaços na memoria, criados como arrays
//  de bytes), um para transmissão e um para recepção de dados.
//
//	No I2C, os escravos nao podem iniciar comunicacoes, dependendo do mestre.
//	O mestre pode transmitir dados ou fazer um pedido para que o escravo envie
//  dados. O escravo deve lidar imediatamente com os comandos do mestre e por
//  isso callbacks são usados no escravo.
//
//	Há um callback que copia os dados recebidos para o buffer de recepção e um
//  que envia ao mestre os conteudos do buffer de envio. Essas funções já são
//  implementadas pela biblioteca, bastando registrar os buffers durante o
//  setup(). No entanto, é preciso ter em mente que durante a execução do loop
//  o código poderá ser interrompido.
//	 
///////////////////////////////////////////////////////////////////////////////

//É necessário usar os DOIS includes abaixo, na ordem que aparecem:
#include <Wire.h>
#include <WireBotzSlave.h>

//////////// Parametros do I2C ////////////

//Todo escravo no barramento tem um endereco, esse sera o nosso
//O codigo do Arduino mestre deve estar usando o mesmo valor
#define slave_address 8
//Definimos aqui quantos bytes tem as menssagens enviadas (tx) e 
//recebidas (rx)
#define TX_MSG_SIZE 2
#define RX_MSG_SIZE 1  //note que TX no mestre é RX no escravo e vice versa

//Os buffers de envio e transmissão (uint8_t = unsigned inteiro de 8 bits)
uint8_t RXBuff[MSG_SIZE];
uint8_t TXBuff[MSG_SIZE];

//////////// setup e loop ////////////
void setup()
{
	//configurar I2C
	//primeiramente, chamamos Slave.begin(). Teremos o endereço slave_address.
	Slave.begin(slave_address);
	//Os buffers de envio de dados (TX) e recepcao (RX) são registrados
	Slave.setRXBuffer(RXBuff, MSG_SIZE);
	Slave.setTXBuffer(TXBuff, MSG_SIZE);

	//configurar pinos
	pinMode(3, OUTPUT); //LED
	pinMode(A0, INPUT); //potenciometro
}

void loop()
{
	static int valorPot;
	static int valorLED;

	//Ler o valor do potenciometro e escrever no LED
	analogWrite(3, valorLED);
	valorLED = analogRead(A0);

	//O valor que foi lido no potenciometro tem que ser passado para
	//o mestre. Para isso colocamos o mesmo em RXbuff e esperamos até
	//que o mestre peça ao escravo que envie seus dados o que pode ou
	//não acontecer antes do próximo loop.
	//
	//Como valorLED vai de 0 a 1023, é preciso usar 2 bytes. TXBuff[0]
	//ficara com os bits mais significativos e TXBuff[1] com os menos.
	TXBuff[0] = (uint8_t) valorPot;  //os bytes mais significativos
	                                 //são descartados na conversão
	TXBuff[1] = (uint8_t) (valorPot >> 8);

	//O mestre pode ou não ter enviado um novo valor para colocar no LED
	//
	//A função Slave.newMessage() retornará verdadeiro quando uma menssagem
	//nova tiver chegado do mestre.
	//Note que a função retornará verdadeiro apenas uma vez e depois
	//retornará falso até que a próxima menssagem chegue
	if(Slave.newMessage())
	{
		//pegar o valor de intensidade do LED
		valorLED = RXBuff[0];
	}



	delay(10);
}
