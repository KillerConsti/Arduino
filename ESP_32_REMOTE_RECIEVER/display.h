
#include <Globals.h>
#include <SPI.h>
#include <TFT_eSPI.h>  // Hardware-specific library
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite spr = TFT_eSprite(&tft);  // Sprite class needs to be invoked
#define TFT_ROT 0


const uint16_t KC_grey = 2113;
const uint16_t KC_dirty_white = 61309;
const uint16_t KC_DARKGREY = 31727;

#include "NotoSansBold15.h"
#include "NotoSansBold36.h"
//#include "NotoSansMonoSCB20.h"
// The font names are arrays references, thus must NOT be in quotes ""
#define AA_FONT_SMALL NotoSansBold15
#define AA_FONT_LARGE NotoSansBold36
//#define AA_FONT_MONO NotoSansMonoSCB20  // NotoSansMono-SemiCondensedBold 20pt
const uint8_t xpos_volleyball = 15;
const uint8_t ypos_volleyball = 115;
const uint8_t xpos_fox = 170;
const uint8_t ypos_fox = 115;

//Special CS Pins for display
//if we set them high <-> we ignore date
//if we set them low <-> we listen toi data

const uint8_t CS_PIN_Display1 = 25;
const uint8_t CS_PIN_Display2 = 33;
class MyDisplay
{
   public :
 MyDisplay();
void initDevice();
void DrawVolleyball();
void DrawFox();
void UpdateHistory(VolleyBallHistory TheArray[], int n, int sender) ;
void SetNewScore(int T1, int T2, int S1, int S2,int sender);
void UpdateScreenColor();
int GetCSPin(int sender);
};

MyDisplay::MyDisplay()
{}

void MyDisplay::DrawVolleyball() {
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


void MyDisplay::DrawFox() {
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

void MyDisplay::UpdateScreenColor()
{
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
void MyDisplay::initDevice()
{
    pinMode(CS_PIN_Display1,OUTPUT);              // The CS pins will be outputs
  pinMode(CS_PIN_Display2,OUTPUT);
  // set chip selects to low so data goes to both screens at once
  digitalWrite(CS_PIN_Display1,0);
  digitalWrite(CS_PIN_Display2,0);
	//https://www.xtronical.com/multiplescreens
   tft.begin();
   tft.setRotation(TFT_ROT);
    tft.fillScreen(KC_grey);
  //tft.fillRect(0, 90, 240, 240, dirty_white);
  tft.fillCircle(60, 180, 90, KC_DARKGREY);
  tft.fillCircle(180, 180, 90, KC_DARKGREY);
  spr.setColorDepth(16);  // 16 bit colour needed to show antialiased fonts

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

digitalWrite(CS_PIN_Display2,HIGH);
  tft.fillCircle(60, 90, 25, mRadioScores[0].BackgroundColor);
  tft.fillCircle(180, 90, 25, mRadioScores[0].BackgroundColor);
  //tft.drawRect(60, 85, 100, 30, TFT_RED);
  tft.fillRect(60, 65, 120, 51, mRadioScores[0].BackgroundColor);

    digitalWrite(CS_PIN_Display1,HIGH);
digitalWrite(CS_PIN_Display2,LOW);
      tft.fillCircle(60, 90, 25, mRadioScores[1].BackgroundColor);
  tft.fillCircle(180, 90, 25, mRadioScores[1].BackgroundColor);
  //tft.drawRect(60, 85, 100, 30, TFT_RED);
  tft.fillRect(60, 65, 120, 51, mRadioScores[1].BackgroundColor);
  digitalWrite(CS_PIN_Display2,HIGH);
}

int MyDisplay::GetCSPin(int sender)
{
   switch(sender)
   {
      case 0:
      return CS_PIN_Display1;
      default: 
      return CS_PIN_Display2;
   }
}
void MyDisplay::UpdateHistory(VolleyBallHistory TheArray[], int n, int sender) 
{
   int CSPin = GetCSPin(sender);
   digitalWrite(CSPin,LOW);
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
digitalWrite(CSPin,HIGH);
}

void MyDisplay::SetNewScore(int T1, int T2, int S1, int S2,int sender) {
      int CSPin = GetCSPin(sender);
   digitalWrite(CSPin,LOW);
 int xpos = tft.width() / 2;  // Half the screen width
  int ypos = 50;
  delay(20);
  Serial.print("set score for sender ");
  Serial.println(sender);
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
  spr.fillSprite(mRadioScores[sender].BackgroundColor);
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
  digitalWrite(CSPin,HIGH);
}