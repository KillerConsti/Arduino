/*
  This sketch is the same as the Font_Demo_3 example, except the fonts in this
  example are in a FLASH (program memory) array. This means that processors
  such as the STM32 series that are not supported by a SPIFFS library can use
  smooth (anti-aliased) fonts.
*/

/*
  There are four different methods of plotting anti-aliased fonts to the screen.

  This sketch uses method 3, the font characters are first plotted in a Sprite, then the
  Sprite is pushed to the screen. This method is very flexible and the Sprite can be
  created, deleted, resized as needed. To render anti-aliased fonts well the Sprite
  needs to be 16 bit.  The fonts will render in 1 bit per pixel sprites but there
  will then be no anti-aliasing. Using 1 bit per pixel Sprites is however useful
  to use the extended Unicode range in fonts on mono displays like ePaper.

  A single Sprite can be re-used for plotting different values and graphics to
  different positions on the screen. This makes this method a very powerful display tool,
  for example round buttons can be created, making use of transparent colour plotting.
  
*/

// A processing sketch to create new fonts can be found in the Tools folder of TFT_eSPI
// https://github.com/Bodmer/TFT_eSPI/tree/master/Tools/Create_Smooth_Font/Create_font

// This sketch uses font files created from the Noto family of fonts:
// https://www.google.com/get/noto/
#include <Globals.h>
#include <FoxAndVolleyball.h>
#include <BLEInterface.h>
#include <display.h>
MyDisplay* mDisplay;

BLEInterface mBLEInterface;
#include <pgmspace.h>


Drehschalter mDrehschalter;


bool m_init = false;

//CMDS
/*
CMD 0 = Request History and Score Data from Uno
CMD 1,T1_P,T2_P,S1_P,S2_P = Set New Score and Sets(Team1, Team2)
CMD 2,(0- or 1) -> Reset ->Soft (0) or Hard (1)
CMD3- Update History
  -T1 Points
  -T2 Points
  -Team Won (0 tie; 1 = T1; 2 = T2) -> if = 255 set is not counted
  -Set Number (duno if that works that smart)
CMD 3,T1,T2,TeamWon,Is Set
CMD4 -PartyMode

*/

MatchDataLong mMatchDataLong;
 MatchDataShort mMatchDataShort;

/* Bluetooth */
/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-web-bluetooth/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

//char MatchNameData_Field2[12][30];
//char MatchNameData_Field3[12][30];






#include "NRFLite.h"
NRFLite _radio;
RadioScorePacket _radioData;
RadioHistoryPacket _radioHistoryData;
Commandpackage _radioCmdData;




/*
The problem is Serial.print when you pass a char * argument, it expects the array to have a null byte to denote the end of the string. Even though you passed a char array, what is passed is a char pointer (i.e. the array bounds are not passed). If you bump up the size of the arrays, and add an explicit null byte, it should print fine. You can use either '\0' or just 0 to represent a null byte, i.e.:
*/
void SetCurrentMatchField1(int mMatchNum)
{
  //Serial.println(mMatchNum);
  if(mMatchNum < 0 || mMatchNum > (mValidTeamNamesField1 /2))
  {
    Serial.println("SetCurrentMatchField1 invalid Matchnum");
    return;
  }
  //copy data
  for(size_t t=0; t <TeamnameLength; t++)
  {
    Serial.print(MatchNameData_Field1[t][mMatchNum*2]);
    Serial.print(" ");
    Serial.println(static_cast<uint8_t>(MatchNameData_Field1[t][mMatchNum*2]));
    mMatchDataLong.TeamName1[t] = MatchNameData_Field1[t][mMatchNum*2];
    mMatchDataLong.TeamName2[t] = MatchNameData_Field1[t][mMatchNum*2+1];
  }
  //mMatchDataLong.TeamName1[11] = '\0';
  //mMatchDataLong.TeamName2[11] = '\0';
  currentMatch_Field1 = mMatchNum;
  //send data

  mMatchDataLong.MatchNum =mMatchNum;
  mMatchDataLong.type = 1;
  Serial.println(mMatchDataLong.MatchNum);
  Serial.println(mMatchDataLong.type);
  //Serial.println(mMatchDataLong.TeamName1);
  //Serial.println(mMatchDataLong.TeamName2);
    if(_radio.send(8, &mMatchDataLong, sizeof(mMatchDataLong),NRFLite::NO_ACK))
  {
    Serial.println("...success");
  }
  else
  {
    Serial.println("...fail");
  }
  Serial.println("send stuff");
}

void SetNextMatchField1(int mMatchNumCurrentMatch)
{
  //Serial.println(mMatchNum);
  int NextMatchNum = mMatchNumCurrentMatch+1;
  if(NextMatchNum < 0 || NextMatchNum > (mValidTeamNamesField1 /2))
  {
    Serial.println("SetNextMatchField1 invalid Matchnum");
    return;
  }
  //copy data
  for(size_t t=0; t <TeamnameLength; t++)
  {
    mMatchDataLong.TeamName1[t] = MatchNameData_Field1[t][NextMatchNum*2];
    mMatchDataLong.TeamName2[t] = MatchNameData_Field1[t][NextMatchNum*2+1];
  }
  //mMatchDataLong.TeamName1[11] = '\0';
  //mMatchDataLong.TeamName2[11] = '\0';
  currentMatch_Field1 = mMatchNumCurrentMatch;
  //send data

  mMatchDataLong.MatchNum =NextMatchNum;
  mMatchDataLong.type = 4;
  //Serial.println(mMatchDataLong.TeamName1);
  //Serial.println(mMatchDataLong.TeamName2);
    if(_radio.send(8, &mMatchDataLong, sizeof(mMatchDataLong),NRFLite::NO_ACK))
  {
    Serial.println("...success");
  }
  else
  {
    Serial.println("...fail");
  }
  Serial.println("send stuff");
}

void SetCurrentMatchField2(int mMatchNum)
{
  //Serial.println(mMatchNum);
  if(mMatchNum < 0 || mMatchNum > (mValidTeamNamesField2 /2))
  {
    Serial.println("SetCurrentMatchField2 invalid Matchnum");
    return;
  }
  //copy data
  for(size_t t=0; t <TeamnameLength; t++)
  {
    mMatchDataLong.TeamName1[t] = MatchNameData_Field2[t][mMatchNum*2];
    mMatchDataLong.TeamName2[t] = MatchNameData_Field2[t][mMatchNum*2+1];
  }
  //mMatchDataLong.TeamName1[11] = '\0';
  //mMatchDataLong.TeamName2[11] = '\0';
  currentMatch_Field2 = mMatchNum;
  //send data

  mMatchDataLong.MatchNum =mMatchNum;
  mMatchDataLong.type = 2;
  Serial.println(mMatchDataLong.MatchNum);
  Serial.println(mMatchDataLong.type);
  //Serial.println(mMatchDataLong.TeamName1);
  //Serial.println(mMatchDataLong.TeamName2);
    if(_radio.send(8, &mMatchDataLong, sizeof(mMatchDataLong),NRFLite::NO_ACK))
  {
    Serial.println("...success");
  }
  else
  {
    Serial.println("...fail");
  }
  Serial.println("send stuff");
}

void SetNextMatchField2(int mMatchNumCurrentMatch)
{
  //Serial.println(mMatchNum);
  int NextMatchNum = mMatchNumCurrentMatch+1;
  if(NextMatchNum < 0 || NextMatchNum > (mValidTeamNamesField2 /2))
  {
    Serial.println("SetNextMatchField2 invalid Matchnum");
    return;
  }
  //copy data
  for(size_t t=0; t <TeamnameLength; t++)
  {
    mMatchDataLong.TeamName1[t] = MatchNameData_Field2[t][NextMatchNum*2];
    mMatchDataLong.TeamName2[t] = MatchNameData_Field2[t][NextMatchNum*2+1];
  }
  //mMatchDataLong.TeamName1[11] = '\0';
  //mMatchDataLong.TeamName2[11] = '\0';
  currentMatch_Field2 = mMatchNumCurrentMatch;
  //send data

  mMatchDataLong.MatchNum =NextMatchNum;
  mMatchDataLong.type = 5;
  //Serial.println(mMatchDataLong.TeamName1);
  //Serial.println(mMatchDataLong.TeamName2);
    if(_radio.send(8, &mMatchDataLong, sizeof(mMatchDataLong),NRFLite::NO_ACK))
  {
    Serial.println("...success");
  }
  else
  {
    Serial.println("...fail");
  }
  Serial.println("send stuff");
}


//Need to connect matchdata with names


/*int currentMatch_Field1= -1;
int mValidTeamNamesField1 = 0;
*/

void UpdateScreenColor() {
  mDisplay->UpdateScreenColor();
}

void SendMatchDataLongToScoreBoard(int MatchNum)
{

}

void SendMatchDataShortToScoreBoard(int MatchNum)
{
  mMatchDataShort.FromRadioId = 0;
  mMatchDataShort.T1_Score = mRadioScores[MatchNum].mT1;
  mMatchDataShort.T2_Score = mRadioScores[MatchNum].mT2;
  mMatchDataShort.T1_Sets = mRadioScores[MatchNum].mS1;
  mMatchDataShort.T2_Sets = mRadioScores[MatchNum].mS2;
  mMatchDataShort.MatchNum =MatchNum+1;
  Serial.print("send data short to LED board ");
  
  if(_radio.send(8, &mMatchDataShort, sizeof(mMatchDataShort),NRFLite::NO_ACK))
  {
    Serial.println("...success");
  }
  else
  {
    Serial.println("...fail");
  }
}
//Communication with ScoreBoard


void setup(void) {
  Serial.begin(9600 );
  //attachInterrupt(digitalPinToInterrupt(PIN_RADIO_IRQ), radioInterrupt, FALLING);
  pinMode(mDrehschalter._CLK,INPUT);
  pinMode(mDrehschalter._DT,INPUT);
  pinMode(mDrehschalter._SW,INPUT);
    mDisplay = new MyDisplay();
  mRadioScores[0].BackgroundColor = 3697;
  mRadioScores[1].BackgroundColor = 61521;  //tft.color565(240,11,140);
  mRadioScores[2].BackgroundColor = 2142;   //tft.color565(11,11,240);
           mDisplay->initDevice();
        digitalWrite(22,HIGH);
        delay(1000);
        
        /*
        #define HSPI_SCK 14 
#define HSPI_MISO 12
#define HSPI_MOSI 13 
#define HSPI_CS 15 
*/
        if (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN)) {
    Serial.println("Cannot communicate with radio");
    //while (1)
      ;  // Wait here forever.
  }
  
 
 
  delay(1000);


  attachInterrupt(digitalPinToInterrupt(mDrehschalter._CLK), DrehschalterDreh1, CHANGE);

  mBLEInterface = *new BLEInterface();
  mBLEInterface.initDevice();
  Serial.println("Waiting a client connection to notify...");
  _radioCmdData.CommandId = 0;
  if(_radio.send(1, &_radioCmdData, sizeof(_radioCmdData)))
  {
    Serial.println("radio msg 1 was sent succesfully");
  }
    if(_radio.send(2, &_radioCmdData, sizeof(_radioCmdData)))
  {
    Serial.println("radio msg 2 was sent succesfully");
  }
}

void loop() {
  if(NeedToUpdateScreenMode)
  {
      _radioCmdData.CommandId = 0;
      _radioCmdData.CommandArg1 =LEDScreenMode;
  if(_radio.send(8, &_radioCmdData, sizeof(_radioCmdData),NRFLite::NO_ACK))
    Serial.println("radio msg 1 was sent succesfully");
    NeedToUpdateScreenMode = false;
    return;
  }
  if(NeedToSendMachData_1_CurrentMatch)
  {
    NeedToSendMachData_1_CurrentMatch = false;
    SetCurrentMatchField1(currentMatch_Field1);
    return;
  }
  if(NeedToSendMachData_1_NextMatch)
  {
    NeedToSendMachData_1_NextMatch = false;
    SetNextMatchField1(currentMatch_Field1);
    return;
  }
    if(NeedToSendMachData_2_CurrentMatch)
  {
    NeedToSendMachData_2_CurrentMatch = false;
    SetCurrentMatchField2(currentMatch_Field2);
    return;
  }
  if(NeedToSendMachData_2_NextMatch)
  {
    NeedToSendMachData_2_NextMatch = false;
    SetNextMatchField2(currentMatch_Field2);
    return;
  }
  CheckInputState();
  if (mUpdateScreenOne) {
    UpdateScreenColor();
    mUpdateScreenOne = false;
    _radioCmdData.CommandId = 0;
    //update my score pls
    _radio.send(1, &_radioCmdData, sizeof(_radioCmdData));
    Serial.println("we sent cmd to radio");
    return;
  }
    if (mUpdateScreenTwo) {
    UpdateScreenColor();
    mUpdateScreenTwo = false;
    _radioCmdData.CommandId = 0;
    //update my score pls
    _radio.send(2, &_radioCmdData, sizeof(_radioCmdData));
    Serial.println("we sent cmd to radio");
    return;
  }
  if (NeedToNotifyTFT1) {
    Serial.println("need to notify tft");
    //in this case we only call this fct from BLE
    SetNewScore(mRadioScores[0].mT1, mRadioScores[0].mT2, mRadioScores[0].mS1, mRadioScores[0].mS2,0);
    NeedToNotifyTFT1 = false;
    
  }
  else if(NeedToNotifyTFT2) 
  {
    Serial.println("need to notify tft 2");
    //in this case we only call this fct from BLE
    SetNewScore(mRadioScores[1].mT1, mRadioScores[1].mT2, mRadioScores[1].mS1, mRadioScores[1].mS2,1);
    NeedToNotifyTFT2 = false;
  }
  if (NeedToNotifyRadio1) {
    Serial.println("Inform Radio 1 now");
    _radioCmdData.CommandId = 1;
    _radioCmdData.CommandArg1 = mRadioScores[0].mT1;
    _radioCmdData.CommandArg2 = mRadioScores[0].mT2;
    _radioCmdData.CommandArg3 = mRadioScores[0].mS1;
    _radioCmdData.CommandArg4 = mRadioScores[0].mS2;
    _radio.send(1, &_radioCmdData, sizeof(_radioCmdData));
    NeedToNotifyRadio1 = false;

  }
  else   if (NeedToNotifyRadio2) {
    Serial.println("Inform Radio 2 now");
    _radioCmdData.CommandId = 1;
    _radioCmdData.CommandArg1 = mRadioScores[1].mT1;
    _radioCmdData.CommandArg2 = mRadioScores[1].mT2;
    _radioCmdData.CommandArg3 = mRadioScores[1].mS1;
    _radioCmdData.CommandArg4 = mRadioScores[1].mS2;
    _radio.send(2, &_radioCmdData, sizeof(_radioCmdData));
    NeedToNotifyRadio2 = false;

  }  
  else if (CommandMode_field1.MessageRecieved) { //This is for Sending Softreset/Hardreset/Partymode
    _radioCmdData.CommandId = CommandMode_field1.CommandId;
    _radioCmdData.CommandArg1 = CommandMode_field1.Arg1;
    CommandMode_field1.MessageRecieved = false;
    _radio.send(1, &_radioCmdData, sizeof(_radioCmdData));
    Serial.println("Send a Cmd");
  }
  uint8_t packetSize = _radio.hasData();
  if(packetSize > 0)
  {
    Serial.print("we recieved data ");
    Serial.println(packetSize);
  }
  if (packetSize == sizeof(RadioScorePacket)) {
    _radio.readData(&_radioData);
    uint8_t sender = _radioData.FromRadioId - 1;
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
    mRadioScores[sender].mT1 = _radioData.T1_Score;
    mRadioScores[sender].mT2 = _radioData.T2_Score;
    mRadioScores[sender].mS1 = _radioData.T1_Sets;
    mRadioScores[sender].mS2 = _radioData.T2_Sets;
      SetNewScore(_radioData.T1_Score, _radioData.T2_Score, _radioData.T1_Sets, _radioData.T2_Sets,sender);
    if(sender ==1)
    {
        NeedToNotifyLEDShort1 = true; //we may specify which matchdata
        NeedToNotifyBLE_BothMatches = true;
    }
      else
      {
        NeedToNotifyLEDShort2 = true;
        NeedToNotifyBLE_BothMatches = true;
      }
  } else if (packetSize == sizeof(RadioHistoryPacket)) {

    uint8_t t1_won = 0;
    uint8_t t2_won = 0;
    _radio.readData(&_radioHistoryData);
    uint8_t sender = _radioData.FromRadioId - 1;
    Serial.println("Verlauf:");
    if (_radioHistoryData.myVolleyBallHistory[0].Set == false) {
      Serial.print("Keine Sätze gespielt");
      return;
    }
    for (size_t t = 0; t < 5; t++) {
      if (!_radioHistoryData.myVolleyBallHistory[t].Set)
        continue;
      Serial.print("(");
      Serial.print(_radioHistoryData.myVolleyBallHistory[t].Satz);
      Serial.print(") ");
      Serial.print(_radioHistoryData.myVolleyBallHistory[t].Team1);
      Serial.print(" : ");
      Serial.println(_radioHistoryData.myVolleyBallHistory[t].Team2);
      if (_radioHistoryData.myVolleyBallHistory[t].Winner <= 1) {
        t1_won++;
      }
      if (_radioHistoryData.myVolleyBallHistory[t].Winner == 2 || _radioHistoryData.myVolleyBallHistory[t].Winner == 0) {
        t2_won++;
      }
    }
    for (uint8_t t = 0; t < 5; t++) {
      mRadioScores[sender].MyVolleyHistory[t] = _radioHistoryData.myVolleyBallHistory[t];
    }
    //if (sender == mCurrentRadioID) 
    {
      UpdateHistory(_radioHistoryData.myVolleyBallHistory, 5, sender);
      Serial.print("Satzverhaeltnis Radio");
      Serial.println(sender);
      Serial.print(t1_won);
      Serial.print(" : ");
      Serial.println(t2_won);
    }  //else silent update
  } else if (packetSize != 0) {
    // We have a packet with an unexpected size, either an invalid packet or packet
    // that was sent from a radio that we cannot handle, so remove it from the radio.

    Serial.print("Received packet with length '");
    Serial.print(packetSize);
    Serial.println("', it will be discarded");

    _radio.discardData(packetSize);
  }
  /* BLE*/
  else {
    if (deviceConnected && m_BLETime + 50 < millis() && NeedToNotifyBLE_BothMatches) {
      String i = "0,";
      i = i + mRadioScores[0].mT1 + ",";
      i = i + mRadioScores[0].mT2 + ",";
      i = i + mRadioScores[0].mS1 + ",";
      i = i + mRadioScores[0].mS2 + ",";
      i = i + mRadioScores[1].mT1 + ",";
      i = i + mRadioScores[1].mT2 + ",";
      i = i + mRadioScores[1].mS1 + ",";
      i = i + mRadioScores[1].mS2;
      Serial.println(i);
	  mBLEInterface.SendScroreToWebBLE(i);
      m_BLETime = millis();
      NeedToNotifyBLE_BothMatches = false;
      //value++;
      //Serial.print("New value notified: ");
      //Serial.println(value);
    }
    // disconnecting
    if (!deviceConnected && oldDeviceConnected && m_BLETime + 500 < millis()) {
      Serial.println("Device disconnected.");
      mBLEInterface.StartAdvertising();  // restart advertising
      Serial.println("Start advertising");
      oldDeviceConnected = deviceConnected;
      m_BLETime = millis();
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
      // do stuff here on connecting
      oldDeviceConnected = deviceConnected;
      Serial.println("Device Connected");
    }
  }
  //maybe LED screen need update
  if(NeedToNotifyLEDShort1)
  {
    NeedToNotifyLEDShort1= false;
    SendMatchDataShortToScoreBoard(1);
  }
  else   if(NeedToNotifyLEDShort2)
  {
    NeedToNotifyLEDShort2= false;
    SendMatchDataShortToScoreBoard(2);
  }
}


void radioInterrupt()
{
    // Ask the radio what caused the interrupt.  This also resets the IRQ pin on the
    // radio so a new interrupt can be triggered.

    uint8_t txOk, txFail, rxReady;
    _radio.whatHappened(txOk, txFail, rxReady);

    // txOk = the radio successfully transmitted data.
    // txFail = the radio failed to transmit data.
    // rxReady = the radio received data.

    if (rxReady)
    {
        //_dataWasReceived = true;
        Serial.println("we recieved data");
    }
}


void UpdateHistory(VolleyBallHistory TheArray[], int n, int sender) {
  mDisplay->UpdateHistory(TheArray,n,sender);
}
void SetNewScore(int T1, int T2, int S1, int S2,int sender) {
 mDisplay->SetNewScore(T1,T2,S1,S2,sender);
}



void DrehschalterDreh1() {
  //Serial.println("Dreh1");
  mDrehschalter.CLK_Wert_n = digitalRead(mDrehschalter._CLK);
  mDrehschalter.Relative_Position = digitalRead(mDrehschalter._DT);
}

/*
struct Drehschalter
{
  DrehPosition = 0;
  int Relative_Position = LOW;
  int Old_Relative_Position = LOW;
  int CLK_Wert_n = LOW;
  int Current_Taster = LOW;
  int Old_Taster = LOW;
  bool WasUpdated = false;
  ButtonPressedTime
};
*/
bool CheckInputState()  //return Anything changed? But only for one Rotary Encoder
{
  bool AnythingChanged = false;
  //this is done in interrupt now (faster updates?)
  //n = digitalRead(CLK);
  mDrehschalter.Current_Taster = !digitalRead(mDrehschalter._SW);

  /* long press Button -> submit set*/
  if (mDrehschalter.Current_Taster == false) {
    //Serial.println("Button not pressed");
    

    mDrehschalter.ButtonPressedTime = millis();
  } else if (mDrehschalter.Old_Taster) { 

    //long button press
    if (millis() - 1000 > mDrehschalter.ButtonPressedTime) {
      //Submit Sentence
      Serial.println("Submit a buttom change");
      mDrehschalter.ButtonPressedTime = millis() + 10000;
      mDrehschalter.OnLongPressButton = true;
      //HandleSetFinished();
      return true;
    }
  }

  if (mDrehschalter.Old_Taster != mDrehschalter.Current_Taster) {
    Serial.print(mDrehschalter.DrehPosition);
    Serial.print("|");
    Serial.print(mDrehschalter.Current_Taster);
    mDrehschalter.Old_Taster = mDrehschalter.Current_Taster;
    //this tells us to go in another mode
    if (mDrehschalter.OnLongPressButton)  //do not go in next mode if we long pressed button
    {
      mDrehschalter.OnLongPressButton = false;
      return false;
    }
    if (mDrehschalter.Old_Taster == false) {
      //short button press
      AnythingChanged = true;
      //This might be unnecessary. Give drehschalter a new  
    mUpdateScreenOne = true;
    mUpdateScreenTwo = true;
    }
  }
  if ((mDrehschalter.Old_Relative_Position == LOW) && (mDrehschalter.CLK_Wert_n == HIGH)) {
    if (mDrehschalter.Relative_Position == LOW) {
      mDrehschalter.DrehPosition++;
      //ModifyColor(true);
    } else {
       mDrehschalter.DrehPosition--;
      //ModifyColor(false);
    }
    Serial.print( mDrehschalter.DrehPosition);
    Serial.print("|");
    Serial.print(mDrehschalter.Current_Taster);
    AnythingChanged = true;
  }
  mDrehschalter.Old_Relative_Position = mDrehschalter.CLK_Wert_n;
  return AnythingChanged;
}

/*
struct __attribute__((packed)) MatchDataLong  // Any packet up to 32 bytes can be sent.
{
  uint8_t FromRadioId;
  uint8_t MatchNum; //we can track 3 matches
  uint8_t T1_Score = 2;
  uint8_t T2_Score = 3;
  uint8_t T1_Sets = 1 ;
  uint8_t T2_Sets = 2;
  char TeamName1[12] ;
  char TeamName2[12];
  uint32_t FailedTxCount;
};

struct __attribute__((packed)) MatchDataShort  // Any packet up to 32 bytes can be sent.
{
  uint8_t FromRadioId;
  uint8_t MatchNum; //we can track 3 matches
  uint8_t T1_Score = 2;
  uint8_t T2_Score = 3;
  uint8_t T1_Sets = 1 ;
  uint8_t T2_Sets = 2;
  uint32_t FailedTxCount;
};
*/
