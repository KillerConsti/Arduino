/*

Demonstrates simple RX operation with an ESP32.
Any of the Basic_TX examples can be used as a transmitter.

ESP's require the use of '__attribute__((packed))' on the RadioPacket data structure
to ensure the bytes within the structure are aligned properly in memory.

The ESP32 SPI library supports configurable SPI pins and NRFLite's mechanism to support this is shown.

Radio    ESP32 module
CE    -> 4
CSN   -> 5
MOSI  -> 23
MISO  -> 19
SCK   -> 18
IRQ   -> No connection
VCC   -> No more than 3.6 volts
GND   -> GND

*/
#include "SPI.h"
#include "NRFLite.h"

const static uint8_t RADIO_ID = 0;
const static uint8_t DESTINATION_RADIO_ID = 1;
const static uint8_t PIN_RADIO_CE = 2;
const static uint8_t PIN_RADIO_CSN = 15;
const static uint8_t PIN_RADIO_MOSI = 23;
const static uint8_t PIN_RADIO_MISO = 19;
const static uint8_t PIN_RADIO_SCK = 18;

struct  __attribute__((packed)) Commandpackage // Any packet up to 32 bytes can be sent.
{
    uint8_t CommandId = 0;
    uint8_t CommandArg1 = 0;
    uint8_t CommandArg2 = 0;
};
struct  __attribute__((packed)) RadioScorePacket // Any packet up to 32 bytes can be sent.
{
    uint8_t FromRadioId;
    uint8_t T1_Score;
    uint8_t T2_Score;
    uint8_t T1_Sets;
    uint8_t T2_Sets;
    uint32_t FailedTxCount;
};

typedef struct
  {
      uint8_t Team1;
      uint8_t Team2;
      uint8_t Satz;
      uint8_t Winner; //0 is no winner 1 -> T1 ; 2 ->t2;
      bool Set = false;
  }  VolleyBallHistory;
VolleyBallHistory MyVolleyHistory[5];

struct  __attribute__((packed)) RadioHistoryPacket // Any packet up to 32 bytes can be sent.
{
  uint8_t FromRadioId;
  VolleyBallHistory myVolleyBallHistory[5];
  uint32_t FailedTxCount;
};
NRFLite _radio;
RadioScorePacket _radioData;
RadioHistoryPacket _radioHistoryData;
Commandpackage _radioCmdData;


void setup()
{
    Serial.begin(9600);
    
    // Configure SPI pins.
    SPI.begin(PIN_RADIO_SCK, PIN_RADIO_MISO, PIN_RADIO_MOSI, PIN_RADIO_CSN);
    
    // Indicate to NRFLite that it should not call SPI.begin() during initialization since it has already been done.
    uint8_t callSpiBegin = 0;
    
    if (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE2MBPS, 100, callSpiBegin))
    {
        Serial.println("Cannot communicate with radio");
        while (1); // Wait here forever.
    }
}

void loop()
{
    if(Serial.available()) 
    {
  // TODO
    Serial.read();
    _radio.send(DESTINATION_RADIO_ID, &_radioCmdData, sizeof(_radioCmdData));
    Serial.println("serial av");
    }
    uint8_t packetSize = _radio.hasData();
    if (packetSize == sizeof(RadioScorePacket))
    {
        _radio.readData(&_radioData);

        String msg = "Radio ";
        msg += _radioData.FromRadioId;
        msg += ", ";
        msg += _radioData.T1_Score;
        msg += " : ";
        msg += _radioData.T2_Score;
        msg += " sets: ";
        msg += _radioData.T1_Sets;
        msg += " : ";
        msg += _radioData.T2_Sets;
        Serial.println(msg);
    }
    else if (packetSize == sizeof(RadioHistoryPacket))
    {
        uint8_t t1_won = 0;
        uint8_t t2_won = 0;
        _radio.readData(&_radioHistoryData);
        Serial.println("Verlauf:");
        if(_radioHistoryData.myVolleyBallHistory[0].Set == false)
        {
          Serial.print("Keine Sätze gespielt");
          return;
        }
        for(size_t t = 0; t < 5;t++)
        {
          if(!_radioHistoryData.myVolleyBallHistory[t].Set)
            continue;
            Serial.print("(");
            Serial.print(_radioHistoryData.myVolleyBallHistory[t].Satz);
            Serial.print(") ");
            Serial.print(_radioHistoryData.myVolleyBallHistory[t].Team1);
            Serial.print(" : ");
            Serial.println(_radioHistoryData.myVolleyBallHistory[t].Team2);
            if(_radioHistoryData.myVolleyBallHistory[t].Winner <= 1)
            {
              t1_won++;
            }
            if (_radioHistoryData.myVolleyBallHistory[t].Winner == 2 || _radioHistoryData.myVolleyBallHistory[t].Winner== 0)
            {
              t2_won++;
            }

        }
        Serial.print("Satzverhaeltnis: ");
        Serial.print(t1_won);
        Serial.print(" : ");
        Serial.println(t2_won);


    }
            else if(packetSize != 0)
        {
            // We have a packet with an unexpected size, either an invalid packet or packet
            // that was sent from a radio that we cannot handle, so remove it from the radio.

            Serial.print("Received packet with length '");
            Serial.print(packetSize);
            Serial.println("', it will be discarded");

            _radio.discardData(packetSize);
        }
}

int ConvertIntToStringWithDoubleDigit(char *array,int num)
{
  array[0] = '0';
  array[1] = '0';
  
  itoa(num,array,10);
  if(num <10)
  {
    
    array[1] = array[0];
    array[0] = '0';
  }
  return 1;
}
