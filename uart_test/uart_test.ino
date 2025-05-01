#include <HardwareSerial.h>

HardwareSerial SerialPort(0); // use UART0


void setup()  
{
  SerialPort.begin(15200, SERIAL_8N1, 1, 3); 
} 
void loop()  
{ 
  SerialPort.print(1);
  delay(5000);
  SerialPort.print(0);
  delay(5000);
}
