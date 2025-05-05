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


BLEInterface mBLEInterface;
#define TFT_ROT 0
#include "NotoSansBold15.h"
#include "NotoSansBold36.h"
//#include "NotoSansMonoSCB20.h"
#include <pgmspace.h>
// The font names are arrays references, thus must NOT be in quotes ""
#define AA_FONT_SMALL NotoSansBold15
#define AA_FONT_LARGE NotoSansBold36
//#define AA_FONT_MONO NotoSansMonoSCB20  // NotoSansMono-SemiCondensedBold 20pt
#include <SPI.h>
#include <TFT_eSPI.h>  // Hardware-specific library
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite spr = TFT_eSprite(&tft);  // Sprite class needs to be invoked

const uint8_t xpos_volleyball = 15;
const uint8_t ypos_volleyball = 115;
const uint8_t xpos_fox = 170;
const uint8_t ypos_fox = 115;


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
const uint16_t KC_grey = 2113;
const uint16_t KC_dirty_white = 61309;
const uint16_t KC_DARKGREY = 31727;






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
  for(size_t t=0; t <12; t++)
  {
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
    if(_radio.send(8, &mMatchDataLong, sizeof(mMatchDataLong)))
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
  for(size_t t=0; t <12; t++)
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
    if(_radio.send(8, &mMatchDataLong, sizeof(mMatchDataLong)))
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
  Serial.print("Update Screen Color ");
  Serial.println(mCurrentRadioID);
  mUpdateScreenColor = true;
  //Clear screen - as we change radio id
  tft.fillCircle(60, 90, 25, mRadioScores[mCurrentRadioID].BackgroundColor);
  tft.fillCircle(180, 90, 25, mRadioScores[mCurrentRadioID].BackgroundColor);
  //tft.drawRect(60, 85, 100, 30, TFT_RED);
  tft.fillRect(60, 65, 120, 51, mRadioScores[mCurrentRadioID].BackgroundColor);
  //tft.fillRect(60, 10, 30, 51, TFT_YELLOW);
   spr.createSprite(80, 40);
  spr.fillSprite(KC_grey);
  spr.pushSprite(80, 20);  // Push to TFT screen coord 10, 10
  //Middle in green
  spr.deleteSprite();
  spr.createSprite(90, 97);
  spr.fillSprite(KC_dirty_white);
  spr.pushSprite(75, 121);
  spr.deleteSprite();
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
  
  if(_radio.send(8, &mMatchDataShort, sizeof(mMatchDataShort)))
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
  Serial.begin(9600);
  //attachInterrupt(digitalPinToInterrupt(PIN_RADIO_IRQ), radioInterrupt, FALLING);
  pinMode(mDrehschalter._CLK,INPUT);
  pinMode(mDrehschalter._DT,INPUT);
  pinMode(mDrehschalter._SW,INPUT);
  
      /*if (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN)) {
    Serial.println("Cannot communicate with radio");
    //while (1)
      ;  // Wait here forever.
  }*/
    if (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN)) {
    Serial.println("Cannot communicate with radio");
    //while (1)
      ;  // Wait here forever.
  }
  tft.begin();

  mCurrentRadioID = 0;
  mRadioScores[0].BackgroundColor = 3697;
  mRadioScores[1].BackgroundColor = 61521;  //tft.color565(240,11,140);
  mRadioScores[2].BackgroundColor = 2142;   //tft.color565(11,11,240);
  //Serial.println(tft.color565(240,11,140));
  //Serial.println(tft.color565(11,11,240));
  //SPI.begin(PIN_RADIO_SCK, PIN_RADIO_MISO, PIN_RADIO_MOSI, PIN_RADIO_CSN);

  // Indicate to NRFLite that it should not call SPI.begin() during initialization since it has already been done.


  attachInterrupt(digitalPinToInterrupt(mDrehschalter._CLK), DrehschalterDreh1, CHANGE);
  tft.setRotation(TFT_ROT);

  spr.setColorDepth(16);  // 16 bit colour needed to show antialiased fonts

  tft.fillScreen(KC_grey);
  //tft.fillRect(0, 90, 240, 240, dirty_white);
  tft.fillCircle(60, 180, 90, KC_DARKGREY);
  tft.fillCircle(180, 180, 90, KC_DARKGREY);


  tft.fillCircle(60, 180, 88, KC_dirty_white);
  tft.fillCircle(180, 180, 88, KC_dirty_white);

  tft.fillRect(60, 90, 120, 240, KC_dirty_white);

  //tft.drawLine(50, 115, 0, 240, TFT_DARKGREY);
  //tft.drawLine(190, 115, 240, 240, TFT_DARKGREY);
  DrawVolleyball();
  DrawFox();
  //tft.drawCircle(120, 120, 120, TFT_DARKGREY);
  //tft.drawCircle(120, 120, 119, TFT_DARKGREY);
  tft.drawArc(120, 120, 120, 119, 270, 90, KC_DARKGREY, KC_DARKGREY);
  /*uint16_t bluish = tft.color565(11,206,140);
  Serial.println("redish");
  Serial.println(tft.color565(206,11,140));
    Serial.println("yellowish");
  Serial.println(tft.color565(206,140,11));*/
  /* start here*/
  //border
  tft.fillCircle(60, 90, 27, KC_DARKGREY);
  tft.fillCircle(180, 90, 27, KC_DARKGREY);
  tft.fillRect(60, 63, 120, 55, KC_DARKGREY);
  //real obj


  tft.fillCircle(60, 90, 25, mRadioScores[0].BackgroundColor);
  tft.fillCircle(180, 90, 25, mRadioScores[0].BackgroundColor);
  //tft.drawRect(60, 85, 100, 30, TFT_RED);
  tft.fillRect(60, 65, 120, 51, mRadioScores[0].BackgroundColor);

  //debug lines
  /*tft.drawLine(120, 0, 120, 240, TFT_RED);
  tft.drawLine(0, 120, 240, 120, TFT_RED);
    tft.drawLine(140, 0, 140, 240, TFT_RED);
      tft.drawLine(100, 0, 100, 240, TFT_RED);*/

  //request an radio update
  //have to do it after tft is setup... else we fail with listening


  /* BLE */
  // Create the BLE Device
  mBLEInterface = *new BLEInterface();
  mBLEInterface.initDevice();
  Serial.println("Waiting a client connection to notify...");
  _radioCmdData.CommandId = 0;
  _radio.send(mCurrentRadioID + 1, &_radioCmdData, sizeof(_radioCmdData));

  //test
}

void loop() {
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
  CheckInputState();
  if (mUpdateScreenColor) {
    UpdateScreenColor();
    mUpdateScreenColor = false;
    _radioCmdData.CommandId = 0;
    //update my score pls
    _radio.send(mCurrentRadioID + 1, &_radioCmdData, sizeof(_radioCmdData));
    Serial.println("we sent cmd to radio");
    return;
  }
  if (NeedToNotifyTFT) {
    Serial.println("need to notify tft");
    //in this case we only call this fct from BLE
    SetNewScore(mRadioScores[mCurrentRadioID].mT1, mRadioScores[mCurrentRadioID].mT2, mRadioScores[mCurrentRadioID].mS1, mRadioScores[mCurrentRadioID].mS2);
    NeedToNotifyTFT = false;
    //no return?
    //and multiple tasks allowed?
    //finally update Scoreboard
    
  }
  /*if(Serial.available()) 
    {
  // TODO
    Serial.read();
    _radio.send(DESTINATION_RADIO_ID, &_radioCmdData, sizeof(_radioCmdData));
    
    Serial.println("serial av");
    }*/
  if (NeedToNotifyRadio) {
    Serial.println("Inform Radio now");
    _radioCmdData.CommandId = 1;
    _radioCmdData.CommandArg1 = mRadioScores[mCurrentRadioID].mT1;
    _radioCmdData.CommandArg2 = mRadioScores[mCurrentRadioID].mT2;
    _radioCmdData.CommandArg3 = mRadioScores[mCurrentRadioID].mS1;
    _radioCmdData.CommandArg4 = mRadioScores[mCurrentRadioID].mS2;
    _radio.send(mCurrentRadioID + 1, &_radioCmdData, sizeof(_radioCmdData));
    NeedToNotifyRadio = false;

  } else if (CommandMode.MessageRecieved) {
    _radioCmdData.CommandId = CommandMode.CommandId;
    _radioCmdData.CommandArg1 = CommandMode.Arg1;
    CommandMode.MessageRecieved = false;
    _radio.send(mCurrentRadioID + 1, &_radioCmdData, sizeof(_radioCmdData));
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
    /*String msg = "Radio ";
        msg += _radioData.FromRadioId;
        msg += ", ";
        msg += _radioData.T1_Score;
        msg += " : ";
        msg += _radioData.T2_Score;
        msg += " sets: ";
        msg += _radioData.T1_Sets;
        msg += " : ";
        msg += _radioData.T2_Sets;*/
    //Serial.println(msg);
    mRadioScores[sender].mT1 = _radioData.T1_Score;
    mRadioScores[sender].mT2 = _radioData.T2_Score;
    mRadioScores[sender].mS1 = _radioData.T1_Sets;
    mRadioScores[sender].mS2 = _radioData.T2_Sets;
    if (sender == mCurrentRadioID)  //only update if we have same radio id ... i.e. we selected to "show" the radio data
    {
      SetNewScore(_radioData.T1_Score, _radioData.T2_Score, _radioData.T1_Sets, _radioData.T2_Sets);
      NeedToNotifyBLE = true;
    }
    NeedToNotifyLEDShort = true; //we may specify which matchdata
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
    if (sender == mCurrentRadioID) {
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
    if (deviceConnected && m_BLETime + 50 < millis() && NeedToNotifyBLE) {
      String i = "0,";
      char buffer[2];  //the ASCII of the integer will be stored in this char array
      i = i + mRadioScores[mCurrentRadioID].mT1 + ",";
      i = i + mRadioScores[mCurrentRadioID].mT2 + ",";
      i = i + mRadioScores[mCurrentRadioID].mS1 + ",";
      i = i + mRadioScores[mCurrentRadioID].mS2;
      Serial.println(i);
      
	  mBLEInterface.SendScroreToWebBLE(i);
      m_BLETime = millis();
      NeedToNotifyBLE = false;
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
  if(NeedToNotifyLEDShort)
  {
    NeedToNotifyLEDShort = false;
    SendMatchDataShortToScoreBoard(mCurrentRadioID);
  }
  return;
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
  int xpos = tft.width() / 2;  // Half the screen width
  int ypos = 50;
  int counter = 5;
  int MaxSpace = counter * 19 + 2;
  spr.createSprite(90, MaxSpace);
  spr.fillSprite(KC_dirty_white);
  spr.setTextColor(TFT_BLACK, KC_DARKGREY);
  spr.loadFont(AA_FONT_SMALL);
  //spr.loadFont(AA_FONT_MONO);
  for (size_t i = 0; i < n; i++) {
    if (!TheArray[i].Set) {

    } else {
      //spr.drawString("(1)",5,i*20);
      spr.print("(");
      int j = TheArray[i].Satz + 1;
      spr.print(j);
      spr.print(") ");
      j = (int)TheArray[i].Team1;
      spr.printf("%02d : ", j);
      j = (int)TheArray[i].Team2;
      spr.printf("%02d", j);
      spr.println("");
      /*spr.drawString("02",30,i*20);
      spr.drawString(":",55,i*20);
       spr.drawString("15",65,i*20);*/
    }
  }
  spr.pushSprite(75, 121);
  spr.deleteSprite();
  /*if(counter <= 3)
  {
    spr.deleteSprite();
    spr.loadFont(AA_FONT_MONO);
    spr.setTextSize(10);
    spr.createSprite(135, 20);
    spr.fillSprite(dirty_white);
    spr.drawString("Beachfreunde",0,0);
    spr.pushSprite(58, 190); 
    spr.deleteSprite();
    spr.createSprite(48,20);
    spr.fillSprite(dirty_white);
    
    //ö  = 0 + zwei Punkte
    spr.drawString("Pohl",0,0);
    spr.fillCircle(14,3,1,TFT_BLACK);
    spr.fillCircle(18,3,1,TFT_BLACK);
    spr.pushSprite(100, 210); 
  }*/
}
void SetNewScore(int T1, int T2, int S1, int S2) {
  int xpos = tft.width() / 2;  // Half the screen width
  int ypos = 50;
  delay(20);

  spr.loadFont(AA_FONT_LARGE);
  spr.createSprite(80, 40);
  spr.fillSprite(KC_grey);
  spr.setTextColor(KC_dirty_white, KC_DARKGREY);
  spr.drawNumber(S1, 0, 00);
  spr.drawString(":", 35, 00);
  spr.drawNumber(S2, 60, 0);
  spr.pushSprite(80, 20);  // Push to TFT screen coord 10, 10
  //Middle in green
  spr.deleteSprite();
  spr.createSprite(124, 45);
  spr.setTextColor(TFT_WHITE, KC_DARKGREY);
  spr.fillSprite(mRadioScores[mCurrentRadioID].BackgroundColor);
  if (T1 < 10) {
    spr.drawNumber(0, 0, 10);
    spr.drawNumber(T1, 22, 10);
  } else
    spr.drawNumber(T1, 0, 10);
  spr.drawString(":", 55, 10);
  if (T2 < 10) {
    spr.drawNumber(0, 80, 10);
    spr.drawNumber(T2, 102, 10);
  } else
    spr.drawNumber(T2, 80, 10);
  spr.pushSprite(60, 67);
  spr.deleteSprite();
}

void DrawVolleyball() {
  uint16_t m_counter = 0;
  uint8_t NewY = 0;
  uint8_t NewX = 0;
  for (int i = 0; i < 2500; i++) {
    if (m_counter >= 50) {
      m_counter = m_counter - 1;
      NewY++;
      NewX = 0;
      m_counter = 0;
    }
    if (61309 != volleyball[i])
      tft.drawPixel(NewX + xpos_volleyball, NewY + ypos_volleyball, volleyball[i]);
    NewX++;
    m_counter++;
  }
}


void DrawFox() {
  uint16_t m_counter = 0;
  uint8_t NewY = 0;
  uint8_t NewX = 0;
  //53 * 50
  for (int i = 0; i < 2650; i++) {
    if (m_counter >= 53) {
      m_counter = m_counter - 1;
      NewY++;
      NewX = 0;
      m_counter = 0;
    }
    if (61309 != fox_pic[i])
      tft.drawPixel(NewX + xpos_fox, NewY + ypos_fox, fox_pic[i]);
    NewX++;
    m_counter++;
  }
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

      mCurrentRadioID = mCurrentRadioID + 1;
      if (mCurrentRadioID >= 3)
        mCurrentRadioID = 0;
        mUpdateScreenColor = true;
      Serial.print("|");
      Serial.println(mCurrentRadioID);
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
    Serial.print("|");
    Serial.println(mCurrentRadioID);
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
