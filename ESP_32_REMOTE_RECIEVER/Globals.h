//pins

//tft in user_setup.h
#pragma once

const static uint8_t RADIO_ID = 0;
const static uint8_t PIN_RADIO_CE = 26;
const static uint8_t PIN_RADIO_CSN = 27;
const static uint8_t PIN_RADIO_MOSI = 23;
const static uint8_t PIN_RADIO_MISO = 19;
const static uint8_t PIN_RADIO_SCK = 18;
const static uint8_t PIN_RADIO_IRQ = 25;


//drehschalter
volatile uint8_t _dataWasReceived; // Note usage of volatile since the variable is used in the radio interrupt
                                   // while also being used outside the interrupt.
struct Drehschalter
{
  //pins
  const uint8_t _CLK = 21;
  const uint8_t _DT = 5;
  const uint8_t _SW = 17;

  int DrehPosition = 0;
  int Relative_Position = LOW;
  int Old_Relative_Position = LOW;
  int CLK_Wert_n = LOW;
  int Current_Taster = LOW;
  int Old_Taster = LOW;
  bool WasUpdated = false;
  unsigned long ButtonPressedTime = 0;
  bool OnLongPressButton = false;
};


/*shared betweend classes*/

bool NeedToNotifyBLE = false;
bool NeedToNotifyTFT1 = false;
bool NeedToNotifyTFT2 = false;
bool NeedToNotifyRadio1 = false;
bool NeedToNotifyRadio2 = false;
bool mUpdateScreenColor = false;
bool NeedToNotifyLEDShort1 = false;
bool NeedToNotifyLEDShort2 = false;
bool NeedToNotifyLEDCurrentMatchData1 = false;
bool NeedToNotifyLEDCurrentMatchData2 = false;
int LEDScreenMode =0;
bool NeedToUpdateScreenMode = false;

//Radio

struct __attribute__((packed)) Commandpackage  // Any packet up to 32 bytes can be sent.
{
  uint8_t CommandId = 0;
  uint8_t CommandArg1 = 0;
  uint8_t CommandArg2 = 0;
  uint8_t CommandArg3 = 0;
  uint8_t CommandArg4 = 0;
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

//ScoreBoard
struct __attribute__((packed)) MatchDataLong  // Any packet up to 32 bytes can be sent.
{
  uint8_t MatchNum; //we can track 3 matches
  uint8_t type; //1-3 are current matchdata //4-6 are next match data
  char TeamName1[12];
  char TeamName2[12];
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
typedef struct
{
  uint8_t Team1;
  uint8_t Team2;
  uint8_t Satz;
  uint8_t Winner;  //0 is no winner 1 -> T1 ; 2 ->t2;
  bool Set = false;
} VolleyBallHistory;


struct __attribute__((packed)) RadioHistoryPacket  // Any packet up to 32 bytes can be sent.
{
  uint8_t FromRadioId;
  VolleyBallHistory myVolleyBallHistory[5];
  uint32_t FailedTxCount;
};

//ble 

struct CommandMsg {
  bool MessageRecieved;
  int CommandId = 0;
  int Arg1 = 0;
};

CommandMsg CommandMode;
int mValidTeamNamesField1 = 30;
int mValidTeamNamesField2 =30;
volatile bool NeedToSendMachData_1_CurrentMatch = false;
volatile bool NeedToSendMachData_1_NextMatch = false;
volatile bool NeedToSendMachData_2_CurrentMatch = false;
volatile bool NeedToSendMachData_2_NextMatch = false;

bool deviceConnected = false;
bool oldDeviceConnected = false;
uint32_t value = 0;
uint32_t m_BLETime = 0;

//More structs



struct ScoreBoard_over_Raio {
  uint8_t mT1 = 0;
  uint8_t mT2 = 0;
  uint8_t mS1 = 0;
  uint8_t mS2 = 0;
  char TeamName1[12];
  char TeamName2[12];
  VolleyBallHistory MyVolleyHistory[5];
  uint16_t BackgroundColor;
};

uint8_t mCurrentRadioID = 0;
//this id -1
ScoreBoard_over_Raio mRadioScores[3];
// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
//up to 15 games (30/2)
char MatchNameData_Field1[12][30];
volatile int currentMatch_Field1= -1;
char MatchNameData_Field2[12][30];
volatile int currentMatch_Field2 = -1;