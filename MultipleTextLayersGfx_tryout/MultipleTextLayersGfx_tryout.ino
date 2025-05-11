/*
  This example shows how multiple scrolling text layers can be used
  This example requires the Adafruit_GFX Library, which you can install through Arduino Library Manager
  The "GFX" Layers in SmartMatrix Library 4.0 depend on Adafruit_GFX and give access to using large fonts included
    with Adafruit_GFX, and the ability to add custom fonts
  Performance of the "GFX" layers has been optimized, and with the Teensy 4, you can have 10 scrolling text layers
    on a 128x64 panel, and still refresh the panel at 240Hz
  Optionally draw a pattern behind the text layers using FastLED (same code as the FastLED_Functions example) by
    removing the comments from `#define INCLUDE_FASTLED_BACKGROUND` below.  With the Teensy 4 you'll still see 
    128x64 @ 240Hz Refresh even with the pattern updating at 60 FPS!
*/


//2 structs

//1. What we send
struct __attribute__((packed)) Commandpackage  // Any packet up to 32 bytes can be sent.
{
  uint8_t CommandId = 0;
  uint8_t CommandArg1 = 0;
  uint8_t CommandArg2 = 0;
  uint8_t CommandArg3 = 0;
  uint8_t CommandArg4 = 0;
};



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



#include "NRFLite.h"
NRFLite _radio;
Commandpackage _radioCmdData;
MatchDataLong _radio_matchDataLong;
MatchDataShort _radio_matchDataShort;
struct RadioChangeEvent
{
  bool BigDataChange =false;
  bool LittleDataChange = false;
  int MatchNum = 0;
};

struct Match
{
  int T1_point;
  int T2_point;
  int T1_sets;
  int T2_sets;
  char Teamname1[12];
  char Teamname2[12];
};
Match mMatch1;
Match mMatch2;
Match mMatch3;
RadioChangeEvent mRadioChangeEvent;


const static uint8_t RADIO_ID = 1;
const static uint8_t DESTINATION_RADIO_ID = 0;
const static uint8_t PIN_RADIO_CE = 19; // 9 or 10 depending on test condition
const static uint8_t PIN_RADIO_CSN = 20;
const static uint8_t PIN_RADIO_IRQ = 21;


#define USE_ADAFRUIT_GFX_LAYERS

//#define INCLUDE_FASTLED_BACKGROUND

// uncomment one line to select your MatrixHardware configuration - configuration header needs to be included before <SmartMatrix.h>
//#include <MatrixHardware_Teensy3_ShieldV4.h>        // SmartLED Shield for Teensy 3 (V4)
#include <MatrixHardware_Teensy4_ShieldV5.h>        // SmartLED Shield for Teensy 4 (V5)
//#include <MatrixHardware_Teensy3_ShieldV1toV3.h>    // SmartMatrix Shield for Teensy 3 V1-V3
//#include <MatrixHardware_Teensy4_ShieldV4Adapter.h> // Teensy 4 Adapter attached to SmartLED Shield for Teensy 3 (V4)
//#include <MatrixHardware_ESP32_V0.h>                // This file contains multiple ESP32 hardware configurations, edit the file to define GPIOPINOUT (or add #define GPIOPINOUT with a hardcoded number before this #include)
//#include "MatrixHardware_Custom.h"                  // Copy an existing MatrixHardware file to your Sketch directory, rename, customize, and you can include it like this
#include <SmartMatrix.h>

#include <Fonts/Picopixel.h>
#include <Fonts/TomThumb.h>
#include <Fonts/FreeMono12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBoldOblique24pt7b.h>
#include <Fonts/FreeSerif24pt7b.h>
#include <Fonts/Org_01.h>
#include <Fonts/Tiny3x3a2pt7b.h>
#include <Fonts/FreeSerifItalic9pt7b.h>
#include <Fonts/FreeSansOblique24pt7b.h>
#include <Fonts/FreeSerifBold9pt7b.h>
#include <Fonts/Open_Sans_ExtraBold_10.h>
#include <Fonts/FreeMonoBold7pt7b.h>
#define COLOR_DEPTH 24                  // Choose the color depth used for storing pixels in the layers: 24 or 48 (24 is good for most sketches - If the sketch uses type `rgb24` directly, COLOR_DEPTH must be 24)
const uint16_t kMatrixWidth = 128;       // Set to the width of your display, must be a multiple of 8
const uint16_t kMatrixHeight = 64;      // Set to the height of your display
const uint8_t kRefreshDepth = 36;       // Tradeoff of color quality vs refresh rate, max brightness, and RAM usage.  36 is typically good, drop down to 24 if you need to.  On Teensy, multiples of 3, up to 48: 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 42, 45, 48.  On ESP32: 24, 36, 48
const uint8_t kDmaBufferRows = 4;       // known working: 2-4, use 2 to save RAM, more to keep from dropping frames and automatically lowering refresh rate.  (This isn't used on ESP32, leave as default)
const uint8_t kPanelType = SM_PANELTYPE_HUB75_32ROW_MOD16SCAN;   // Choose the configuration that matches your panels.  See more details in MatrixCommonHub75.h and the docs: https://github.com/pixelmatix/SmartMatrix/wiki
const uint32_t kMatrixOptions = (SM_HUB75_OPTIONS_NONE);        // see docs for options: https://github.com/pixelmatix/SmartMatrix/wiki
const uint8_t kScrollingLayerOptions = (SM_SCROLLING_OPTIONS_NONE);
int i = 0;
SMARTMATRIX_ALLOCATE_BUFFERS(matrix, kMatrixWidth, kMatrixHeight, kRefreshDepth, kDmaBufferRows, kPanelType, kMatrixOptions);

#ifdef INCLUDE_FASTLED_BACKGROUND
  const uint8_t kBackgroundLayerOptions = (SM_BACKGROUND_GFX_OPTIONS_NONE);
  SMARTMATRIX_ALLOCATE_BACKGROUND_GFX_LAYER(backgroundLayer, kMatrixWidth, kMatrixHeight, COLOR_DEPTH, kBackgroundLayerOptions);
#endif

// For larger fonts, you need to allocate enough memory to hold the entire scrolling text bitmap (the bounds of all the characters in the string) in memory
//   and this may be larger than kMatrixWidth*kMatrixHeight.  Use the SMARTMATRIX_ALLOCATE_GFX_MONO_LAYER macro, which allows for setting the character bitmap
//   to different size than the matrix bounds.  We allocate several times the default amount of memory here:
// Basing the bitmap allocation on the matrix size doesn't make much sense, as the size of the scrolling text is mostly independent of matrix size
//   Here we allocate an amount of memory that is known to work as a 6kb wide bitmap, with height of 1.  The layer will resize the bitmap to fit the text
SMARTMATRIX_ALLOCATE_GFX_MONO_LAYER(scrollingLayer01, kMatrixWidth, kMatrixHeight, 6*1024, 1, COLOR_DEPTH, kScrollingLayerOptions);

// The required sizes for some of the layers are printed out to serial, so these layers can be sized exactly to the needed bitmap dimensions
SMARTMATRIX_ALLOCATE_GFX_MONO_LAYER(scrollingLayer02, kMatrixWidth, kMatrixHeight, 168, 48, COLOR_DEPTH, kScrollingLayerOptions);
//SMARTMATRIX_ALLOCATE_GFX_MONO_LAYER(scrollingLayer03, kMatrixWidth, kMatrixHeight, 224, 40, COLOR_DEPTH, kScrollingLayerOptions);
const uint8_t kIndexedLayerOptions = (SM_INDEXED_OPTIONS_NONE);
SMARTMATRIX_ALLOCATE_INDEXED_LAYER(scrollingLayer03, kMatrixWidth, kMatrixHeight, COLOR_DEPTH, kIndexedLayerOptions);
SMARTMATRIX_ALLOCATE_INDEXED_LAYER(points_match1, kMatrixWidth, kMatrixHeight, COLOR_DEPTH, kIndexedLayerOptions);
SMARTMATRIX_ALLOCATE_INDEXED_LAYER(backgroundLayer, kMatrixWidth, kMatrixHeight, COLOR_DEPTH, kIndexedLayerOptions);

SMARTMATRIX_ALLOCATE_INDEXED_LAYER(sets_match1_layer, kMatrixWidth, kMatrixHeight, COLOR_DEPTH, kIndexedLayerOptions);

SMARTMATRIX_ALLOCATE_INDEXED_LAYER(teams_match2, kMatrixWidth, kMatrixHeight, COLOR_DEPTH, kIndexedLayerOptions);
SMARTMATRIX_ALLOCATE_INDEXED_LAYER(points_match2, kMatrixWidth, kMatrixHeight, COLOR_DEPTH, kIndexedLayerOptions);
SMARTMATRIX_ALLOCATE_INDEXED_LAYER(sets_match2, kMatrixWidth, kMatrixHeight, COLOR_DEPTH, kIndexedLayerOptions);
const int randomSpeedMax = 120;
const int randomSpeedMin = 20;

// select random offset, from range of what's visible on screen (0 - matrixHeight-layerHeight)
void chooseRandomVisibleOffset(SMLayerGFXMono<rgb24, rgb1, 0> *layer) {
  // first get the total number of pixels on the screen in the text's vertical direction
  int16_t maxHeight = (layer->getLayerRotation() % 2) ? kMatrixWidth : kMatrixHeight;

  // subtract the height of the text
  maxHeight -= layer->getLocalHeight();

  // get random number within range 0..maxHeight
  if(maxHeight > 0)
    maxHeight = random(maxHeight);
  else
    maxHeight = 0;

  // set offset that keeps the text visible
  layer->setOffsetFromTop(maxHeight);
}

rgb24 chooseRandomBrightColor() {
  rgb24 color;

  switch(random(3)) {
    case 0:
      color = rgb24(random(0x80, 0x100), random(0x40), random(0x40));
      break;

    case 1:
      color = rgb24(random(0x40), random(0x80, 0x100), random(0x40));
      break;

    case 2:
      color = rgb24(random(0x40), random(0x40), random(0x80, 0x100));
    default:
      break;
  }
  return color;
}

#ifdef INCLUDE_FASTLED_BACKGROUND
#include <FastLED.h>

// The 32bit version of our coordinates
static uint16_t x;
static uint16_t y;
static uint16_t z;

// We're using the x/y dimensions to map to the x/y pixels on the matrix.  We'll
// use the z-axis for "time".  speed determines how fast time moves forward.  Try
// 1 for a very slow moving effect, or 60 for something that ends up looking like
// water.
// uint16_t speed = 1; // almost looks like a painting, moves very slowly
uint16_t speed = 10; // a nice starting speed, mixes well with a scale of 100
// uint16_t speed = 33;
// uint16_t speed = 100; // wicked fast!

// Scale determines how far apart the pixels in our noise matrix are.  Try
// changing these values around to see how it affects the motion of the display.  The
// higher the value of scale, the more "zoomed out" the noise iwll be.  A value
// of 1 will be so zoomed in, you'll mostly see solid colors.

// uint16_t scale = 1; // mostly just solid colors
// uint16_t scale = 4011; // very zoomed out and shimmery
uint16_t scale = 31;

// This is the array that we keep our computed noise values in
uint8_t noise[kMatrixWidth][kMatrixHeight];

// Fill the x/y array of 8-bit noise values using the inoise8 function.
void fillnoise8() {
  for(int i = 0; i < kMatrixWidth; i++) {
    int ioffset = scale * i;
    for(int j = 0; j < kMatrixHeight; j++) {
      int joffset = scale * j;
      noise[i][j] = inoise8(x + ioffset,y + joffset,z);
    }
  }
  z += speed;
}

#endif

void setup() {
  Serial.begin(115200);
  Serial.print(static_cast<uint8_t>('ä'));
  delay(500);

  randomSeed(analogRead(0));
#ifdef INCLUDE_FASTLED_BACKGROUND
  matrix.addLayer(&backgroundLayer); 
  
  // Initialize our coordinates to some random values
  x = random16();
  y = random16();
  z = random16();
#endif

matrix.addLayer(&backgroundLayer);
  matrix.addLayer(&points_match1); 
  //matrix.addLayer(&scrollingLayer01); 
  //matrix.addLayer(&scrollingLayer02); 
  matrix.addLayer(&scrollingLayer03); 
  matrix.addLayer(&sets_match1_layer);

matrix.addLayer(&teams_match2);
matrix.addLayer(&points_match2);
matrix.addLayer(&sets_match2);
  matrix.begin();

  points_match1.setColor(rgb24(0, 255, 0));
  scrollingLayer01.setColor(chooseRandomBrightColor());
  scrollingLayer02.setColor(chooseRandomBrightColor());
  scrollingLayer03.setColor(rgb24(0, 40, 255));

  points_match1.setSpeed(0);
  scrollingLayer01.setSpeed(0);
  scrollingLayer02.setSpeed(0);
  scrollingLayer01.setFont(&FreeMonoBold18pt7b);
  scrollingLayer02.setFont(&FreeSerif24pt7b);



  //scrollingLayer03.setFont(&FreeMonoBold7pt7b);
  //scrollingLayer03.setTextSize();
        scrollingLayer03.fillScreen(0);
        //scrollingLayer03.setCursor(30, 50);
        //scrollingLayer03.println("00 : 25");
        scrollingLayer03.setCursor(2, 2);
        scrollingLayer03.println("Beachfreunde");
        scrollingLayer03.setCursor(22, 11);
        scrollingLayer03.println("vs.");
        scrollingLayer03.setCursor(2, 22);
        scrollingLayer03.println("Team 2");
       scrollingLayer03.swapBuffers(false);

       points_match1.setFont(&FreeSans10pt7b);
       points_match1.setCursor(75, 15);
       points_match1.println("00:25");
      points_match1.swapBuffers(false);
       backgroundLayer.setColor(rgb24(0, 180, 180));
      backgroundLayer.drawRect(0, 0, 128,32, 0xFFE0);
      backgroundLayer.drawRect(0, 32, 128,32, 0xFFE0);
      backgroundLayer.swapBuffers(false);


        //sets_match1_layer.setFont(&FreeSans10pt7b);
        sets_match1_layer.setColor(rgb24(0, 255, 0));
       sets_match1_layer.setCursor(86, 21);
       sets_match1_layer.println("(0:1)");
      sets_match1_layer.swapBuffers(false);

      //match 2
      //teams_match2
      teams_match2.setColor(rgb24(0, 40, 255));
          teams_match2.setCursor(2, 32+2);
        teams_match2.println("Ebersbrunn");
        teams_match2.setCursor(22, 32+11);
        teams_match2.println("vs.");
        teams_match2.setCursor(2, 32+22);
        teams_match2.print("Volleyb");
        //ä  0x84
        //ö 0x94
        //ü 0x81 
        teams_match2.write(0x84);   // Print the u-with-umlauts
        teams_match2.print("ren");
       teams_match2.swapBuffers(false);
       teams_match2.writePixel(1,1,0xffff);

      points_match2.setColor(rgb24(0, 255, 0));
              points_match2.setFont(&FreeSans10pt7b);
       points_match2.setCursor(75, 32+15);
       points_match2.println("12:09");
      points_match2.swapBuffers(false);

              sets_match2.setColor(rgb24(0, 255, 0));
       sets_match2.setCursor(86, 21+32);
       sets_match2.println("(0:1)");
      sets_match2.swapBuffers(false);

       //init radio
   if (!_radio.init(8, 20, 19)) {
    Serial.println("Cannot communicate with radio");
    //while (1)
      ;  // Wait here forever.
  }
  else
  {
    Serial.println("radio init succesfull");
        delay(50);
}
}

bool RecievedMatchDataLong()
{
      _radio.readData(&_radio_matchDataLong);
    uint8_t matchtype = _radio_matchDataLong.type;
    Serial.println(matchtype);
    Match* MyMatch = &mMatch1;
    if(matchtype == 2)
    {
      MyMatch = &mMatch2;
    }
    else if (matchtype == 3)
    {
      MyMatch = &mMatch3;
    }
    else if(matchtype < 0)return true; //Handle 4-6
    if(matchtype < 4)
    {
      //mRadioChangeEvent.BigDataChange = true;
      ChangeTeamNames(matchtype);
    }
    return true;
}

bool NewRadioEvent()
{
  uint8_t packetSize = _radio.hasData();
  if(packetSize == 0)
  return false;
   Serial.println(packetSize);
  if (packetSize == sizeof(MatchDataLong))
  {
    Serial.println("Radio Msg Long recieved");
     return RecievedMatchDataLong();
  }
  else if (packetSize == sizeof(MatchDataShort))
  {
    Serial.println("Radio Msg short recieved");
     return RecievedMatchDataShort();
  }
  else if (packetSize == sizeof(_radioCmdData))
  {
    return HandleCMDData();
  }
  _radio.discardData(packetSize);
    return false; 
}

bool HandleCMDData()
{
      _radio.readData(&_radioCmdData);
    uint8_t id = _radioCmdData.CommandId;
    uint8_t arg = _radioCmdData.CommandArg1;
    if(id != 0)
    return false;
    switch (arg)
    {
      case 0: //Beachfreunde Scrren
      {
        Serial.print("Show Beachfreunde screen");
        return true;
      }
      case 1:
      {
        Serial.print("Show Scores");
        return true;
      }
      default:
      {
        Serial.print("Toggle between screens");
        return true; 
      }
    }

}



bool RecievedMatchDataShort()
{

  _radio.readData(&_radio_matchDataShort);
      uint8_t sender = _radio_matchDataShort.FromRadioId;
    if(sender != 0)
    return false;
    Match* MyMatch = &mMatch1;
    if(_radio_matchDataShort.MatchNum == 2)
    {
      MyMatch = &mMatch2;
    }
    else if (_radio_matchDataShort.MatchNum == 3)
    {
      MyMatch = &mMatch3;
    }
    Serial.print("MatchNum");
    Serial.println(_radio_matchDataShort.MatchNum);
    MyMatch->T1_point = _radio_matchDataShort.T1_Score;
    MyMatch->T2_point = _radio_matchDataShort.T2_Score;
    MyMatch->T1_sets = _radio_matchDataShort.T1_Sets;
    MyMatch->T2_sets = _radio_matchDataShort.T2_Sets;
    mRadioChangeEvent.LittleDataChange = true;
    mRadioChangeEvent.MatchNum = _radio_matchDataShort.MatchNum;
    return true;
}
void loop() {
  if(NewRadioEvent())
  {
  }
  if(mRadioChangeEvent.LittleDataChange)
  {
      Serial.println("we recieved little data change");
      PerformLittleDataChange();
      mRadioChangeEvent.LittleDataChange = false;
  }
  else if (mRadioChangeEvent.BigDataChange)
  {
    //Serial.println("we recieved big data change");
      //PerformBigDataChange();
      //mRadioChangeEvent.BigDataChange = false;
  }
  delay(20);
}

void PerformLittleDataChange()
{
//set correct layers to manipulate
    if(mRadioChangeEvent.MatchNum == 1)
    {
       uint8_t i = mMatch1.T1_point;
       uint8_t j = mMatch1.T2_point;
       uint8_t x = mMatch1.T1_sets;
       uint8_t y = mMatch1.T2_sets;

      points_match1.fillScreen(0);
       points_match1.setCursor(75, 15);
        if(i > 99)
        {
          i = 0;
        }
        if(i < 10)
        {
          points_match1.print(0);
        }
        points_match1.print(i);
        points_match1.print(":");
        if(j > 99)
        {
          j = 0;
        }
        if(j < 10)
        {
          points_match1.print(0);
        }
        points_match1.print(j);
        points_match1.swapBuffers(false);
    
  }
  else if (mRadioChangeEvent.MatchNum == 2)
  {
      points_match2.fillScreen(0);
      points_match2.setCursor(75, 32+15);
       uint8_t i = mMatch2.T1_point;
       uint8_t j = mMatch2.T2_point;
       uint8_t x = mMatch2.T1_sets;
       uint8_t y = mMatch2.T2_sets;

        if(i > 99)
        {
          i = 0;
        }
        if(i < 10)
        {
          points_match2.print(0);
        }
        points_match2.print(i);
        points_match2.print(":");
        if(j > 99)
        {
          j = 0;
        }
        if(j < 10)
        {
          points_match2.print(0);
        }
        points_match2.print(j);
        points_match2.swapBuffers(false);
        sets_match2.fillScreen(0);
        sets_match2.setCursor(86, 21+32);
        sets_match2.print("(");
        sets_match2.print(x);
        sets_match2.print(":");
        sets_match2.print(y);
        sets_match2.print(")");
        sets_match2.swapBuffers(false);
}
}
void ChangeTeamNames(int MatchNum)
{
    Serial.print("Change Team NAmes");
  mRadioChangeEvent.MatchNum = MatchNum;
  Serial.println("Matchnum ");
  
  int y_Offset = (MatchNum-1) *32;
  Serial.println(y_Offset);
  SMLayerGFXMono<rgb24,rgb1,kIndexedLayerOptions>* MyLayerTeams= &scrollingLayer03;
  Match* MyMatch = &mMatch1;
  if(mRadioChangeEvent.MatchNum == 2)
  {
    MyLayerTeams = &teams_match2;
    MyMatch = &mMatch2;
  }
  MyLayerTeams->fillScreen(0);
        MyLayerTeams->setCursor(2, y_Offset+2);
        Serial.print("Teamname 1 ");
        for(size_t t=0; t < 12; t++)
        {
          char currentchar = _radio_matchDataLong.TeamName1[t];
          MyMatch->Teamname1[t] = currentchar;
          Serial.print(currentchar);
          Serial.println(static_cast<uint8_t>(currentchar));
          if(currentchar == 'ä')
          {
            MyLayerTeams->write(0x84);   // Print the u-with-umlauts
          }
          else if (currentchar == 'ö')
          {
            MyLayerTeams->write(0x94);
          }
          else if (currentchar == 'ü')
          {
            MyLayerTeams->write(0x81);
          }
          else
          {
            MyLayerTeams->print(currentchar);
          }
        }
        Serial.println("");
        MyLayerTeams->setCursor(22, y_Offset+11);
        MyLayerTeams->println("vs.");

        MyLayerTeams->setCursor(2, y_Offset+22);
        Serial.print("Teamname 2 ");
                for(size_t t=0; t < 12; t++)
        {
          
          char currentchar = _radio_matchDataLong.TeamName2[t];
          MyMatch->Teamname2[t] = currentchar;
          Serial.print(currentchar);
          if(currentchar == 'ä')
          {
            MyLayerTeams->write(0x84);   // Print the u-with-umlauts
          }
          else if (currentchar == 'ö')
          {
            MyLayerTeams->write(0x94);
          }
          else if (currentchar == 'ü')
          {
            MyLayerTeams->write(0x81);
          }
          else
          {
            MyLayerTeams->print(currentchar);
          }
        }
        Serial.println("");
        
        MyLayerTeams->swapBuffers(false);
    //finally set scores
}

