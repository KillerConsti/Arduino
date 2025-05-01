#include <PS4Controller.h>

#include <FastLED.h>

FASTLED_USING_NAMESPACE

// FastLED "100-lines-of-code" demo reel, showing just a few 
// of the kinds of animation patterns you can quickly and easily 
// compose using FastLED.  
//
// This example also shows one easy way to define multiple 
// animations patterns and have them automatically rotate.
//
// -Mark Kriegsman, December 2014


#define DATA_PIN    17
//#define CLK_PIN   4
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    288
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120
int index3;
CRGB currentPalette2;
void setup()
{
 Serial.begin(115200);
  Serial.println("Starting Arduino BLE Client application...");
  index3 =0;
  
    PS4.begin("b0:05:94:3c:25:d5");
  Serial.println("Initialization ready!");
  PS4.attachOnConnect(onConnection);
  PS4.attach(onEvent);
  currentPalette2 = CRGB::Red;
  //FastLED
    delay(3000); // 3 second delay for recovery
  Serial.println("init led ..");
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
  SetNewNumber();
  Serial.println("finished");
}

void onEvent(){
bool Eventhappened = false;
 
 
  if(PS4.event.button_down.cross){
    Serial.println("cross pressed");
    index3++;
    Eventhappened = true;
    if(index3 > 288) index3 =287;
  }
   
  if(PS4.event.button_down.square){
     Serial.println("square pressed");
     index3--;
     Eventhappened = true;
     if(index3 < 0) index3 =0;
  }
  if(PS4.event.button_down.options)
  {
  index3 = 0;
  Eventhappened = true;
  }
  if(Eventhappened)
   SetNewNumber();
}
 
void onConnection() {
   
  if (PS4.isConnected()) {
    Serial.println("Controller connected.");
  }
}

void SetNewNumber() //Set Number but only on event
{
   int lastdigit =index3 % 10;
    int firstdigit =(index3 - lastdigit)/10;

    //left side
    SetLeftLEDPad(TranslateNumbers(firstdigit),0,CRGB::Red);
    SetLeftLEDPad(TranslateNumbers(lastdigit),48,CRGB::Red);
    ShowMiniNumber(lastdigit,96,CRGB::Green);

    //right side
    ShowMiniNumber_Inverted(firstdigit,96+40,CRGB::Green);
    SetLeftLEDPad(TranslateNumbers(2),48+96+16,CRGB::Blue);
    SetLeftLEDPad(TranslateNumbers(3),96+96+16,CRGB::Blue);

      Serial.print("X is");
  Serial.println(index3); 
  FastLED.show();
}
void loop()
{
 //eventbased
    /*if (PS4.isConnected()){
    Serial.println("Connected!");
  }

  delay(500);*/
}

int TranslateNumbers(int Input)
{
  //translate number to bitwise output

  //1 ->rechts unten, rechter Streifen, rechts, Mitte, rechts oben

  /*
   * 1 unten rechts
   *  2 unten
   *  4 unten links
   *  
   *  8 Mitte links
   *  16 Mitte
   *  32 Mitte rechts
   *  
   *  64 oben links
   *  128 oben Mitte
   *  256 oben rechts
   *  512 s links -oben
   *  1024 s links unten
   *  2048 s rechts oben
   *  4096 s rechts unten
   */
  if(Input ==1)
  return(1+32+256+2048+4096);
    if(Input ==2)
  return(1+2+4+8+16+32+64+128+256+1024+2048);
  if(Input ==3)
  return(1+2+4+8+16+32+64+128+256+2048+4096);
  if(Input ==4)
  return(1+8+16+32+64+256+512+2048+4096);
  if(Input ==5)
  return(1+2+4+8+16+32+64+128+256+4096+512);
    if(Input ==6)
  return(1+2+4+8+16+32+64+128+256+512+1024+4096);
    if(Input ==7)
  return(1+32+128+256+2048+4096);
      if(Input ==8)
  return(1+2+4+8+16+32+64+128+256+512+1024+2048+4096);
      if(Input ==9)
  return(1+2+4+8+16+32+64+128+256+512+2048+4096);
  
  if(Input ==0)
  return(1+2+4+8+32+64+128+256+512+1024+2048+4096);

  return(0);
}

void ShowMiniNumber(int Number,int Verschiebung,CRGB Color)
{
    for(int x = 0; x <= 20;x++) //Reset Leds
    {
      leds[Verschiebung+x] = CRGB::Black;
    }
  //Panel = 3* 5
  if(Number == 0)
  {
    leds[Verschiebung+0] = Color;
    leds[Verschiebung+1] = Color;
    leds[Verschiebung+2] = Color;
    leds[Verschiebung+3] = Color;
    leds[Verschiebung+4] = Color;
    leds[Verschiebung+11] = Color;
    leds[Verschiebung+15] = Color;
    leds[Verschiebung+16] = Color;
    leds[Verschiebung+17] = Color;
    leds[Verschiebung+18] = Color;
    leds[Verschiebung+19] = Color;
    leds[Verschiebung+20] = Color;
  }
  else if (Number == 1)
  {
    leds[Verschiebung+14] = Color;
    leds[Verschiebung+16] = Color;
    leds[Verschiebung+17] = Color;
    leds[Verschiebung+18] = Color;
    leds[Verschiebung+19] = Color;
    leds[Verschiebung+20] = Color;
  }
  else if (Number == 2)
  {
      leds[Verschiebung+0] = Color;
      leds[Verschiebung+3] = Color;
      leds[Verschiebung+4] = Color;
      leds[Verschiebung+11] = Color;
      leds[Verschiebung+13] = Color;
      leds[Verschiebung+15] = Color;
      leds[Verschiebung+16] = Color;
      leds[Verschiebung+17] = Color;
      leds[Verschiebung+18] = Color;
      leds[Verschiebung+20] = Color;
      
  }
} 
void ShowMiniNumber_Inverted(int Number,int Verschiebung,CRGB Color) //Platzverschiebung, da led Leiste von unten kommt
{
      for(int x = 0; x <= 20;x++) //Reset Leds
    {
      leds[Verschiebung+x] = CRGB::Black;
    }
    if(Number == 0)
  {
    leds[Verschiebung+3] = Color;
    leds[Verschiebung+4] = Color;
    leds[Verschiebung+5] = Color;
    leds[Verschiebung+6] = Color;
    leds[Verschiebung+7] = Color;
    leds[Verschiebung+8] = Color;
    leds[Verschiebung+12] = Color;
    leds[Verschiebung+19] = Color;
    leds[Verschiebung+20] = Color;
    leds[Verschiebung+21] = Color;
    leds[Verschiebung+22] = Color;
    leds[Verschiebung+23] = Color;
  }
   else if (Number == 1)
  {
    leds[Verschiebung+9] = Color;
    leds[Verschiebung+19] = Color;
    leds[Verschiebung+20] = Color;
    leds[Verschiebung+21] = Color;
    leds[Verschiebung+22] = Color;
    leds[Verschiebung+23] = Color;
  }
  else if (Number == 2)
  {
      leds[Verschiebung+3] = Color;
      leds[Verschiebung+4] = Color;
      leds[Verschiebung+7] = Color;
      leds[Verschiebung+8] = Color;
      leds[Verschiebung+10] = Color;
      leds[Verschiebung+12] = Color;
      leds[Verschiebung+19] = Color;
      leds[Verschiebung+21] = Color;
      leds[Verschiebung+22] = Color;
      leds[Verschiebung+23] = Color;
      
  }
}
void SetLeftLEDPad(int CompareVal, int Verschiebung,CRGB currentPalette)
{
  if(CompareVal & 1) //unten Rechts
  {
    leds[39+Verschiebung] = currentPalette;
  }
  else
  {
    leds[39+Verschiebung] = CRGB::Black;
  }
  if(CompareVal & 2) //unten
  {
    leds[23+Verschiebung] = currentPalette;
    leds[24+Verschiebung] = currentPalette;
  }
    else
  {
    leds[23+Verschiebung] = CRGB::Black;
    leds[24+Verschiebung] = CRGB::Black;
  }
    if(CompareVal & 4) //unten links
  {
    leds[8+Verschiebung] = currentPalette;
  }
    else
  {
    leds[8+Verschiebung] = CRGB::Black;
  }
  //Mitte
  if(CompareVal & 8)//Mitte links
  {
    leds[11+Verschiebung] = currentPalette;
    leds[12+Verschiebung] = currentPalette;
  }
  else
  {
    leds[11+Verschiebung] = CRGB::Black;
    leds[12+Verschiebung] = CRGB::Black;
  }
  
  
  if(CompareVal & 16)//Mitte
    {
    leds[19+Verschiebung] = currentPalette;
    leds[20+Verschiebung] = currentPalette;
    leds[27+Verschiebung] = currentPalette;
    leds[28+Verschiebung] = currentPalette;
  }
    else
  {
    leds[19+Verschiebung] = CRGB::Black;
    leds[20+Verschiebung] = CRGB::Black;
    leds[27+Verschiebung] = CRGB::Black;
    leds[28+Verschiebung] = CRGB::Black;
  }

    if(CompareVal & 32)//Mitte rechts
  {
    leds[35+Verschiebung] = currentPalette;
    leds[36+Verschiebung] = currentPalette;
  }
  else
  {
    leds[35+Verschiebung] = CRGB::Black;
    leds[36+Verschiebung] = CRGB::Black;
  }

  //oben
    if(CompareVal & 64)//links
  {
    leds[15+Verschiebung] = currentPalette;
  }
  else
  {
    leds[15+Verschiebung] = CRGB::Black;
  }

  
  
  if(CompareVal & 128)//Mitte
    {
    leds[16+Verschiebung] = currentPalette;
    leds[31+Verschiebung] = currentPalette;
  }
    else
  {
    leds[16+Verschiebung] = CRGB::Black;
    leds[31+Verschiebung] = CRGB::Black;
  }

   if(CompareVal & 256)// rechts
  {
    leds[32+Verschiebung] = currentPalette;
  }
  else
  {
    leds[32+Verschiebung] = CRGB::Black;
  }


  //Streifen links

   if(CompareVal & 512)// s_links_oben
   {
    leds[13+Verschiebung] = currentPalette;
    leds[14+Verschiebung] = currentPalette;
  }
    else
  {
    leds[13+Verschiebung] = CRGB::Black;
    leds[14+Verschiebung] = CRGB::Black;
  }

    if(CompareVal & 1024)//s_links_unten
   {
    leds[9+Verschiebung] = currentPalette;
    leds[10+Verschiebung] = currentPalette;
  }
    else
  {
    leds[9+Verschiebung] = CRGB::Black;
    leds[10+Verschiebung] = CRGB::Black;
  }

    //Streifen rechts


    if(CompareVal & 4096)//rechts unten
   {
    leds[37+Verschiebung] = currentPalette;
    leds[38+Verschiebung] = currentPalette;
  }
    else
  {
    leds[37+Verschiebung] = CRGB::Black;
    leds[38+Verschiebung] = CRGB::Black;
  }

  
   if(CompareVal & 2048)// s_rechts_oben
   {
    leds[33+Verschiebung] = currentPalette;
    leds[34+Verschiebung] = currentPalette;
  }
    else
  {
    leds[33+Verschiebung] = CRGB::Black;
    leds[34+Verschiebung] = CRGB::Black;
  }
}
