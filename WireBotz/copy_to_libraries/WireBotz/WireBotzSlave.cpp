// Implementação da classe Slave na biblioteca WireBotz

#include <Wire.h>
#include <WireBotzSlave.h>

// Declare static members of class
uint8_t* Slave_t::RXBuffer;
uint8_t* Slave_t::TXBuffer;
int      Slave_t::RXSize;   //0 if no buffer registered
int      Slave_t::TXSize;   //0 if no buffer registered
uint16_t Slave_t::RXCnt;
uint16_t Slave_t::TXCnt;

bool     Slave_t::hasNewMessage;  //diz se recebeu uma menssagem nova

// Define member functions
void Slave_t::begin(byte address)
{
	Wire.begin(address);   //join the bus as slave

	Wire.onRequest(Slave_t::sendData);    //callback to answer master
	Wire.onReceive(Slave_t::receiveData);     //callBack to receive data from master

	//set private variables to default values
	Slave_t::hasNewMessage = false;

	Slave_t::TXSize = 0;
	Slave_t::RXSize = 0;

  Slave_t::TXCnt = 0;
  Slave_t::RXCnt = 0;
}

void Slave_t::setTXBuffer(uint8_t buffPtr[], int size)
{
	Slave_t::TXBuffer = buffPtr;
	Slave_t::TXSize = size;

	if(Slave_t::TXSize < 0)
		Slave_t::TXSize = 0;
}

void Slave_t::setRXBuffer(uint8_t buffPtr[], int size)
{
	Slave_t::RXBuffer = buffPtr;
	Slave_t::RXSize = size;

	if(Slave_t::RXSize < 0)
		Slave_t::RXSize = 0;
}

bool Slave_t::newMessage()
{
	if(Slave_t::hasNewMessage)
	{
		Slave_t::hasNewMessage = false;
		return true;
	}
	else
		return false;
}

void Slave_t::sendData()
{
	Wire.write(Slave_t::TXBuffer, Slave_t::TXSize);

    Slave_t::TXCnt += 1;
}

void Slave_t::receiveData(int n)
{
	//copy message from arduino to library buffer
	for(int i = 0; Wire.available() && i<Slave_t::RXSize; i++)
  	Slave_t::RXBuffer[i] = Wire.read();
	
	Slave_t::hasNewMessage = true;
  Slave_t::RXCnt += 1;
}

uint16_t Slave_t::getRXCnt() { return RXCnt; }
uint16_t Slave_t::getTXCnt() { return TXCnt; }

// Instatiate Slave object using the default (empty) constructor
Slave_t Slave = Slave_t();
