/*!
	@file     TM1638plus_HELLOWORLD_Model2.ino
	@author   Gavin Lyons
	@brief 
		A Basic test file for model 2, TM1638 module(16 KEY 16 pushbuttons).
		Carries out series most basic test , "hello world" helo wrld
*/

#include <TM1638plus_Model2.h>
#include <stdlib.h>

#include <EEPROM.h>
// GPIO I/O pins on the Arduino connected to strobe, clock, data, pick on any I/O pin you want.
#define  STROBE_TM 5  // strobe = GPIO connected to strobe line of module
#define  CLOCK_TM 22  // clock = GPIO connected to clock line of module
#define  DIO_TM 21 // data = GPIO connected to data line of module
bool swap_nibbles = false; //Default is false if left out, see issues section in readme at URL
bool high_freq = false; //default false, If using a high freq CPU > ~100 MHZ set to true. 
int counter = -1;

int points_left;
int points_right;
// Constructor object Init the module

uint64_t time_pressed_swap = millis();
uint64_t time_pressed_left_up= millis();
uint64_t time_pressed_left_right_up= millis();
bool left_up_pressed = false;
bool right_up_pressed = false;
bool Set_Was_Finished = false;
bool mShowMirrored = true;
bool NoData = true;
bool connected = false;
TM1638plus_Model2 tm(STROBE_TM, CLOCK_TM , DIO_TM, swap_nibbles, high_freq);

// RadioData

//1 SPI AND PINS
#include "SPI.h"


//tft in user_setup.h

//2 ID and PINS
const static uint8_t RADIO_ID = 0;
const static uint8_t PIN_RADIO_CE = 2;
const static uint8_t PIN_RADIO_CSN = 15;
const static uint8_t PIN_RADIO_MOSI = 23;
const static uint8_t PIN_RADIO_MISO = 19;
const static uint8_t PIN_RADIO_SCK = 18;

//Remotes are 4-6 which targeting 1-3 
//0 is general remote control (with ble server)


uint8_t mRadioToSendTo = 1; //targetradio -> change in EEPROM
byte mOurRadioID = 4; //ourself
uint64_t mTimeLastRadioConnectMessageWasSent= millis();

//2 structs

//1. What we send
struct __attribute__((packed)) Radio_Remote_Commandpackage  // Any packet up to 32 bytes can be sent.
{
  uint8_t FromRadioId;
  uint8_t CommandId = 0;
  uint8_t CommandArg1 = 0;
};

//Commands
enum Commands
{
  Connect,
  PlusT1,
  MinusT1,
  PlusT2,
  MinusT2,
  Swap,
  Reset,
  SetFinished
};

struct __attribute__((packed)) RadioScorePacket  // Any packet up to 32 bytes can be sent.
{
  uint8_t FromRadioId;
  uint8_t T1_Score;
  uint8_t T2_Score;
  uint8_t T1_Sets;
  uint8_t T2_Sets;
  uint32_t FailedTxCount;
};




#include "NRFLite.h"
NRFLite _radio;
RadioScorePacket _radioData;
Radio_Remote_Commandpackage _radioCmdData;
int mCurrentRadioID = 0;
struct CommandMsg {
  bool MessageRecieved;
  int CommandId = 0;
  int Arg1 = 0;
};


//OTA
/*#include <WiFi.h>
#include <ESPmDNS.h>
#include <NetworkUdp.h>
#include <ArduinoOTA.h>*/

const char *ssid = "FRITZ!Box 7590 JV";
const char *password = "03030231803958020870";
int network_tries =0;

//Start here
//notice that we need to init eeprom and also use commit

//NRF <-> Radio End
void BuildString(int PointsLeft,int PointsRight,int SetsLeft,int SetsRight,char *text)
{
  char Buffer[2];
  itoa(PointsLeft, Buffer, 10);
  if(PointsLeft < 10)
  {
    text[0] = '0';
    text[1] = Buffer[0]; 
  }
  else
  {
    text[0] = Buffer[0];
    text[1] = Buffer[1];  
  }
  text[2] = ' ';
  itoa(SetsLeft, Buffer, 10);
  text[3] = Buffer[0];

  //Right Side
  itoa(PointsRight, Buffer, 10);
  if(PointsRight < 10)
  {
    text[6] ='0';
    text[7] = Buffer[0]; 
  }
  else
  {
    text[6] = Buffer[0];
    text[7] = Buffer[1];  
  }
  text[5] = ' ';
  itoa(SetsRight, Buffer, 10);
  text[4] = Buffer[0];

}

void LoadRadioIdsFromEEPROM()
{
   mOurRadioID = EEPROM.read(0);
     Serial.print("Read EEPROM");
  Serial.println((int)mOurRadioID);
   if(mOurRadioID < 4 || mOurRadioID >6)
   {
    mOurRadioID = 4;
   }
  //need to implent
  mRadioToSendTo = mOurRadioID-3; //targetradio -> change in EEPROM
  Serial.print("We use radio id ");
  Serial.print((int)mOurRadioID);
  Serial.print(" and send to");
  Serial.println((int)mRadioToSendTo);
}

void GoIntONextChannel()
{
  Serial.println("go into next channel");
  mOurRadioID++;
  if(mOurRadioID > 6)
  {
    mOurRadioID = 4;
  }
  mRadioToSendTo = mOurRadioID -3;
  EEPROM.write(0, mOurRadioID);
  EEPROM.commit();
    Serial.print("We changed channel radio id ");
  Serial.print((int)mOurRadioID);
  /*Serial.print(" EEPROM ");
  byte result = EEPROM.read(0);
   Serial.print((int)result);*/
  Serial.print(" and send to");
  Serial.println((int)mRadioToSendTo);
  if(mOurRadioID == 4)
  tm.DisplayStr("CH 1");
  else if (mOurRadioID == 5)
  tm.DisplayStr("CH 2");
  else if (mOurRadioID == 6)
  tm.DisplayStr("CH 3");
  delay(2000);
  NeedConnection();
  delay(500);
    T1Point(1);
    T1Point(-1);
}


void setup() {
  Serial.begin(9600);
  Serial.println("we start here");
    EEPROM.begin(512);
 tm.displayBegin();
 NeedConnection();

 //init radio
   if (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN)) {
    Serial.println("Cannot communicate with radio");
    //while (1)
      ;  // Wait here forever.
  }
  //TODO Implent EEPROM func
  LoadRadioIdsFromEEPROM();

  //ota
  /*WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    network_tries++;
    if(network_tries > 2)
    {
    Serial.println("No network connection");
    break;
    }
    else
    delay(200);
  }*/
  /*if(network_tries < 3)
  connected = true;

 ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH) {
        type = "sketch";
      } else {  // U_SPIFFS
        type = "filesystem";
      }

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) {
        Serial.println("Auth Failed");
      } else if (error == OTA_BEGIN_ERROR) {
        Serial.println("Begin Failed");
      } else if (error == OTA_CONNECT_ERROR) {
        Serial.println("Connect Failed");
      } else if (error == OTA_RECEIVE_ERROR) {
        Serial.println("Receive Failed");
      } else if (error == OTA_END_ERROR) {
        Serial.println("End Failed");
      }
    });

  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());*/
}

void loop() {
  //if(connected)
//ArduinoOTA.handle();

  ListenToRadioMessages();
    if(NoData && mTimeLastRadioConnectMessageWasSent +1000 < millis()) //do wait until we got a msg
  {
      //this is a lil hack
    T1Point(1);
    T1Point(-1);
    mTimeLastRadioConnectMessageWasSent = millis();
    return;
  }
  if(counter == tm.ReadKey16Two())
  return;
  if(tm.ReadKey16Two() == 9 && !Set_Was_Finished)
  {
    Set_Was_Finished = true;
    RegisterSet();
    Serial.println("finished set");
    left_up_pressed = true;
    right_up_pressed = true;
  }
  if(tm.ReadKey16Two() == 512)
  {
    //store time when we press it

    //short press <-> change sites

    //long press mirror display

    time_pressed_swap = millis();

  }
  char buffer[7];         //the ASCII of the integer will be stored in this char array
  itoa(tm.ReadKey16Two(),buffer,10); //(integer, yourBuffer, base)
  Serial.printf ("decimal: %s\n",buffer);
  if(tm.ReadKey16Two() == 0) //execute at button up
  {
    Set_Was_Finished = false;
    Serial.println("we are here");
    if(counter  == 1 && left_up_pressed)
    {
      left_up_pressed = false;
      counter = 0;
      return;
    }
    else if( counter == 8 && right_up_pressed)
    {
      right_up_pressed = false;
      counter = 0;
      return;
    }
    onPressButton();
  }
  else if (tm.ReadKey16Two() == 1+8+4096+32768)
  {
    GoIntONextChannel();
  }
    counter = tm.ReadKey16Two();
	delay(20);
}
void onPressButton()
{
  switch (counter)
  {
    case 1:
    if(mShowMirrored)
    {
      T2Point(1);
    }
    else
    {
      T1Point(1);
    }
    return;
    case 4096:
        if(mShowMirrored)
    {
      T2Point(-1);
    }
    else
    {
      T1Point(-1);
    }
    return;
    case 8:
    Serial.println("oben");
            if(mShowMirrored)
    {
      Serial.println("mirrored");
      T1Point(1);
    }
    else
    {
      T2Point(1);
    }
    return;
    case 32768:
    Serial.println("unten");
                if(mShowMirrored)
    {
      Serial.println("mirrored");
      T1Point(-1);
    }
    else
    {
      Serial.println("not mirrored");
      T2Point(-1);
    }
    return;
    case 512: //swap
    {
      if(time_pressed_swap +1000 < millis())
      {
          Mirror();
        //we do mirror
         Serial.println("mirror");
      }
      else
      {
          SawpData();
          Serial.println("swap");
      }
      char buffer[7];         //the ASCII of the integer will be stored in this char array
        itoa(millis()-time_pressed_swap,buffer,10); //(integer, yourBuffer, base)
        Serial.printf ("decimal: %s\n",buffer);
      return;
    }
    case 1024:
    {
      ResetButton();
      Serial.println("reset");
      return;
    }
    return;
  }
}

void SawpData()
{
   _radioCmdData.CommandId = Commands::Swap;
   _radioCmdData.FromRadioId = mOurRadioID;
   _radioCmdData.CommandArg1 = 0;
  if(_radio.send(mRadioToSendTo, &_radioCmdData, sizeof(_radioCmdData)))
  {
      Serial.println("SawpData...Success");
    } else {
      Serial.println("SawpData...Failed");
    }
}
//this is intern
void Mirror()
{
  mShowMirrored = !mShowMirrored;
  UpdateDisplay();
}
void ResetButton()
{
    _radioCmdData.CommandId = Commands::Reset;
   _radioCmdData.FromRadioId = mOurRadioID;
   _radioCmdData.CommandArg1 = 0;
  if(_radio.send(mRadioToSendTo, &_radioCmdData, sizeof(_radioCmdData)))
  {
      Serial.println("ResetButton...Success");
    } else {
      Serial.println("ResetButton...Failed");
    }
}
void T1Point(int point)
{
    if(point > 0)
   _radioCmdData.CommandId = Commands::PlusT1;
   else
   _radioCmdData.CommandId = Commands::MinusT1;
   _radioCmdData.FromRadioId = mOurRadioID;
   _radioCmdData.CommandArg1 = 0;
    if(_radio.send(mRadioToSendTo, &_radioCmdData, sizeof(_radioCmdData)))
  {
      Serial.println("T1Point...Success");
    } else {
      //Serial.println("T1Point...Failed");
    }
}

void T2Point(int point)
{
    if(point > 0)
   _radioCmdData.CommandId = Commands::PlusT2;
   else
   _radioCmdData.CommandId = Commands::MinusT2;
   _radioCmdData.FromRadioId = mOurRadioID;
   _radioCmdData.CommandArg1 = 0;
    if(_radio.send(mRadioToSendTo, &_radioCmdData, sizeof(_radioCmdData)))
  {
      Serial.println("T2Point...Success");
    } else {
      Serial.println("T2Point...Failed");
    }
}

void NeedConnection()
{
  tm.DisplayStr("NO  DATA");
}
void UpdateDisplay()
{
  char MyText[8];
  if(mShowMirrored)
  {
    BuildString(_radioData.T1_Score,_radioData.T2_Score,_radioData.T1_Sets,_radioData.T2_Sets,MyText);
  }
  else
  {
  BuildString(_radioData.T2_Score,_radioData.T1_Score,_radioData.T2_Sets,_radioData.T1_Sets,MyText);
  }
  tm.DisplayStr(MyText,0);
  Serial.print(MyText);
}

bool ListenToRadioMessages()
{
  //most important <-> apply data
  uint8_t packetSize = _radio.hasData();
  if (packetSize == sizeof(RadioScorePacket)) {
    Serial.println("Radio Msg recieved");
      _radio.readData(&_radioData);
    uint8_t sender = _radioData.FromRadioId;
    if(_radioData.FromRadioId != mRadioToSendTo)
    return false;
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
    UpdateDisplay();
    NoData = false;
    return true;
  }
  return false;
}



void RegisterSet()
{
   _radioCmdData.CommandId = Commands::SetFinished;
   _radioCmdData.FromRadioId = mOurRadioID;
   _radioCmdData.CommandArg1 = 0;
    if(_radio.send(mRadioToSendTo, &_radioCmdData, sizeof(_radioCmdData)))
  {
      Serial.println("RegisterSet...Success");
    } else {
      Serial.println("RegisterSet...Failed");
    }
}
