#include <HardwareSerial.h>

HardwareSerial SerialPort(0); // use UART0


char number  = ' ';
int LED = 4;

void setup()
{
  SerialPort.begin(15200, SERIAL_8N1, 1, 3);
  pinMode(LED, OUTPUT);
}
void loop()
{
  if (SerialPort.available())
  {
    char number = SerialPort.read();
    if (number == '0') {
      digitalWrite(LED, LOW);
    }
    if (number == '1') {
      digitalWrite(LED, HIGH);
    }
  }
}
