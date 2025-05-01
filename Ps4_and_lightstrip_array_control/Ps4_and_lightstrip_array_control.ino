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
CRGB currentPalette;
void setup()
{
 Serial.begin(115200);
  Serial.println("Starting Arduino BLE Client application...");
  index3 =0;
  
    PS4.begin("b0:05:94:3c:25:d5");
  Serial.println("Initialization ready!");
  PS4.attachOnConnect(onConnection);
  PS4.attach(onEvent);
  currentPalette = CRGB::Red;
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
    currentPalette = CRGB::Red;
    Eventhappened = true;
    if(index3 > 288) index3 =287;
  }
   
  if(PS4.event.button_down.square){
     Serial.println("square pressed");
     index3--;
     Eventhappened = true;
     currentPalette = CRGB::Red;
     if(index3 < 0) index3 =0;
  }
  if(PS4.event.button_down.options)
  {
  index3 = 0;
  currentPalette = CRGB::Red;
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
    SetRightLEDPad(TranslateNumbers(lastdigit));
    SetLeftLEDPad(TranslateNumbers(firstdigit));
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
  if(Input ==1)
  return(1+32+256+2048+4096);
  if(Input ==5)
  return(1+2+4+8+16+32+64+128+256+4096+512);
  if(Input ==3)
  return(1+2+4+8+16+32+64+128+256+2048+4096);
  if(Input ==4)
  return(1+8+16+32+64+256+512+2048+4096);
  if(Input ==2)
  return(1+2+4+8+16+32+64+128+256+1024+2048);
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
void SetRightLEDPad(int CompareVal)
{
  if(CompareVal & 1) //unten Rechts
  {
    leds[3] = currentPalette;
  }
  else
  {
    leds[3] = CRGB::Black;
  }
  if(CompareVal & 2) //unten
  {
    leds[4] = currentPalette;
    leds[5] = currentPalette;
    leds[6] = currentPalette;
    leds[7] = currentPalette;
  }
    else
  {
    leds[4] = CRGB::Black;
    leds[5] = CRGB::Black;
    leds[6] = CRGB::Black;
    leds[7] = CRGB::Black;
  }
    if(CompareVal & 4) //unten links
  {
    leds[8] = currentPalette;
  }
    else
  {
    leds[8] = CRGB::Black;
  }
  //Mitte
  if(CompareVal & 8)//Mitte links
  {
    leds[49] = currentPalette;
  }
  else
  {
    leds[49] = CRGB::Black;
  }
  
  
  if(CompareVal & 16)//Mitte
    {
    leds[50] = currentPalette;
    leds[51] = currentPalette;
    leds[52] = currentPalette;
    leds[53] = currentPalette;
  }
    else
  {
    leds[50] = CRGB::Black;
    leds[51] = CRGB::Black;
    leds[52] = CRGB::Black;
    leds[53] = CRGB::Black;
  }

    if(CompareVal & 32)//Mitte rechts
  {
    leds[54] = currentPalette;
  }
  else
  {
    leds[54] = CRGB::Black;
  }

  //oben
    if(CompareVal & 64)//links
  {
    leds[79] = currentPalette;
  }
  else
  {
    leds[79] = CRGB::Black;
  }

  
  
  if(CompareVal & 128)//Mitte
    {
    leds[75] = currentPalette;
    leds[76] = currentPalette;
    leds[77] = currentPalette;
    leds[78] = currentPalette;
  }
    else
  {
    leds[75] = CRGB::Black;
    leds[76] = CRGB::Black;
    leds[77] = CRGB::Black;
    leds[78] = CRGB::Black;
  }

   if(CompareVal & 256)// rechts
  {
    leds[74] = currentPalette;
  }
  else
  {
    leds[74] = CRGB::Black;
  }


  //Streifen links

   if(CompareVal & 512)// s_links_oben
   {
    leds[169] = currentPalette;
    leds[170] = currentPalette;
    leds[171] = currentPalette;
  }
    else
  {
    leds[169] = CRGB::Black;
    leds[170] = CRGB::Black;
    leds[171] = CRGB::Black;
  }

    if(CompareVal & 1024)//s_links_unten
   {
    leds[177] = currentPalette;
    leds[178] = currentPalette;
    leds[179] = currentPalette;
  }
    else
  {
    leds[177] = CRGB::Black;
    leds[178] = CRGB::Black;
    leds[179] = CRGB::Black;
  }

    //Streifen rechts


    if(CompareVal & 2048)//rechts unten
   {
    leds[207] = currentPalette;
    leds[208] = currentPalette;
    leds[209] = currentPalette;
  }
    else
  {
    leds[207] = CRGB::Black;
    leds[208] = CRGB::Black;
    leds[209] = CRGB::Black;
  }

  
   if(CompareVal & 4096)// s_rechts_oben
   {
    leds[200] = currentPalette;
    leds[201] = currentPalette;
    leds[202] = currentPalette;
  }
    else
  {
    leds[200] = CRGB::Black;
    leds[201] = CRGB::Black;
    leds[202] = CRGB::Black;
  }
  FastLED.show();
}

void SetLeftLEDPad(int CompareVal)
{
  if(CompareVal & 1) //unten Rechts
  {
    leds[12] = currentPalette;
  }
  else
  {
    leds[12] = CRGB::Black;
  }
  if(CompareVal & 2) //unten
  {
    leds[13] = currentPalette;
    leds[14] = currentPalette;
    leds[15] = currentPalette;
    leds[16] = currentPalette;
  }
    else
  {
    leds[13] = CRGB::Black;
    leds[14] = CRGB::Black;
    leds[15] = CRGB::Black;
    leds[16] = CRGB::Black;
  }
    if(CompareVal & 4) //unten links
  {
    leds[17] = currentPalette;
  }
    else
  {
    leds[17] = CRGB::Black;
  }
  //Mitte
  if(CompareVal & 8)//Mitte links
  {
    leds[40] = currentPalette;
  }
  else
  {
    leds[40] = CRGB::Black;
  }
  
  
  if(CompareVal & 16)//Mitte
    {
    leds[41] = currentPalette;
    leds[42] = currentPalette;
    leds[43] = currentPalette;
    leds[44] = currentPalette;
  }
    else
  {
    leds[41] = CRGB::Black;
    leds[42] = CRGB::Black;
    leds[43] = CRGB::Black;
    leds[44] = CRGB::Black;
  }

    if(CompareVal & 32)//Mitte rechts
  {
    leds[45] = currentPalette;
  }
  else
  {
    leds[45] = CRGB::Black;
  }

  //oben
    if(CompareVal & 64)//links
  {
    leds[88] = currentPalette;
  }
  else
  {
    leds[88] = CRGB::Black;
  }

  
  
  if(CompareVal & 128)//Mitte
    {
    leds[84] = currentPalette;
    leds[85] = currentPalette;
    leds[86] = currentPalette;
    leds[87] = currentPalette;
  }
    else
  {
    leds[84] = CRGB::Black;
    leds[85] = CRGB::Black;
    leds[86] = CRGB::Black;
    leds[87] = CRGB::Black;
  }

   if(CompareVal & 256)// rechts
  {
    leds[83] = currentPalette;
  }
  else
  {
    leds[83] = CRGB::Black;
  }


  //Streifen links

   if(CompareVal & 512)// s_links_oben
   {
    leds[112] = currentPalette;
    leds[113] = currentPalette;
    leds[114] = currentPalette;
  }
    else
  {
    leds[112] = CRGB::Black;
    leds[113] = CRGB::Black;
    leds[114] = CRGB::Black;
  }

    if(CompareVal & 1024)//s_links_unten
   {
    leds[121] = currentPalette;
    leds[122] = currentPalette;
    leds[123] = currentPalette;
  }
    else
  {
    leds[121] = CRGB::Black;
    leds[122] = CRGB::Black;
    leds[123] = CRGB::Black;
  }

    //Streifen rechts


    if(CompareVal & 4096)//rechts unten
   {
    leds[140] = currentPalette;
    leds[141] = currentPalette;
    leds[142] = currentPalette;
  }
    else
  {
    leds[140] = CRGB::Black;
    leds[141] = CRGB::Black;
    leds[142] = CRGB::Black;
  }

  
   if(CompareVal & 2048)// s_rechts_oben
   {
    leds[148] = currentPalette;
    leds[149] = currentPalette;
    leds[150] = currentPalette;
  }
    else
  {
    leds[148] = CRGB::Black;
    leds[149] = CRGB::Black;
    leds[150] = CRGB::Black;
  }
  FastLED.show();
}
