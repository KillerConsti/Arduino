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

#include "NotoSansBold15.h"
#include "NotoSansBold36.h"
#include "NotoSansMonoSCB20.h"

// The font names are arrays references, thus must NOT be in quotes ""
#define AA_FONT_SMALL NotoSansBold15
#define AA_FONT_LARGE NotoSansBold36
#define AA_FONT_MONO  NotoSansMonoSCB20 // NotoSansMono-SemiCondensedBold 20pt
#define PI 3.1415926535897932384626433832795
#include <SPI.h>
#include <TFT_eSPI.h>       // Hardware-specific library
TFT_eSPI    tft = TFT_eSPI();
TFT_eSprite spr = TFT_eSprite(&tft); // Sprite class needs to be invoked

/* png stuff*/
#include <PNGdec.h>
#include <C:\Users\49151\Documents\Arduino\libraries\TJpg_Decoder\examples\FLash_array\Flash_Jpg\volleyball.h>
/*Fuchs kommt hierer <a href="https://de.freepik.com/vektoren-kostenlos/hand-bemalt-fuchsentwurf_975549.htm#query=fuchs%20svg&position=10&from_view=keyword&track=ais&uuid=67e8a5e5-30c4-4d4e-8363-bbf851c943e1">Bild von ajipebriana</a> auf Freepik
Bild von ajipebriana
</a>
 auf Freepik
 */
#include <C:\Users\49151\Documents\Arduino\libraries\TJpg_Decoder\examples\FLash_array\Flash_Jpg\fox.h>
PNG png; // PNG decoder inatance
#define MAX_IMAGE_WIDTH 55 // Adjust for your images
int16_t xpos_volleyball = 15;
int16_t ypos_volleyball = 115;
int16_t xpos_fox = 170;
int16_t ypos_fox = 115;

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


void setup(void) {

  Serial.begin(9600);
  tft.begin();
SPI.begin(PIN_RADIO_SCK, PIN_RADIO_MISO, PIN_RADIO_MOSI, PIN_RADIO_CSN);
    
    // Indicate to NRFLite that it should not call SPI.begin() during initialization since it has already been done.
    uint8_t callSpiBegin = 0;
    
    if (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE2MBPS, 100, callSpiBegin))
    {
        Serial.println("Cannot communicate with radio");
        while (1); // Wait here forever.
    }
  

  tft.setRotation(2);

  spr.setColorDepth(16); // 16 bit colour needed to show antialiased fonts

    uint16_t grey = tft.color565(11,11,11);
  uint16_t dirty_white = tft.color565(238,238,238);
  tft.fillScreen(grey);
  //tft.fillRect(0, 90, 240, 240, dirty_white);
  tft.fillCircle(60, 180, 90, TFT_DARKGREY);
  tft.fillCircle(180, 180, 90, TFT_DARKGREY);


  tft.fillCircle(60, 180, 88, dirty_white);
  tft.fillCircle(180, 180, 88, dirty_white);
  
  tft.fillRect(60, 90, 120, 240, dirty_white);

  //tft.drawLine(50, 115, 0, 240, TFT_DARKGREY);
  //tft.drawLine(190, 115, 240, 240, TFT_DARKGREY);
  DrawVolleyball();
  DrawFox();
    //tft.drawCircle(120, 120, 120, TFT_DARKGREY);
  //tft.drawCircle(120, 120, 119, TFT_DARKGREY);
  tft.drawArc(120, 120, 120,119, 270, 90, TFT_DARKGREY, TFT_DARKGREY);
  uint16_t bluish = tft.color565(11,206,140);
  /* start here*/
  //border
  tft.fillCircle(60, 90, 27, TFT_DARKGREY);
  tft.fillCircle(180, 90, 27, TFT_DARKGREY);
  tft.fillRect(60, 63 , 120, 55, TFT_DARKGREY);
  //real obj


  tft.fillCircle(60, 90, 25,bluish );
  tft.fillCircle(180, 90, 25,bluish );
  //tft.drawRect(60, 85, 100, 30, TFT_RED);
  tft.fillRect(60, 65, 120, 51, bluish);

  //debug lines
  /*tft.drawLine(120, 0, 120, 240, TFT_RED);
  tft.drawLine(0, 120, 240, 120, TFT_RED);
    tft.drawLine(140, 0, 140, 240, TFT_RED);
      tft.drawLine(100, 0, 100, 240, TFT_RED);*/
      
}

void loop() {

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
        SetNewScore(_radioData.T1_Score,_radioData.T2_Score,_radioData.T1_Sets,_radioData.T2_Sets);
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
        UpdateHistory(_radioHistoryData.myVolleyBallHistory,5);
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
  return;
}

void UpdateHistory(VolleyBallHistory TheArray[], int n)
{
     int xpos = tft.width() / 2; // Half the screen width
  int ypos = 50;
  uint16_t grey = tft.color565(11,11,11);
  uint16_t bluish = tft.color565(11,206,140);
  uint16_t dirty_white = tft.color565(238,238,238);
        int counter = 5;
  int MaxSpace = counter*19+2; 
  spr.createSprite(90, MaxSpace);
  spr.fillSprite(dirty_white);
  spr.setTextColor(TFT_BLACK, TFT_DARKGREY);
  spr.loadFont(AA_FONT_SMALL);
  //spr.loadFont(AA_FONT_MONO);
  for(size_t i = 0; i < n; i++)
  {
    if(!TheArray[i].Set)
    {

    } 
    else
    {
      //spr.drawString("(1)",5,i*20);
      spr.print("(");
      int j = TheArray[i].Satz+1;
      spr.print(j);
      spr.print(") ");
      j = (int)TheArray[i].Team1;
      spr.printf("%02d : ",j);
      j = (int)TheArray[i].Team2;
      spr.printf("%02d",j);
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
void SetNewScore(int T1,int T2, int S1,int S2)
{
  char returnme[2];
  char returnme2[2];
 int xpos = tft.width() / 2; // Half the screen width
  int ypos = 50;
  uint16_t grey = tft.color565(11,11,11);
  uint16_t bluish = tft.color565(11,206,140);
  uint16_t dirty_white = tft.color565(238,238,238);
  delay(1000);

  spr.loadFont(AA_FONT_LARGE);
  spr.createSprite(80, 40);
  spr.fillSprite(grey);
  spr.setTextColor(dirty_white, TFT_DARKGREY); 
  spr.drawNumber(S1,0,00);
  spr.drawString(":",35,00);
  spr.drawNumber(S2,60,0);
  spr.pushSprite(80, 20);         // Push to TFT screen coord 10, 10
  //Middle in green
  spr.deleteSprite();
  spr.createSprite(124, 45);
  spr.setTextColor(TFT_WHITE, TFT_DARKGREY);
  spr.fillSprite(bluish);
    Serial.println(returnme);
  Serial.println(returnme2);
  if(T1 < 10)
  {
    spr.drawNumber(0,0,10);
    spr.drawNumber(T1,22,10);
  }
  else
    spr.drawNumber(T1,0,10);
  spr.drawString(":",55,10);
    if(T2< 10)
  {
    spr.drawNumber(0,80,10);
    spr.drawNumber(T2,102,10);
  }
  else
    spr.drawNumber(T2,80,10);
  Serial.println(returnme);
  Serial.println(returnme2);
  spr.pushSprite(60, 67); 
  spr.deleteSprite();
}

// #########################################################################
// Draw a number in a rounded rectangle with some transparent pixels
// Load the font before calling
// #########################################################################
void button(int x, int y, int num )
{

  // Size of sprite
  #define IWIDTH  40
  #define IHEIGHT 25

  // Create a 16 bit sprite 40 pixels wide, 25 high (2000 bytes of RAM needed)
  spr.setColorDepth(16);
  spr.createSprite(IWIDTH, IHEIGHT);

  // Fill it with black (this will be the transparent colour this time)
  spr.fillSprite(TFT_BLACK);

  // Draw a background for the numbers
  spr.fillRoundRect(  0, 0,  IWIDTH, IHEIGHT, 8, TFT_RED);
  spr.drawRoundRect(  0, 0,  IWIDTH, IHEIGHT, 8, TFT_WHITE);

  // Set the font parameters

  // Set text coordinate datum to middle centre
  spr.setTextDatum(MC_DATUM);

  // Set the font colour and the background colour
  spr.setTextColor(TFT_WHITE, TFT_RED);

  // Draw the number
  spr.drawNumber(num, IWIDTH/2, 1 + IHEIGHT/2);

  // Push sprite to TFT screen CGRAM at coordinate x,y (top left corner)
  // All black pixels will not be drawn hence will show as "transparent"
  spr.pushSprite(x, y, TFT_BLACK);

  // Delete sprite to free up the RAM
  spr.deleteSprite();
}

void DrawVolleyball()
{
  int16_t rc = png.openFLASH((uint8_t *)volleyball, sizeof(volleyball), pngDraw);
  if (rc == PNG_SUCCESS) {
    Serial.println("Successfully opened png file");
    Serial.printf("image specs: (%d x %d), %d bpp, pixel type: %d\n", png.getWidth(), png.getHeight(), png.getBpp(), png.getPixelType());
    tft.startWrite();
    uint32_t dt = millis();
    rc = png.decode(NULL, 0);
    Serial.print(millis() - dt); Serial.println("ms");
    tft.endWrite();
    // png.close(); // not needed for memory->memory decode
  }
}

void DrawFox()
{
  int16_t rc = png.openFLASH((uint8_t *)fox, sizeof(fox), pngDrawfox);
  if (rc == PNG_SUCCESS) {
    Serial.println("Successfully opened png file");
    Serial.printf("image specs: (%d x %d), %d bpp, pixel type: %d\n", png.getWidth(), png.getHeight(), png.getBpp(), png.getPixelType());
    tft.startWrite();
    uint32_t dt = millis();
    rc = png.decode(NULL, 0);
    Serial.print(millis() - dt); Serial.println("ms");
    tft.endWrite();
    // png.close(); // not needed for memory->memory decode
  }
}

void pngDraw(PNGDRAW *pDraw) {
  uint16_t lineBuffer[MAX_IMAGE_WIDTH];
  png.getLineAsRGB565(pDraw, lineBuffer, PNG_RGB565_BIG_ENDIAN, 0xffffffff);
  tft.pushImage(xpos_volleyball, ypos_volleyball + pDraw->y, pDraw->iWidth, 1, lineBuffer);
}

void pngDrawfox(PNGDRAW *pDraw) {
  uint16_t lineBuffer[MAX_IMAGE_WIDTH];
  png.getLineAsRGB565(pDraw, lineBuffer, PNG_RGB565_BIG_ENDIAN, 0xffffffff);
  tft.pushImage(xpos_fox, ypos_fox + pDraw->y, pDraw->iWidth, 1, lineBuffer);
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