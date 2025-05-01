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
void setup(void) {

  Serial.begin(250000);

  tft.begin();

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
  tft.drawLine(120, 0, 120, 240, TFT_RED);
  tft.drawLine(0, 120, 240, 120, TFT_RED);
    tft.drawLine(140, 0, 140, 240, TFT_RED);
      tft.drawLine(100, 0, 100, 240, TFT_RED);
      
}

void loop() {
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
  spr.drawNumber(1,0,00);
  spr.drawString(":",35,00);
  spr.drawNumber(5,60,0);
  spr.pushSprite(80, 20);         // Push to TFT screen coord 10, 10
  //Middle in green
  spr.deleteSprite();
  spr.createSprite(120, 45);
  spr.setTextColor(TFT_WHITE, TFT_DARKGREY);
  spr.fillSprite(bluish);
  spr.drawString("02",0,10);
  spr.drawString(":",55,10);
  spr.drawString("15",80,10);
  spr.pushSprite(60, 67); 

  //History
  
  spr.deleteSprite();
  int counter = 3;
  int MaxSpace = counter*19+2; 
  spr.createSprite(90, MaxSpace);
  spr.fillSprite(dirty_white);
  spr.setTextColor(TFT_BLACK, TFT_DARKGREY);
  spr.loadFont(AA_FONT_SMALL);
  for(int i = 0; i < counter;i++)
  //for(int i = 0; i < 5;i++)
  {
    spr.drawString("(1)",5,i*20);
      spr.drawString("02",30,i*20);
  spr.drawString(":",55,i*20);
  spr.drawString("15",65,i*20);
  
  }
  spr.pushSprite(75, 121); 
  if(counter <= 3)
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
  }
  /*for (int i = 0; i <= 200; i++) {
    spr.fillSprite(TFT_BLUE);
    spr.drawFloat(i / 100.0, 2, 50, 10); // draw with 2 decimal places at 50,10 in sprite
    spr.pushSprite(10, 130); // Push to TFT screen coord 10, 130
    delay (20);
  }*/
  delay(1000);
  spr.unloadFont(); // Remove the font to recover memory used

  spr.deleteSprite(); // Recover memory
  // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  // Small font
  // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

  /*spr.loadFont(AA_FONT_SMALL); // Must load the font first into the sprite class

  spr.createSprite(100, 50);   // Create a sprite 100 pixels wide and 50 high

  spr.fillSprite(TFT_BLUE);

  spr.drawRect(0, 0, 100, 50, TFT_WHITE); // Draw sprite border outline (so we see extent)

  spr.setTextColor(TFT_YELLOW, TFT_DARKGREY); // Set the sprite font colour and the background colour

  spr.setTextDatum(MC_DATUM); // Middle Centre datum
  
  spr.drawString("15pt font", 50, 25 ); // Coords of middle of 100 x 50 Sprite

  spr.pushSprite(10, 10); // Push to TFT screen coord 10, 10

  spr.pushSprite(10, 70, TFT_BLUE); // Push to TFT screen, TFT_BLUE is transparent
 
  spr.unloadFont(); // Remove the font from sprite class to recover memory used

  delay(4000);

  // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  // Large font
  // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

  tft.fillScreen(TFT_BLACK);

  // Beware: Sprites are a differerent "class" to TFT, so different fonts can be loaded
  // in the tft and sprite instances, so load the font in the class instance you use!
  // In this example this means the spr. instance.

  spr.loadFont(AA_FONT_LARGE); // Load another different font into the sprite instance

  // 100 x 50 sprite was created above and still exists...

  spr.fillSprite(TFT_GREEN);

  spr.setTextColor(TFT_BLACK, TFT_GREEN); // Set the font colour and the background colour

  spr.setTextDatum(MC_DATUM); // Middle Centre datum

  spr.drawString("Fits", 50, 25); // Make sure text fits in the Sprite!
  spr.pushSprite(10, 10);         // Push to TFT screen coord 10, 10

  spr.fillSprite(TFT_RED);
  spr.setTextColor(TFT_WHITE, TFT_RED); // Set the font colour and the background colour

  spr.drawString("Too big", 50, 25); // Text is too big to all fit in the Sprite!
  spr.pushSprite(10, 70);            // Push to TFT screen coord 10, 70

  // Draw changing numbers - no flicker using this plot method!

  // >>>> Note: it is best to use drawNumber() and drawFloat() for numeric values <<<<
  // >>>> this reduces digit position movement when the value changes             <<<<
  // >>>> drawNumber() and drawFloat() functions behave like drawString() and are <<<<
  // >>>> supported by setTextDatum() and setTextPadding()                        <<<<

  spr.setTextDatum(TC_DATUM); // Top Centre datum

  spr.setTextColor(TFT_WHITE, TFT_BLUE); // Set the font colour and the background colour

  for (int i = 0; i <= 200; i++) {
    spr.fillSprite(TFT_BLUE);
    spr.drawFloat(i / 100.0, 2, 50, 10); // draw with 2 decimal places at 50,10 in sprite
    spr.pushSprite(10, 130); // Push to TFT screen coord 10, 130
    delay (20);
  }

  spr.unloadFont(); // Remove the font to recover memory used

  spr.deleteSprite(); // Recover memory
  
  delay(1000);

  // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  // Mono spaced font
  // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  
  spr.loadFont(AA_FONT_MONO); // Mono spaced fonts have fixed inter-character gaps to
                              // aid formatting
  int bnum = 1;

  // Example of drawing buttons
  for (int j = 0; j < 4; j++)
  {
    for (int k = 0; k < 4; k++)
    {
      int x = 120 + k * 45;
      int y = 40  + j * 30;
      button(x, y, bnum++);
    }
  }

  for (int i = 0; i < 100; i++)
  {
    button(120, 160, i);
    delay(50);
  }
  
  spr.unloadFont();

  delay(8000);*/
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
