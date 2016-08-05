// Implementação da classe Master na biblioteca WireBotz

#include <Wire.h>
#include <WireBotzMaster.h>

// Public Methods
void Master_t::begin()
{
	Wire.begin();  //just this
}

int Master_t::write(byte addr, uint8_t msg[], int size)
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

int Master_t::read(byte addr, uint8_t msg[], int maxSize)
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

// Instatiate the Master object using the default (empty) constructor
Master_t Master = Master_t();