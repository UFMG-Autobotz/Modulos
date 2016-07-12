// Implementação da biblioteca WireBotz.h

#include <Wire.h>
#include <WireBotz.h>

// Declare
uint8_t* Slave::RXBuffer;
uint8_t* Slave::TXBuffer;
int      Slave::RXSize;   //0 if no buffer registered
int      Slave::TXSize;   //0 if no buffer registered

bool     Slave::stayQuiet;    //diz se pode ou nao responder o mestre
bool     Slave::hasNewMessage;  //diz se recebeu uma menssagem nova


////////////////////////////////////////////////////////////////
//////////////////        Master Class        //////////////////
////////////////////////////////////////////////////////////////

void Master::begin()
{
	Wire.begin();  //just this
}

int Master::write(byte addr, uint8_t msg[], int size)
{
  int n, error;

  Wire.beginTransmission(addr);

  n = Wire.write(msg, size);  //Write message
  if (n != size)
    return (-21);

  error = Wire.endTransmission(true); // Verifica o estado do barramento I2C.
  if (error != 0) //Se tiver algum erro, a função retorna ele.
    return (error);

  return (0);         // Retorna sem erro.

//return codes:
//	-21 = could not write 'size' bytes
//	1   = data too long to fit in transmit buffer
//	2   = received NACK on transmit of address
//	3   = received NACK on transmit of data
//	4   = other error
//  0   = ok
}

int Master::read(byte addr, uint8_t msg[], int maxSize)
{
	Wire.requestFrom((int)addr, maxSize);
	int receivedBytes = 0;

	while(Wire.available()) //read received message byte by byte
	{
		if(receivedBytes > maxSize)
			return(1);

		msg[receivedBytes] = Wire.read();
		receivedBytes ++;
	}
	
	return 0;

//return codes:
//  1 = message is longer than maxSize, buffer contains first maxSize bytes
//	0 = received message, didn't reach maxSize
}

//////////////////////////////////////////////////////////////
/////////////////        Slave Class        //////////////////
//////////////////////////////////////////////////////////////

void Slave::begin(byte address)
{
	Wire.begin(address);   //join the bus as slave

	Wire.onRequest(Slave::sendData);    //callback to answer master
	Wire.onReceive(Slave::receiveData);     //callBack to receive data from master

	//set private variables to default values
	Slave::stayQuiet = false;
	Slave::hasNewMessage = false;

	Slave::TXSize = 0;
	Slave::RXSize = 0;
}

void Slave::setTXBuffer(uint8_t buffPtr[], int size)
{
	Slave::TXBuffer = buffPtr;
	Slave::TXSize = size;

	if(Slave::TXSize < 0)
		Slave::TXSize = 0;
}

void Slave::setRXBuffer(uint8_t buffPtr[], int size)
{
	Slave::RXBuffer = buffPtr;
	Slave::RXSize = size;

	if(Slave::RXSize < 0)
		Slave::RXSize = 0;
}

bool Slave::newMessage()
{
	if(Slave::hasNewMessage)
	{
		Slave::hasNewMessage = false;
		return true;
	}
	else
		return false;
}

void Slave::quiet(bool q)
{
	Slave::stayQuiet = q;
}

void Slave::sendData()
{
	Wire.write(Slave::TXBuffer, Slave::TXSize);
}

void Slave::receiveData(int n)
{
  	//copy message from arduino to library buffer
  	for(int i = 0; Wire.available() && i<Slave::RXSize; i++)
    	Slave::RXBuffer[i] = Wire.read();
	
	Slave::hasNewMessage = true;
}
