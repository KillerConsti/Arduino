#include "C:\Users\49151\Documents\Arduino\beachfreunde_anzeige_mit_runddisplay_gc901a_copy_no_blink\settings.h"



#include <EEPROM.h>
#include <Arduino.h>
#include <NRFLite.h>

#include <C:\Users\49151\Documents\Arduino\beachfreunde_anzeige_mit_runddisplay_gc901a_copy_no_blink\led_anzeige.h>

#ifdef UseRoundDisplay
#include <C:\Users\49151\Documents\Arduino\beachfreunde_anzeige_mit_runddisplay_gc901a_copy_no_blink\Display_GC901A.h>
RoundDisplay display;
#else
#include <C:\Users\49151\Documents\Arduino\beachfreunde_anzeige_mit_runddisplay_gc901a_copy_no_blink\Display_Waveshare_st7789.h>
SquareDisplay display;
#endif

#include "tm1638.h"

tm1638 tm;

LED_MATRIX rgb_mat;

#ifdef USE_ESP32
#include <C:\Users\49151\AppData\Local\Arduino15\packages\esp32\hardware\esp32\2.0.11\libraries\SPI\src\SPI.h>
/*SPI PINS for Reference
    SCK (SCL)	13 (fest)
    SDA /MOSI	11 (fest)
    MISO	12 (fest)*/
//they are defined in led_anzeige aswell
#define DATA_PIN 3
#define DATA2_PIN 21
//drehschalter
int CLK = 27;
int DT = 14;
int SW = 12;
int b_oben_rechts = 32;
int b_unten_rechts = 33;
int b_links_oben = 25;
int b_links_unten = 26;
const static uint8_t PIN_RADIO_CE = 2;
const static uint8_t PIN_RADIO_CSN = 15;

#else /* UNO */
#include "SPI.h"
#define DATA_PIN 1
#define DATA2_PIN 0
//drehschalter
int CLK = 2;
int DT = 3;
int SW = 4;
//buttons
#ifdef SwapWiringButtons
 //Thomas Lösung
int b_oben_rechts = 6;
int b_unten_rechts = 5;
int b_links_oben = 8;
int b_links_unten = 7;
#else
int b_oben_rechts = 8;
int b_unten_rechts = 7;
int b_links_oben = 6;
int b_links_unten = 5;
#endif


//funker
const static uint8_t PIN_RADIO_CE = 9;
const static uint8_t PIN_RADIO_CSN = 10;
#endif

int left_number_value;
int right_nmber_value;
int left_set_value;
int right_set_value;
int currentPalette_index_LeftNumber;
int currentPalette_index_MiddleNumber;
int currentPalette_index_RightNumber;
int Helligkeit;
//Bedienungselements


bool IgnoreUpdateLeftBecauseSwap = false;
bool IgnoreUpdateRightBecauseSwap = false;
//Timer Funktions - track time

unsigned long StartTime;
unsigned long CurrentTime;
unsigned long ButtonTimeSubmitSet = 0;
unsigned long ButtonTimeLeftUP = 0;
unsigned long ButtonTimeLeftDown = 0;
unsigned long ButtonTimeRightUP = 0;
unsigned long ButtonTimeRightDown = 0;
unsigned long ButtonTimeReset = 0;

unsigned long ButtonTimeSwap = 0;
int ModifyButtonState = 0;
bool ColorVal_changed = false;
bool OldBlinkState;
bool SubmitSetChange = false;
bool DoNotUpdateHistoryOnDisplay = false;
bool PartyMode = false;
VolleyBallHistory MyVolleyHistory[10];

typedef struct
{
  int Points1 = 0;
  int Points2 = 0;
  int Set1 = 0;
  int Set2 = 0;  //0 is no winner 1 -> T1 ; 2 ->t2;
  bool UpdateColorLeft;
  bool UpdateColorRight;
  bool UpdateColorMiddle;
} Score;
Score mNewScore;
Score mOldScore;
//color palette
CRGBPalette16 myPalette(
  CRGB::Amethyst,
  CRGB::AntiqueWhite,
  CRGB::Red,
  CRGB::Yellow,
  CRGB::Green,
  CRGB::Blue,
  CRGB::Purple,
  CRGB::Black,

  0x100000,
  0x200000,
  0x400000,
  0x800000,

  CHSV(30, 255, 255),
  CHSV(50, 255, 255),
  CHSV(70, 255, 255),
  CHSV(90, 255, 255));

/*Display stuff */

int mOldModifyButtonState;
typedef struct
{
  bool ModeChanged =false;
  bool ScoreChanged = false;
  bool SetsChanged = false;
  bool NeedToCareForHistoryLimit = false;
  bool RGBMAT_Update = false;
  bool mUpdateForcedByRadio = false;
  bool UpdateHistoryOnDisplay =false;
} ReasonForUpdate;
ReasonForUpdate WhatIsTheReason;
int tries = 0;
bool AnythingChanged(ReasonForUpdate MyReason)
{
  return MyReason.ModeChanged || MyReason.ScoreChanged || MyReason.SetsChanged || MyReason.NeedToCareForHistoryLimit || MyReason.RGBMAT_Update || MyReason.UpdateHistoryOnDisplay; 
}
bool mForceUpdateESPRadio = false;
// If display breakout has a backlight control pin, that can be defined here
// as TFT_BL. On some breakouts it's not needed, backlight is always on.


//datapin RGB Matrix = 1

//drehschalter
int Position = 0;
int Neue_Position = LOW;
int Letzte_Position = LOW;
int n = LOW;
int Taster = LOW;
int Letzte_Taster = LOW;

//4 buttons

int b_oben_rechts_old_state = LOW;

int b_unten_rechts_old_state = LOW;

int b_links_oben_old_state = LOW;

int b_links_unten_old_state = LOW;

/*pinout Sendemodul NRF24L01
//von oben links (viereckiger Pol)
GND	GND 
VCC	3,3 V (NIEMALS 5V)
CE	9
CSN	10 (fest)
SCK (SCL)	13 (fest)
SDA /MOSI	11 (fest)
MISO	12 (fest)
IRQ	unused
*/
/* SPI pins sind sowieso gesetzt

//Pinout TFT
SDA - 11 (hard)
SCL - 13 (hard)
DC    14 (A0)
CS    15 (A1)
RS    16 (A2)
*/





/*radio */


#if RADIO == 1
const static uint8_t RADIO_ID = 1;  // Our radio's id.
//const static uint16_t KC_BackgroundColor = 61521;
#endif
#if RADIO == 2
const static uint8_t RADIO_ID = 2;  // Our radio's id.
#endif
#if RADIO == 3
const static uint8_t RADIO_ID = 3;  // Our radio's id.
#endif
const static uint8_t DESTINATION_RADIO_ID = 0;   // Id of the radio we will transmit to.
struct __attribute__((packed)) RadioScorePacket  // Any packet up to 32 bytes can be sent.
{
  uint8_t FromRadioId = RADIO_ID;
  uint8_t T1_Score;
  uint8_t T2_Score;
  uint8_t T1_Sets;
  uint8_t T2_Sets;
  uint32_t FailedTxCount;
};

struct __attribute__((packed)) RadioHistoryPacket  // Any packet up to 32 bytes can be sent.
{
  uint8_t FromRadioId = RADIO_ID;
  VolleyBallHistory myVolleyBallHistory[5];
  uint32_t FailedTxCount;
};
struct __attribute__((packed)) Commandpackage  // Any packet up to 32 bytes can be sent.
{
  uint8_t CommandId = 0;
  uint8_t CommandArg1 = 0;
  uint8_t CommandArg2 = 0;
  uint8_t CommandArg3 = 0;
  uint8_t CommandArg4 = 0;
};
//we wont use delay
bool mRadioNeedUpdateScore = false;
bool mRadioHistoryNeedUpdate = false;
unsigned long mRadioTime = 0;
NRFLite _radio;
RadioScorePacket _radioScoreData;
RadioHistoryPacket _radioHistoryData;
Commandpackage _radioCommandData;

/** NEW ESP HAND REMOTE **/

// ESP Remote RADIO DATA (weißer Kasten)

//1. What we send
struct __attribute__((packed)) Radio_Remote_Commandpackage  // Any packet up to 32 bytes can be sent.
{
  uint8_t FromRadioId;
  uint8_t CommandId = 0;
  uint8_t CommandArg1 = 0;
};

//Commands
enum Radio_Remote_Commands
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
uint8_t mESP_RADIO = RADIO +3;
Radio_Remote_Commandpackage mRemoteCommandPackage;
//ESP REMOTE END

/**/
void setup() {
  Serial.begin(9600);
  //EEPROM.write(0, 31); //we only did this once for setup board no longer needed for that board
  //EEPROM.write(1, 79);
  //EEPROM.write(2, 63);

  Serial.println("Initialization starting!");
  //init our sensors
  // initialize the pushbutton pin as an input:
  pinMode(b_oben_rechts, INPUT);
  pinMode(b_unten_rechts, INPUT);
  pinMode(b_links_unten, INPUT);
  pinMode(b_links_oben, INPUT);
  //Drehschalter
  pinMode(CLK, INPUT_PULLUP);
  pinMode(DT, INPUT_PULLUP);
  pinMode(SW, INPUT_PULLUP);


  //set first values for buttons:)
  b_oben_rechts_old_state = digitalRead(b_oben_rechts);
  b_unten_rechts_old_state = digitalRead(b_unten_rechts);
  b_links_oben_old_state = digitalRead(b_links_oben);
  b_links_unten_old_state = digitalRead(b_links_unten);

  //load current Time - needed for blinking etc
  StartTime = millis();
  //may load color from EEPROM later
  currentPalette_index_LeftNumber = EEPROM.read(0);
  currentPalette_index_MiddleNumber = EEPROM.read(1);
  currentPalette_index_RightNumber = EEPROM.read(2);
  Helligkeit = EEPROM.read(3);
  Helligkeit = constrain(Helligkeit, 10, 255);
  //display.SetBrightness(Helligkeit);
  //set current display values

  left_number_value = 0;
  right_nmber_value = 0;
  left_set_value = 0;
  right_set_value = 0;

  //FastLED
  delay(3000);  // 3 second delay for recovery
  Serial.println("init led ..");
  // tell FastLED about the LED strip configuration
  //LED_MATRIX::init();
  #ifdef Mirror_LEDS
  rgb_mat.Setup(DATA_PIN, true,DATA2_PIN);
  #endif
    #ifndef Mirror_LEDS
     rgb_mat.Setup(DATA_PIN, false,DATA2_PIN);
    #endif

  rgb_mat.Set_Brightness(Helligkeit);

  //set to -1 so we enforce an update at "SetNewNumber"
  mOldScore.Points1 = -15;
  mOldScore.Points2 = -15;
  mOldScore.Set1 = -1;
  mOldScore.Set2 = -1;
  SetNewNumber(true);
  //u8g2.begin();
  display.InitDisplay();
  //Serial.print("Helligkeit ");
  //Serial.println(Helligkeit);
  Serial.println("finished");

  //Test Drehknopf mit interrupt
  //Vorteil keine Falschen Richtungen mehr
  attachInterrupt(digitalPinToInterrupt(CLK), DrehschalterDreh1, CHANGE);
  //init Radio
  if (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN)) {
    Serial.println("Cannot communicate with radio");
    //while (1)
      ;  // Wait here forever.
  }

  _radioScoreData.FromRadioId = RADIO_ID;
}


void SetNewNumber(bool InformRadio)  //Set Number but only on event
{
  //updates are heavy and block input so try to make less heavy updates by a lot if's
  //do specific checks which areas need inputs
  /*Score mNewScore;
  Score mOldScore;*/

  mNewScore.Points1 = left_number_value;
  mNewScore.Points2 = right_nmber_value;
  mNewScore.Set1 = left_set_value;
  mNewScore.Set2 = right_set_value;

  if (InformRadio && mNewScore.Points1 != mOldScore.Points1 || mNewScore.Set1 != mOldScore.Set1 || mNewScore.Points2 != mOldScore.Points2 || mNewScore.Set2 != mOldScore.Set2) {
    //Inform RadioSystem
    mRadioNeedUpdateScore = true;
  }

  
  //left side
  if (mNewScore.UpdateColorLeft || mNewScore.Points1 != mOldScore.Points1) {
    mRadioNeedUpdateScore = true;
    rgb_mat.SetNewNumberLeft(left_number_value,currentPalette_index_LeftNumber);
  }

  //minis
  {
   rgb_mat.SetNewNumberMiddle(left_set_value,right_set_value,currentPalette_index_MiddleNumber);

  }
  if (mNewScore.UpdateColorRight || mNewScore.Points2 != mOldScore.Points2)
  //right side
  {
    rgb_mat.SetNewNumberRight(right_nmber_value,currentPalette_index_RightNumber);
    mNewScore.UpdateColorRight = false;
  }
  mOldScore = mNewScore;
  rgb_mat.Show();
  tm.SetNewScore(mNewScore.Points1,mNewScore.Points2,mNewScore.Set1,mNewScore.Set2);
}

void ModifyColor(bool GoUP) {
  //4 ist auch hier weil einfacher aber macht einiges anders
  //nutze speicheradressen hier -einfacher
  int* Colorindex = NULL;
  int ZwischenValue;
  if (ModifyButtonState == 1) {
    
    #ifndef Mirror_LEDS
    mNewScore.UpdateColorLeft = true;
    Colorindex = &currentPalette_index_LeftNumber;
    #else
    mNewScore.UpdateColorRight = true;
    Colorindex = &currentPalette_index_RightNumber;
    #endif
  } else if (ModifyButtonState == 2) {
    mNewScore.UpdateColorMiddle = true;
    Colorindex = &currentPalette_index_MiddleNumber;
  } else if (ModifyButtonState == 3) {
    
     #ifndef Mirror_LEDS
     mNewScore.UpdateColorRight = true;
    Colorindex = &currentPalette_index_RightNumber;
    #else
    mNewScore.UpdateColorLeft = true;
     Colorindex = &currentPalette_index_LeftNumber;
     #endif
  } else if (ModifyButtonState == 4)
  {
    Colorindex = &Helligkeit;
    display.SetBrightness(Helligkeit);
  }
  if (Colorindex == NULL)  //Element gibt es nicht?
    return;

  if (GoUP) {
    ZwischenValue = *Colorindex + 16;
    if (ModifyButtonState == 4)  //do not reset brightness
    {
      if (ZwischenValue < 100)
        //ZwischenValue= ZwischenValue+10;
        ZwischenValue = ZwischenValue - 8;
      ZwischenValue = constrain(ZwischenValue, 5, 255);
      rgb_mat.Set_Brightness(ZwischenValue);

    } else if (ZwischenValue > 255)
      ZwischenValue = 0;
  } else {
    ZwischenValue = *Colorindex - 16;
    if (ModifyButtonState == 4)  //do not reset brightness
    {
      if (ZwischenValue < 100)
        //ZwischenValue= ZwischenValue+10;
        ZwischenValue = ZwischenValue + 10;
      ZwischenValue = constrain(ZwischenValue, 5, 255);
      rgb_mat.Set_Brightness(ZwischenValue);
      //if(ZwischenValue != Helligkeit)

    } else if (ZwischenValue < -1)
      ZwischenValue = 255;
    else if (ZwischenValue == -1)
      ZwischenValue = 0;
  }
  *Colorindex = ZwischenValue;
  ColorVal_changed = true;  //inform eeprom to save color
  Serial.println(ZwischenValue);
  if (ModifyButtonState == 4)
    display.DrawBrightness();  //update Display but after we set a new Brightness value ... thats what *Colorindex = ZwischenValue does
}

bool CheckInputState()  //return Anything changed?
{
  uint16_t RemoteCode = tm.EventKeyTriggered(ModifyButtonState);
  if(RemoteCode != 0)
  {
    ExecuteRemoteCode(RemoteCode);
    return true;
  }
  bool AnythingChanged = false;
  //this is done in interrupt now (faster updates?)
  //n = digitalRead(CLK);
  Taster = !digitalRead(SW);
  /* long press Button -> submit set*/
  if (Taster == false) {
    //Serial.println("Button not pressed");

    ButtonTimeSubmitSet = CurrentTime;
  } else if (Letzte_Taster) {
    if (CurrentTime - 1000 > ButtonTimeSubmitSet) {
      //Submit Sentence
      Serial.println("Submit a change");
      ButtonTimeSubmitSet = CurrentTime + 10000;
      SubmitSetChange = true;
      HandleSetFinished();
      return true;
    }
  }
  /* mod -> submit set<- end here*/
  if (Taster != Letzte_Taster) {
    Serial.print(Position);
    Serial.print("|");
    Serial.print(Taster);

    //kc can  we take it out?
    //delay(10);
    Letzte_Taster = Taster;
    //this tells us to go in another mode
    if (SubmitSetChange)  //do not go in next mode if we long pressed button
    {
      SubmitSetChange = false;
      return false;
    }
    if (Letzte_Taster == false) {
      AnythingChanged = true;
      if (ColorVal_changed)  //write new color to EEPROM -stays in memory even after reset
      {
        if (ModifyButtonState == 1)
        #ifndef Mirror_LEDS
          EEPROM.write(0, currentPalette_index_LeftNumber);
        #else
          EEPROM.write(2, currentPalette_index_RightNumber);
        #endif
        else if (ModifyButtonState == 2)
          EEPROM.write(1, currentPalette_index_MiddleNumber);
        else if (ModifyButtonState == 3)
         #ifndef Mirror_LEDS
          EEPROM.write(2, currentPalette_index_RightNumber);
               #else
          EEPROM.write(0, currentPalette_index_LeftNumber);
        #endif
        else if (ModifyButtonState == 4)
          EEPROM.write(3, Helligkeit);
      }
      ColorVal_changed = false;
      WhatIsTheReason.ModeChanged = true;
      WhatIsTheReason.UpdateHistoryOnDisplay = true;
      ModifyButtonState = ModifyButtonState + 1;
      if (ModifyButtonState >= 5)
        ModifyButtonState = 0;
      Serial.print("|");
      Serial.println(ModifyButtonState);
    }
  }
  if ((Letzte_Position == LOW) && (n == HIGH)) {
    if (Neue_Position == LOW) {
      Position++;
      ModifyColor(true);
      WhatIsTheReason.RGBMAT_Update = true;
    } else {
      Position--;
      ModifyColor(false);
      WhatIsTheReason.RGBMAT_Update = true;
    }
    Serial.print(Position);
    Serial.print("|");
    Serial.print(Taster);
    Serial.print("|");
    Serial.println(ModifyButtonState);
    
    AnythingChanged = true;
  }
  Letzte_Position = n;

  /* finally check if a button changed
  *
  */

  //normally button pressed left or right number goes up or down
  //check if a delay is inbetween (so button is not registered twice)
  //if ColorChange Mode is 2 (middle colors are selected we change instead the set value from the middle position

  /*we take care only increase/decrease number if button released
  * 200ms at least inbetween
  * special state for mode 2 (sets are selected)
  * -there we increase sets
  * -there is a ignore property which disable increasing that is used when doing "easy swap" by pressing both upper buttons
  * do it 4 times 4 each button
  */

  //upper right
  if (b_oben_rechts_old_state != digitalRead(b_oben_rechts)) {
    b_oben_rechts_old_state = digitalRead(b_oben_rechts);
    Serial.print("b_oben_rechts|");
    Serial.println(b_oben_rechts_old_state);
    if (ButtonTimeRightUP + 200 < millis() && b_oben_rechts_old_state == false) {
      AnythingChanged = true;
      ButtonTimeRightUP = millis();
      if (IgnoreUpdateRightBecauseSwap)
        IgnoreUpdateRightBecauseSwap = false;
      else if (ModifyButtonState != 2) {
        right_nmber_value = constrain(right_nmber_value + 1, 0, 99);
        WhatIsTheReason.ScoreChanged = true;
      } else {
        WhatIsTheReason.SetsChanged = true;
        WhatIsTheReason.UpdateHistoryOnDisplay = true;
        right_set_value = constrain(right_set_value + 1, 0, 3);
      }
    }
  }
  //down right
  if (b_unten_rechts_old_state != digitalRead(b_unten_rechts)) {
    b_unten_rechts_old_state = digitalRead(b_unten_rechts);
    Serial.print("b_unten_rechts|");
    Serial.println(b_unten_rechts_old_state);
    if (ButtonTimeRightDown + 200 < millis() && b_unten_rechts_old_state == false) {
      AnythingChanged = true;
      ButtonTimeRightDown = millis();
      if (ModifyButtonState != 2)
      {
        right_nmber_value = constrain(right_nmber_value - 1, 0, 99);
        WhatIsTheReason.ScoreChanged = true;
      }
      else {
        WhatIsTheReason.SetsChanged = true;
        WhatIsTheReason.UpdateHistoryOnDisplay = true;
        right_set_value = constrain(right_set_value - 1, 0, 3);
      }
    }
  }

  //upper left
  if (b_links_oben_old_state != digitalRead(b_links_oben))  //ohne Einrasten
  {
    b_links_oben_old_state = digitalRead(b_links_oben);
    Serial.print("b_links_oben|");
    Serial.println(b_links_oben_old_state);
    if (ButtonTimeLeftUP + 200 < millis() && b_links_oben_old_state == false) {
      AnythingChanged = true;
      ButtonTimeLeftUP = millis();
      if (IgnoreUpdateLeftBecauseSwap)
        IgnoreUpdateLeftBecauseSwap = false;
      else if (ModifyButtonState != 2)
    {
        left_number_value = constrain(left_number_value + 1, 0, 99);
        WhatIsTheReason.ScoreChanged = true;
    }
      else {
        left_set_value = constrain(left_set_value + 1, 0, 3);
        WhatIsTheReason.SetsChanged = true;
        WhatIsTheReason.UpdateHistoryOnDisplay = true;
      }
    }
  }
  //down left
  if (b_links_unten_old_state != digitalRead(b_links_unten)) {
    b_links_unten_old_state = digitalRead(b_links_unten);
    Serial.print("b_links_unten|");
    Serial.println(b_links_unten_old_state);
    if (ButtonTimeLeftDown + 200 < millis() && b_links_unten_old_state == false) {

      AnythingChanged = true;
      ButtonTimeLeftDown = millis();
      if (ModifyButtonState != 2)
      {
        left_number_value = constrain(left_number_value - 1, 0, 99);
        WhatIsTheReason.ScoreChanged = true;
      }
      else {
        left_set_value = constrain(left_set_value - 1, 0, 3);
         WhatIsTheReason.SetsChanged = true;
        WhatIsTheReason.UpdateHistoryOnDisplay = true;
      }
    }
  }

  /*save and reset*/
  if (!b_links_unten_old_state || !b_unten_rechts_old_state)  //safety allow it only all 10 secs
  {
    ButtonTimeReset = millis();  //get current time
  }
  if (b_links_unten_old_state && b_unten_rechts_old_state)  //reset funktion - hold both lower buttons for 3 secs
  {
    if (ButtonTimeReset + 1500 < millis())  //reset after pressing all buttons 2 secs
    {
      if (right_nmber_value == 0 && left_number_value == 0) {
        right_nmber_value = 0;
        right_set_value = 0;
        left_number_value = 0;
        left_set_value = 0;
        ModifyButtonState = 0;
        AnythingChanged = true;
        ButtonTimeReset = millis() + 100000;  //set it increadible high so it wont be triggered again if buttons are not released (wait 100 secs)
        ClearAllVolleyBallHistory();
        display.ClearAndUpdateHistory();
        WhatIsTheReason.SetsChanged = true;
        WhatIsTheReason.NeedToCareForHistoryLimit = true;
        mOldScore.Points1 = -15;
        mOldScore.Points2 = -15;
        mOldScore.Set1 = -1;
        mOldScore.Set2 = -1;
        Serial.println("Hard Reset");
      } else {
        mOldScore.Points1 = -10;
        mOldScore.Points2 = -10;
        left_number_value = 0;
        right_nmber_value = 0;
        WhatIsTheReason.ScoreChanged = true;
        AnythingChanged = true;
        Serial.println("Point Reset");
        ButtonTimeReset = millis() + 100000;  //set it increadible high so it wont be triggered again if buttons are not released (wait 100 secs)
      }
    }
  }

  /*swap places fct (press both up buttons for 2 secs)
    /must tell up-buttons to not increase so points stay the same
    */
  if (b_links_oben_old_state && b_oben_rechts_old_state)  //reset funktion
  {
    if (millis() > ButtonTimeSwap + 10000)  //safety allow it only all 10 secs
    {
      ButtonTimeSwap = millis();  //get current time
    } else {
      if (ButtonTimeSwap + 2000 < millis())  //swap after pressing all buttons 3 secs
      {
        //save two values
        int mem1 = right_nmber_value;
        int mem2 = right_set_value;
        //overwrite "swap" all 4 values
        right_nmber_value = left_number_value;
        right_set_value = left_set_value;
        left_number_value = mem1;
        left_set_value = mem2;
        //this is to make it unable to increase value after releasing buttons
        IgnoreUpdateLeftBecauseSwap = true;
        IgnoreUpdateRightBecauseSwap = true;
        AnythingChanged = true;
        mOldScore.Points1 = -10;
        mOldScore.Points2 = -10;
        SwapVolleyballHistory();
        WhatIsTheReason.ScoreChanged = true;
        WhatIsTheReason.SetsChanged = true;
        WhatIsTheReason.NeedToCareForHistoryLimit = true;
        //force non update if not pressed buttons agains
        ButtonTimeSwap = millis() + 100000;  //set it increadible high so it wont be triggered again if buttons are not released (wait 100 secs)

        #ifdef SwapColor
        int zwischen = currentPalette_index_LeftNumber;
        currentPalette_index_LeftNumber = currentPalette_index_RightNumber;
        currentPalette_index_RightNumber = zwischen;
        #endif
      }
    }
  } else
    ButtonTimeSwap = 0;
  return AnythingChanged;
}

void ExecuteRemoteCode(uint16_t RemoteCode)
{
  //down right button
  if(RemoteCode & 1)
  {
        right_nmber_value = constrain(right_nmber_value - 1, 0, 99);
        WhatIsTheReason.ScoreChanged = true;
  }
    if(RemoteCode & 2)
  {
        WhatIsTheReason.SetsChanged = true;
        WhatIsTheReason.UpdateHistoryOnDisplay = true;
        right_set_value = constrain(right_set_value - 1, 0, 3);
  }
  //down left button
  if(RemoteCode & 4)
  {
        left_number_value = constrain(left_number_value - 1, 0, 99);
        WhatIsTheReason.ScoreChanged = true;
  }
  if(RemoteCode & 8)
  {
          left_set_value = constrain(left_set_value - 1, 0, 3);
         WhatIsTheReason.SetsChanged = true;
        WhatIsTheReason.UpdateHistoryOnDisplay = true;
  }
  //oben rechts
  if(RemoteCode & 16)
  {
        right_nmber_value = constrain(right_nmber_value + 1, 0, 99);
        WhatIsTheReason.ScoreChanged = true;
  }
    if(RemoteCode & 32)
  {
        WhatIsTheReason.SetsChanged = true;
        WhatIsTheReason.UpdateHistoryOnDisplay = true;
        right_set_value = constrain(right_set_value + 1, 0, 3);
  }
  //oben links
    if(RemoteCode & 64)
  {
        left_number_value = constrain(left_number_value + 1, 0, 99);
        WhatIsTheReason.ScoreChanged = true;
  }
  if(RemoteCode & 128)
  {
          left_set_value = constrain(left_set_value + 1, 0, 3);
         WhatIsTheReason.SetsChanged = true;
        WhatIsTheReason.UpdateHistoryOnDisplay = true;
  }
  //reset
  if(RemoteCode & 256)
  {
     if (right_nmber_value == 0 && left_number_value == 0) {
        right_nmber_value = 0;
        right_set_value = 0;
        left_number_value = 0;
        left_set_value = 0;
        ModifyButtonState = 0;
        ClearAllVolleyBallHistory();
        display.ClearAndUpdateHistory();
        WhatIsTheReason.SetsChanged = true;
        WhatIsTheReason.NeedToCareForHistoryLimit = true;
        mOldScore.Points1 = -15;
        mOldScore.Points2 = -15;
        mOldScore.Set1 = -1;
        mOldScore.Set2 = -1;
        Serial.println("Hard Reset");
      } else {
        mOldScore.Points1 = -10;
        mOldScore.Points2 = -10;
        left_number_value = 0;
        right_nmber_value = 0;
        WhatIsTheReason.ScoreChanged = true;
        Serial.println("Point Reset");
      }
  }
    if(RemoteCode & 512)
  {
      //save two values
        int mem1 = right_nmber_value;
        int mem2 = right_set_value;
        //overwrite "swap" all 4 values
        right_nmber_value = left_number_value;
        right_set_value = left_set_value;
        left_number_value = mem1;
        left_set_value = mem2;
        //this is to make it unable to increase value after releasing buttons
        IgnoreUpdateLeftBecauseSwap = true;
        IgnoreUpdateRightBecauseSwap = true;
        mOldScore.Points1 = -10;
        mOldScore.Points2 = -10;
        SwapVolleyballHistory();
        WhatIsTheReason.ScoreChanged = true;
        WhatIsTheReason.SetsChanged = true;
        WhatIsTheReason.NeedToCareForHistoryLimit = true;
        //force non update if not pressed buttons agains

        #ifdef SwapColor
        int zwischen = currentPalette_index_LeftNumber;
        currentPalette_index_LeftNumber = currentPalette_index_RightNumber;
        currentPalette_index_RightNumber = zwischen;
        #endif
  }
}

void loop() {
  CurrentTime = millis() - StartTime;
  //Any Button or other input event?
  /*bool UpdateNeeded = *///CheckInputState();
  if(CheckInputState())
  {
      mRadioNeedUpdateScore = true;
      Serial.println("need to update radio");
  }
  //set numbers ... but only if input changed?
  //can be messy because we have a blink animation
  /*bool NewBlinkState = (CurrentTime % 800) <= 400;

  //only update Display if we  have new input
  //0 is normal mode 
  //1 is left side color
  //2 is set mode
  //3 is right side color
  //4 Brightness
  if((ModifyButtonState > 0  && NewBlinkState != OldBlinkState) || UpdateNeeded)
  {
    SetNewNumber();
  }*/
  if ((AnythingChanged(WhatIsTheReason) || WhatIsTheReason.mUpdateForcedByRadio) && !PartyMode)  //ignore blink because display is a way slower
  {
    Serial.println("anything changed 1");
    if(AnythingChanged(WhatIsTheReason))
    {
      Serial.print(WhatIsTheReason.ModeChanged);
      Serial.print(WhatIsTheReason.ScoreChanged);
      Serial.print(WhatIsTheReason.SetsChanged);
      Serial.print(WhatIsTheReason.NeedToCareForHistoryLimit);
      Serial.print(WhatIsTheReason.RGBMAT_Update);
      Serial.print(WhatIsTheReason.mUpdateForcedByRadio);
      Serial.print(WhatIsTheReason.SetsChanged);
      Serial.print(WhatIsTheReason.UpdateHistoryOnDisplay);
    }
    else if(WhatIsTheReason.mUpdateForcedByRadio)
    {
      Serial.println("forced by radio");
    }
    if (WhatIsTheReason.ModeChanged)  //check but dont change
    {
      rgb_mat.DrawMatrixSign(ModifyButtonState);
      mOldModifyButtonState = ModifyButtonState;
      WhatIsTheReason.ModeChanged = false;
    }
    SetNewNumber(true);
    //SetNewNumber is a mix of radio and rgb mat so we can disable RGBMAT Update
    WhatIsTheReason.RGBMAT_Update = false;
    display.SetModifyButtonState(ModifyButtonState);
    if(WhatIsTheReason.ScoreChanged)
    {
      display.UpdateScore(left_number_value,right_nmber_value);
      WhatIsTheReason.ScoreChanged = false;
      SendESP_RADIOINFO();
    }
    if(WhatIsTheReason.SetsChanged || WhatIsTheReason.UpdateHistoryOnDisplay)
    {
      display.DrawOnDisplay(MyVolleyHistory,left_set_value,right_set_value,left_number_value,right_nmber_value,true,DoNotUpdateHistoryOnDisplay);
      Serial.println("draw on display done");
      WhatIsTheReason.NeedToCareForHistoryLimit = false;
      WhatIsTheReason.SetsChanged= false;
      WhatIsTheReason.UpdateHistoryOnDisplay = false;
      SendESP_RADIOINFO();
    }
    if(!WhatIsTheReason.mUpdateForcedByRadio && mRadioHistoryNeedUpdate)
    {
      mRadioHistoryNeedUpdate = true;
    }
    WhatIsTheReason.mUpdateForcedByRadio = false;

  } else if (_radio.hasData() > 0)  //we got a Cmd from CmdCenter over Radio
  {
    Serial.println("radio has data");
    PerformRadioCommand();
  } else if (mRadioNeedUpdateScore)  //Test send Radio stuff but only if we have free time /*i.e. no update is needed)
  {
    SendRadioInfo(0);
    SendRadioInfo(mESP_RADIO);
  } else if (mRadioHistoryNeedUpdate) {
    SendRadioHistoryInfo();

  }
  if (PartyMode)
    rgb_mat.DoParty();
  /*if(mForceUpdateESPRadio)
  {
    if(tries <= 1 && mRadioTime < millis())
    {
            if(SendRadioInfo(mESP_RADIO))
            {
                mForceUpdateESPRadio = false;
            }
              tries++;
    }

  }*/

}








/* this is my struct
typedef struct
  {
      int Team1;
      int Team2;
      int Satz;
      bool Team1_Won;
      bool Set = false;
  }  VolleyBallHistory;
*/
void ClearAllVolleyBallHistory() {
  for (char t = 0; t < 10; t++) {
    MyVolleyHistory[t].Set = false;
  }
}

void HandleSetFinished() {
  //can be 0
  //int SetCount = left_set_value +right_set_value;
  //shall be 1 if left side won
  // 2 if right side won
  //0 if no side won
  int Winner = 0;
  if(left_number_value > right_nmber_value)
  Winner = 1;
  else if (left_number_value < right_nmber_value)
  Winner =2;
  Serial.print("Winner is");
  Serial.println(Winner);
  int SetCounter = 0;
  for(size_t t=0; t < 10; t++)
  {
    if(MyVolleyHistory[t].Set)
      SetCounter++;
  }
  MyVolleyHistory[SetCounter] = VolleyBallHistory {left_number_value,right_nmber_value,SetCounter+1,Winner,true};
  if(Winner == 1)
  {
    left_set_value = left_set_value+1;
  }
  if(Winner == 2)
  {
    right_set_value = right_set_value+1;
  }
  else if(Winner == 0)
  {
    //better as both is 0?
    left_set_value = left_set_value+1;
    right_set_value = right_set_value+1;
  }
    //left_number_value = 0; <-> enforce hand reset
    //right_number_value = 0;
    WhatIsTheReason.SetsChanged = true;
    WhatIsTheReason.NeedToCareForHistoryLimit = true;
    WhatIsTheReason.UpdateHistoryOnDisplay = true;
}

void SwapVolleyballHistory() {
  for (int t = 0; t < 10; t++) {
    int lnv = MyVolleyHistory[t].Team1;
    MyVolleyHistory[t].Team1 = MyVolleyHistory[t].Team2;
    MyVolleyHistory[t].Team2 = lnv;
    if (MyVolleyHistory[t].Winner == 1) {
      MyVolleyHistory[t].Winner = 2;
    } else if (MyVolleyHistory[t].Winner == 2) {
      MyVolleyHistory[t].Winner = 1;
    }
  }
    WhatIsTheReason.SetsChanged = true;
    WhatIsTheReason.NeedToCareForHistoryLimit = true;
    WhatIsTheReason.UpdateHistoryOnDisplay = true;
}



bool NeedToUpdateBothNumbers(int Num, int OldNum) {
  //divide by 10 like 25 and 26
  // both remains as "2" (rounded)
  Serial.print(Num);
  Serial.print(" ");
  Serial.println(Num % 10);
  Serial.print(OldNum);
  Serial.print(" ");
  Serial.println(OldNum % 10);
  if (Num % 10 == 0 || OldNum % 10 == 0) {
    Serial.println("update both");
    return true;
  }
  Serial.println("update one");
  return false;
}

void DrehschalterDreh1() {
  //Serial.println("Dreh1");
  n = digitalRead(CLK);
  Neue_Position = digitalRead(DT);
}

bool SendRadioInfo(int TargetRadioId) {
  if (mRadioTime < millis()) {
    mRadioNeedUpdateScore = false;
    mRadioTime = millis() + 100;
    _radioScoreData.T1_Score = left_number_value;
    _radioScoreData.T2_Score = right_nmber_value;
    _radioScoreData.T1_Sets = left_set_value;
    _radioScoreData.T2_Sets = right_set_value;
    //_radioScoreData.OnTimeMillis = millis();
    Serial.print("Radio: Sending Score to");
    Serial.print(TargetRadioId);
    // By default, 'send' transmits data and waits for an acknowledgement.  If no acknowledgement is received,
    // it will try again up to 16 times.  This retry logic is built into the radio hardware itself, so it is very fast.
    // You can also perform a NO_ACK send that does not request an acknowledgement.  In this situation, the data packet
    // will only be transmitted a single time and there is no verification of delivery.  So NO_ACK sends are suited for
    // situations where performance is more important than reliability.
    //   _radio.send(DESTINATION_RADIO_ID, &_radioData, sizeof(_radioData), NRFLite::REQUIRE_ACK) // THE DEFAULT
    //   _radio.send(DESTINATION_RADIO_ID, &_radioData, sizeof(_radioData), NRFLite::NO_ACK)

    if (_radio.send(TargetRadioId, &_radioScoreData, sizeof(_radioScoreData)))  // Note how '&' must be placed in front of the variable name.
    {
      Serial.println("...Success");
      return true;
    } else {
      Serial.println("...Failed");
      _radioScoreData.FailedTxCount++;
      return false;
    }
  }
  return false;
}

void SendRadioHistoryInfo() {
  Serial.print("Radio: Sending History ");
  VolleyBallHistory RadioVolleyBallHistory[5];
  if (mRadioTime < millis()) {
    mRadioHistoryNeedUpdate = false;
    mRadioTime = millis() + 100;
    int mCurrentCounter = 0;
    for (size_t t = 0; t < 10; t++) {
      if (MyVolleyHistory[t].Set == false)
        continue;
      _radioHistoryData.myVolleyBallHistory[mCurrentCounter].Satz = MyVolleyHistory[t].Satz;
      _radioHistoryData.myVolleyBallHistory[mCurrentCounter].Team1 = MyVolleyHistory[t].Team1;
      _radioHistoryData.myVolleyBallHistory[mCurrentCounter].Team2 = MyVolleyHistory[t].Team2;
      _radioHistoryData.myVolleyBallHistory[mCurrentCounter].Set = true;
      _radioHistoryData.myVolleyBallHistory[mCurrentCounter].Winner = MyVolleyHistory[t].Winner;
      mCurrentCounter++;
      if (mCurrentCounter >= 5)  //max 5 items- 0...4
        break;
    }
    for (size_t t = mCurrentCounter; t < 5; t++) {
      _radioHistoryData.myVolleyBallHistory[t].Set = false;
      mCurrentCounter++;
    }
    //_radioScoreData.OnTimeMillis = millis();

    Serial.print("to ");
    Serial.print(DESTINATION_RADIO_ID);
    if (_radio.send(DESTINATION_RADIO_ID, &_radioHistoryData, sizeof(_radioHistoryData)))  // Note how '&' must be placed in front of the variable name.
    {
      Serial.println(" ...Success");
    } else {
      Serial.println(" ...Failed");
      _radioHistoryData.FailedTxCount++;
    }
  }
}


void PerformRadioCommand() {
    uint8_t packetSize = _radio.hasData();
  if (packetSize == sizeof(Radio_Remote_Commandpackage)) {
     _radio.readData(&mRemoteCommandPackage);
     Serial.println("Remote package recieved");
    OnRecieve_REMOTE_CONTROL_DATA();
    return;
  }
  
  else if(packetSize== sizeof(Commandpackage))
  {
    _radio.readData(&_radioCommandData);
    //go on in this function
     ReadCommandData();
  }
  else
  {
    //discard data?
    _radio.discardData(packetSize);
    return;
  }
}

void ReadCommandData()
{
  Serial.print("Cmd arrived: ");
  Serial.println(_radioCommandData.CommandId);
  if (_radioCommandData.CommandId == 0)  //we request an update on History and Score on Radio Device
  {
    mRadioNeedUpdateScore = true;
    mRadioHistoryNeedUpdate = true;
    return;  //do it next tick - when we are idel
  } else if (_radioCommandData.CommandId == 1) {
    left_number_value = _radioCommandData.CommandArg1;
    right_nmber_value = _radioCommandData.CommandArg2;
    left_set_value = _radioCommandData.CommandArg3;
    right_set_value = _radioCommandData.CommandArg4;
    mNewScore.Points1 = left_number_value;
    mNewScore.Points2 = right_nmber_value;
    mNewScore.Set1 = left_set_value;
    mNewScore.Set1 = right_set_value;
    Serial.println(left_number_value);
    Serial.println(right_nmber_value);
    Serial.println(left_set_value);
    Serial.println(right_set_value);
    WhatIsTheReason.mUpdateForcedByRadio = true;
    //clever=
    if (mNewScore.Points1 != mOldScore.Points1 || mNewScore.Points2 != mOldScore.Points2)
      display.UpdateScore(left_number_value,right_nmber_value);
    if (mNewScore.Set1 != mOldScore.Set1 || mNewScore.Set2 != mOldScore.Set2)
      display.UpdateSetCounter(left_set_value,right_set_value);
    //do this after the other tasks because it sets new values for old and new score
    SetNewNumber(false);

  } else if (_radioCommandData.CommandId == 2) {
    WhatIsTheReason.mUpdateForcedByRadio = true;
    if (_radioCommandData.CommandArg1 == 1)  //Soft Reset aka reset points
    {
      right_nmber_value = 0;
      right_set_value = 0;
      left_number_value = 0;
      left_set_value = 0;
      ModifyButtonState = 0;
      ButtonTimeReset = millis() + 100000;  //set it increadible high so it wont be triggered again if buttons are not released (wait 100 secs)
      ClearAllVolleyBallHistory();
      WhatIsTheReason.SetsChanged = true;
      WhatIsTheReason.NeedToCareForHistoryLimit = true;
      WhatIsTheReason.UpdateHistoryOnDisplay = true;
      mOldScore.Points1 = -15;
      mOldScore.Points2 = -15;
      mOldScore.Set1 = -1;
      mOldScore.Set2 = -1;
      Serial.println("Hard Reset requested by Radio");
    } else {
      mOldScore.Points1 = -10;
      mOldScore.Points2 = -10;
      left_number_value = 0;
      right_nmber_value = 0;
      Serial.println("Point Reset requested by Radio");
      ButtonTimeReset = millis() + 100000;  //set it increadible high so it wont be triggered again if buttons are not released (wait 100 secs)
    }
  } else if (_radioCommandData.CommandId == 3)  //set history
  {
    uint8_t HistoryEntry = _radioCommandData.CommandArg1;
    uint8_t SetNum = _radioCommandData.CommandArg2;  //200+SetNum -> set is not counted <-> or 0-5???? (9 is unsupported)
    uint8_t T1_NewPoints = constrain(_radioCommandData.CommandArg3, 0, 99);
    uint8_t T2_NewPoints = constrain(_radioCommandData.CommandArg4, 0, 99);
    bool Counted = true;
    //MyVolleyHistory
    if (SetNum > 200) {
      Counted = false;
      SetNum = SetNum - 200;
    }
    if (SetNum > 9) {
      Serial.println("CMD3 SetNum is too high");
      return;
    }
    if (T1_NewPoints > T2_NewPoints) {
      MyVolleyHistory[SetNum].Winner = 1;
    } else if (T1_NewPoints < T2_NewPoints) {
      MyVolleyHistory[SetNum].Winner = 2;
    } else if (T1_NewPoints == T2_NewPoints) {
      MyVolleyHistory[SetNum].Winner = 0;
    }
    MyVolleyHistory[SetNum].Set = Counted;
    MyVolleyHistory[SetNum].Team1 = T1_NewPoints;
    MyVolleyHistory[SetNum].Team2 = T2_NewPoints;
    int WinT1 = 0;
    int WinT2 = 0;
    for (size_t t = 0; t < 10; t++) {
      if (MyVolleyHistory[t].Set) {
        if (MyVolleyHistory[t].Winner <= 1)
          WinT1++;
        if (MyVolleyHistory[t].Winner != 1)
          WinT2++;
      }
    }
    //Now Update
    //do not inform radio as data came from Radio
    if (left_set_value == WinT1 && right_set_value == WinT2) {
      display.UpdateHistory(MyVolleyHistory);
    } else {
      left_set_value = WinT1;
      right_set_value = WinT2;
      display.UpdateHistory(MyVolleyHistory);
      display.UpdateSetCounter(left_set_value,right_set_value);
      SetNewNumber(false);
    }


  } else if (_radioCommandData.CommandId == 4) {

    PartyMode = !PartyMode;
    Serial.print("PartyMode");
    Serial.println(PartyMode);
    if (PartyMode == false) {
      mOldScore.Points1 = -15;
      mOldScore.Points2 = -15;
      mOldScore.Set1 = -1;
      mOldScore.Set2 = -1;
      WhatIsTheReason.mUpdateForcedByRadio = true;
      rgb_mat.FadeBlack();
    }
  }
}

void OnRecieve_REMOTE_CONTROL_DATA()
{
  //mRemoteCommandPackage
  if(mRemoteCommandPackage.FromRadioId != mESP_RADIO)
  return;
  Serial.println("recieved package from our remote");
  switch (mRemoteCommandPackage.CommandId)
  {
    case Radio_Remote_Commands::Connect:
    {
      //just send score back
      Serial.print("Connect (send radio info)");
      WhatIsTheReason.ScoreChanged = true;
      WhatIsTheReason.mUpdateForcedByRadio = true;
      SendESP_RADIOINFO();
      return;
    }
    case Radio_Remote_Commands::PlusT1:
    {
      right_nmber_value = constrain(right_nmber_value+1,0,99);
      
      WhatIsTheReason.ScoreChanged = true;
      WhatIsTheReason.mUpdateForcedByRadio = true;
      Serial.print("PlusT1 (send radio info)");
      SendESP_RADIOINFO();
      return;
    }
    case Radio_Remote_Commands::MinusT1:
    {
      right_nmber_value = constrain(right_nmber_value-1,0,99);
      
      WhatIsTheReason.ScoreChanged = true;
      WhatIsTheReason.mUpdateForcedByRadio = true;
      SendESP_RADIOINFO();
      Serial.print("MinusT1 (send radio info)");
      return;
    }
        case Radio_Remote_Commands::PlusT2:
    {
      left_number_value = constrain(left_number_value+1,0,99);
      WhatIsTheReason.ScoreChanged = true;
      WhatIsTheReason.mUpdateForcedByRadio = true;
      SendESP_RADIOINFO();
      Serial.print("PlusT2 (send radio info)");
      return;
    }
    case Radio_Remote_Commands::MinusT2:
    {
      left_number_value = constrain(left_number_value-1,0,99);
      WhatIsTheReason.ScoreChanged = true;
      WhatIsTheReason.mUpdateForcedByRadio = true;
      SendESP_RADIOINFO();
      Serial.print("MinusT2 (send radio info)");
      return;
    }
    case Radio_Remote_Commands::Swap:
    {
      //save two values
        int mem1 = right_nmber_value;
        int mem2 = right_set_value;
        //overwrite "swap" all 4 values
        right_nmber_value = left_number_value;
        right_set_value = left_set_value;
        left_number_value = mem1;
        left_set_value = mem2;
        //this is to make it unable to increase value after releasing buttons
        IgnoreUpdateLeftBecauseSwap = true;
        IgnoreUpdateRightBecauseSwap = true;
        mOldScore.Points1 = -10;
        mOldScore.Points2 = -10;
        WhatIsTheReason.ScoreChanged = true;
        WhatIsTheReason.SetsChanged = true;
        WhatIsTheReason.NeedToCareForHistoryLimit = true;
        SendESP_RADIOINFO();
        //force non update if not pressed buttons agains
        #ifdef SwapColor
        int zwischen = currentPalette_index_LeftNumber;
        currentPalette_index_LeftNumber = currentPalette_index_RightNumber;
        currentPalette_index_RightNumber = zwischen;
        #endif
      return;
    }
    case Radio_Remote_Commands::Reset:
    {
        if (right_nmber_value == 0 && left_number_value == 0) {
        right_nmber_value = 0;
        right_set_value = 0;
        left_number_value = 0;
        left_set_value = 0;
        ModifyButtonState = 0;
        ClearAllVolleyBallHistory();
        display.ClearAndUpdateHistory();
        WhatIsTheReason.SetsChanged = true;
        WhatIsTheReason.NeedToCareForHistoryLimit = true;
        mOldScore.Points1 = -15;
        mOldScore.Points2 = -15;
        mOldScore.Set1 = -1;
        mOldScore.Set2 = -1;
        Serial.println("Hard Reset");
      } else {
        mOldScore.Points1 = -10;
        mOldScore.Points2 = -10;
        left_number_value = 0;
        right_nmber_value = 0;
        WhatIsTheReason.ScoreChanged = true;
        Serial.println("Point Reset");
      }
      return;
    }
    case Radio_Remote_Commands::SetFinished:
    {
      HandleSetFinished();
      return;
    }

  }
}

void SendESP_RADIOINFO()
{
        mForceUpdateESPRadio = true;
        tries = 0;
}
