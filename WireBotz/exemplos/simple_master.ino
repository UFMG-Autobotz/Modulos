///////////////////////////////////////////////////////////////////////////////
//
//  Exemplo de uso da biblioteca WireBotz
//  Esse arquivo funciona junto de simple_slave.ino
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
//  FUNCIONAMENTO DA BIBLIOTECA (master)
//
//	A biblioteca utiliza dois buffers (espaços na memoria, criados como arrays
//  de bytes), um para transmissão e um para recepção de dados.
//
//	O buffer de recepcao é usado para guardar a resposta do escravo quando a
//  funcao Master.read é chamada. Ja o conteudo do buffer de escrita é enviado
//  ao escravo atraves da funcao Master.write 
//
///////////////////////////////////////////////////////////////////////////////


//É necessário usar os DOIS includes abaixo, na ordem que aparecem:
#include <Wire.h>
#include <WireBotzMaster.h>

//////////// Parametros do I2C ////////////

//Todo escravo no barramento tem um endereco
//O codigo do Arduino escravo deve estar usando o mesmo valor
#define slave_address 8
//Definimos aqui quantos bytes tem as menssagens enviadas (tx) e 
//recebidas (rx) tem
#define TX_MSG_SIZE 1
#define RX_MSG_SIZE 2

//////////// setup e loop ////////////

void setup()
{
	//Usaremos o Serial para mostrar o valor lido do potenciometro
	Serial.begin(9600);

	//Para usar o I2C, chamar Master.begin() durante setup
	Master.begin();
}

void loop()
{
	//As variaveis estaticas tem seus valores preservados entre
	//um loop e outro. Frequentemente é uma boa ideia usar buffers
	//estaticos, apesar de não ser estritamente necessario nesse exemplo
	static byte RXbuff[RX_MSG_SIZE];
	static byte TXbuff[TX_MSG_SIZE];

	int valorAnalogico;
	byte valorPWM;

	//Pedir ao escravo o valor lido no potenciometro
	Master.read(slave_address, RXbuff, RX_MSG_SIZE);
	//O valor lido por um Arduino é de 0 a 1023 e por isso foi divido em 2
	//bytes. O primeiro armazena os 8 bits mais significativos e o segundo
	//os menos.
	valorAnalogico = RXbuff[0] << 8;
	valorAnalogico = valorAnalogico + RXbuff[1];

	//A saida analogica do arduino deve receber valores de 0 a 255
	valorPWM = (valorAnalogico/1023.0)*255;

	//O valor calculado é enviado de volta ao escravo
	//o "(char)" é usado para conversao (cast) de inteiro para byte
	TXbuff[0] = (char) valorPWM;
	Master.write(slave_address, TXbuff, TX_MSG_SIZE);

	//Escreve valor lido do potenciometro no Serial
	Serial.println(valorAnalogico);

	//
	delay(10);
}
