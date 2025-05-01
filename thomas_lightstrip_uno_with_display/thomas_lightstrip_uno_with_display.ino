
#include <FastLED.h>
#include <EEPROM.h>
#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
FASTLED_USING_NAMESPACE

// FastLED "100-lines-of-code" demo reel, showing just a few 
// of the kinds of animation patterns you can quickly and easily 
// compose using FastLED.  
//
// This example also shows one easy way to define multiple 
// animations patterns and have them automatically rotate.
//
// -Mark Kriegsman, December 2014

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
#define DATA_PIN    3
//#define CLK_PIN   4
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    288
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120
int left_number_value;
int right_nmber_value;
int left_set_value;
int right_set_value;
int currentPalette_index_LeftNumber;
int currentPalette_index_MiddleNumber;
int currentPalette_index_RightNumber;
int Helligkeit;
//Bedienungselements
//drehschalter
int DT = 6;
int CLK = 5;
int SW = 7;
int Position = 0;
int Letzte_Position = LOW;
int n = LOW;
int Taster = LOW;
int Letzte_Taster = LOW;

//4 buttons
int b_oben_rechts = 8;
int b_oben_rechts_old_state = LOW;
int b_unten_rechts = 9;
int b_unten_rechts_old_state = LOW;
int b_links_oben = 10;
int b_links_oben_old_state = LOW;
int b_links_unten = 11;
int b_links_unten_old_state = LOW;

bool IgnoreUpdateLeftBecauseSwap = false;
bool IgnoreUpdateRightBecauseSwap = false;
//Timer Funktions - track time

unsigned long StartTime;
unsigned long CurrentTime;

unsigned long ButtonTimeLeftUP =0 ;
unsigned long ButtonTimeLeftDown = 0;
unsigned long ButtonTimeRightUP =0 ;
unsigned long ButtonTimeRightDown = 0;
unsigned long ButtonTimeReset = 0;

unsigned long ButtonTimeSwap = 0;
int ModifyButtonState = 0;
bool ColorVal_changed =false;
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
  CHSV(90, 255, 255)
);
void setup()
{
  Serial.begin(115200);
  //EEPROM.write(0, 31); //we only did this once for setup board no longer needed for that board
  //EEPROM.write(1, 79); 
  //EEPROM.write(2, 63); 
  Serial.println("Initialization starting!");

  //init our sensors
  // initialize the pushbutton pin as an input:
  pinMode(b_oben_rechts, INPUT);
  pinMode(b_oben_rechts, INPUT);
  pinMode(b_oben_rechts, INPUT);
  pinMode(b_oben_rechts, INPUT);
  //Drehschalter
  pinMode(CLK, INPUT_PULLUP);
  pinMode(DT, INPUT_PULLUP);
  pinMode(SW, INPUT_PULLUP);
  Serial.begin(115200);

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
  //set current display values

  left_number_value = 0;
  right_nmber_value = 0;
  left_set_value = 0;
  right_set_value = 0;

  //FastLED
  delay(3000); // 3 second delay for recovery
  Serial.println("init led ..");
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
  SetNewNumber();
  DrawOnDisplay();
  u8g2.begin();  
  Serial.println("finished");
}

void DrawOnDisplay()
{
  char left_num_str[3];
  char right_num_str[3];
  char left_set_num_str[3];
  char right_set_num_str[3];
  strcpy(left_num_str, u8x8_u8toa(left_number_value, 2));
   strcpy(right_num_str, u8x8_u8toa(right_nmber_value, 2));
    strcpy(left_set_num_str, u8x8_u8toa(left_set_value, 1));
     strcpy(right_set_num_str, u8x8_u8toa(right_set_value, 1));

     u8g2.clearBuffer();					// clear the internal memory
     //now write data
     u8g2.setFont(u8g2_font_logisoso26_tr);
    u8g2.drawStr(5,63,left_num_str);
    u8g2.drawStr(60,63,":");
    u8g2.drawStr(90,63,right_num_str);
    u8g2.setFont(u8g2_font_logisoso16_tn);
    u8g2.drawStr(50,35,left_set_num_str);
    u8g2.drawStr(70,35,right_set_num_str);
    u8g2.drawStr(62,35,":");
    u8g2.setFont(u8g2_font_8x13_mf);
    u8g2.drawStr(5, 11, "Beachfreunde");
    u8g2.setFont(u8g2_font_open_iconic_weather_2x_t);
     u8g2.drawGlyph(110, 17, 69);
    u8g2.sendBuffer();
}
void SetNewNumber() //Set Number but only on event
{
  //let us blink because we want to modify a color
  bool BlinkAndBeBlack = false;
  if (ModifyButtonState >= 0)
  {
    //Blende die Zahl aus , teile zuerst die Zeit in Millisekunden / 400 und nimm den Rest
    //Rest <= 200 ->ausblenden
    BlinkAndBeBlack = (CurrentTime % 800) <= 400;
  }

  int lastdigit_num_left = left_number_value % 10;
  int firstdigit_num_left = (left_number_value - lastdigit_num_left) / 10;

  int lastdigit_num_right = right_nmber_value % 10;
  int firstdigit_num_right = (right_nmber_value - lastdigit_num_right) / 10;

  //left side
  if (ModifyButtonState == 1 && BlinkAndBeBlack)
  {
    //0 turns off display
    SetLeftLEDPad(0, 0, CRGB::Black);
    SetLeftLEDPad(0, 48, CRGB::Black);
  }
  else //display number
  {
    SetLeftLEDPad(TranslateNumbers(firstdigit_num_left), 0, ColorFromPalette(RainbowColors_p, currentPalette_index_LeftNumber));
    SetLeftLEDPad(TranslateNumbers(lastdigit_num_left), 48, ColorFromPalette(RainbowColors_p, currentPalette_index_LeftNumber));
  }

  //minis
  if (ModifyButtonState == 2 && BlinkAndBeBlack)
  {
    //left
    ShowMiniNumber(-1, 96, CRGB::Black); //-1 is invalid ->wont show up
                       //right
    ShowMiniNumber_Inverted(-1, 96 + 40, CRGB::Black);
  }
  else
  {
    //left
    ShowMiniNumber(left_set_value, 96, ColorFromPalette(RainbowColors_p, currentPalette_index_MiddleNumber));
    //right
    ShowMiniNumber_Inverted(right_set_value, 96 + 40, ColorFromPalette(RainbowColors_p, currentPalette_index_MiddleNumber));
  }

  //right side
  if (ModifyButtonState == 3 && BlinkAndBeBlack)
  {
    //0 turns off display
    SetLeftLEDPad(0, 48 + 96 + 16, CRGB::Black);
    SetLeftLEDPad(0, 96 + 96 + 16, CRGB::Black);
  }
  else if (ModifyButtonState == 4 && BlinkAndBeBlack)
  {
    SetLeftLEDPad(0, 0, CRGB::Black);
    SetLeftLEDPad(0, 48, CRGB::Black);
    SetLeftLEDPad(0, 48 + 96 + 16, CRGB::Black);
    SetLeftLEDPad(0, 96 + 96 + 16, CRGB::Black);
    ShowMiniNumber(-1, 96, CRGB::Black); //-1 is invalid ->wont show up
                       //right
    ShowMiniNumber_Inverted(-1, 96 + 40, CRGB::Black);
  }
  else //display number
  {
    SetLeftLEDPad(TranslateNumbers(firstdigit_num_right), 48 + 96 + 16, ColorFromPalette(RainbowColors_p, currentPalette_index_RightNumber));
    SetLeftLEDPad(TranslateNumbers(lastdigit_num_right), 96 + 96 + 16, ColorFromPalette(RainbowColors_p, currentPalette_index_RightNumber));
  }
  FastLED.show();
}

void ModifyColor(bool GoUP)
{
  //4 ist auch hier weil einfacher aber macht einiges anders
  //nutze speicheradressen hier -einfacher
  int* Colorindex = NULL;
  int ZwischenValue;
  if (ModifyButtonState == 1)
    Colorindex = &currentPalette_index_LeftNumber;
  else if (ModifyButtonState == 2)
    Colorindex = &currentPalette_index_MiddleNumber;
  else if (ModifyButtonState == 3)
    Colorindex = &currentPalette_index_RightNumber;
    else if (ModifyButtonState == 4)
    Colorindex = &Helligkeit;
  if (Colorindex == NULL) //Element gibt es nicht?
    return;

  if (GoUP)
  {
    ZwischenValue = *Colorindex + 16;
    if(ModifyButtonState == 4) //do not reset brightness
    {
      ZwischenValue = constrain(ZwischenValue,32,255);
      FastLED.setBrightness(ZwischenValue/2.55 );
    }
    else if (ZwischenValue > 255)
      ZwischenValue = 0;
  }
  else
  {
     ZwischenValue = *Colorindex - 16;
         if(ModifyButtonState == 4) //do not reset brightness
    {
      ZwischenValue = constrain(ZwischenValue,32,255);
      FastLED.setBrightness(ZwischenValue/2.55 );
    }
    else if (ZwischenValue < -1)
      ZwischenValue = 255;
    else if (ZwischenValue == -1)
      ZwischenValue = 0;
  }
  *Colorindex = ZwischenValue; 
  if(ModifyButtonState != 4)
  ColorVal_changed = true;//inform eeprom to save color
  Serial.println(ZwischenValue);
}

bool CheckInputState() //return Anything changed?
{
  bool AnythingChanged =false;
  n = digitalRead(CLK);
  Taster = !digitalRead(SW);
  if (Taster != Letzte_Taster) {
    Serial.print(Position);
    Serial.print("|");
    Serial.print(Taster);

    //kc can we take it out?
    //delay(10);
    Letzte_Taster = Taster;
    //this tells us to go in another mode

    if (Letzte_Taster == false)
    {
      AnythingChanged = true;
      if(ColorVal_changed) //write new color to EEPROM -stays in memory even after reset
      {
          if(ModifyButtonState == 1)
              EEPROM.write(0,currentPalette_index_LeftNumber);
            else if(ModifyButtonState == 2)
              EEPROM.write(1,currentPalette_index_MiddleNumber);
            else  if(ModifyButtonState == 3)
            EEPROM.write(2,currentPalette_index_RightNumber);
      }
      ColorVal_changed = false;
      ModifyButtonState = ModifyButtonState + 1;
      if (ModifyButtonState >= 5)
        ModifyButtonState = 0;
      Serial.print("|");
      Serial.println(ModifyButtonState);
    }
  }
  if ((Letzte_Position == LOW) && (n == HIGH)) {
    if (digitalRead(DT) == LOW) {
      Position++;
      ModifyColor(true);
    }
    else {
      Position--;
      ModifyColor(false);
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
  if (b_oben_rechts_old_state != digitalRead(b_oben_rechts))
  {
    b_oben_rechts_old_state = digitalRead(b_oben_rechts);
    Serial.print("b_oben_rechts|");
    Serial.println(b_oben_rechts_old_state);
        if(ButtonTimeRightUP +200 < millis() && b_oben_rechts_old_state ==false)
    {
        AnythingChanged = true;
        ButtonTimeRightUP = millis();
        if(IgnoreUpdateRightBecauseSwap)
        IgnoreUpdateRightBecauseSwap = false;
        else if(ModifyButtonState != 2)
          right_nmber_value = constrain(right_nmber_value+1,0,99);
        else
          right_set_value = constrain(right_set_value+1,0,3);
        
    }
  }
  //down right
  if (b_unten_rechts_old_state != digitalRead(b_unten_rechts))
  {
    b_unten_rechts_old_state = digitalRead(b_unten_rechts);
    Serial.print("b_unten_rechts|");
    Serial.println(b_unten_rechts_old_state);
        if(ButtonTimeRightDown +200 < millis()&& b_unten_rechts_old_state ==false)
    {
        AnythingChanged = true;
        ButtonTimeRightDown = millis();
                if(ModifyButtonState != 2)
        right_nmber_value = constrain(right_nmber_value-1,0,99);
                else
          right_set_value = constrain(right_set_value-1,0,3);
    }
  }

  //upper left
  if (b_links_oben_old_state != digitalRead(b_links_oben)) //ohne Einrasten
  {
    b_links_oben_old_state = digitalRead(b_links_oben);
    Serial.print("b_links_oben|");
    Serial.println(b_links_oben_old_state);
    if(ButtonTimeLeftUP +200 < millis() && b_links_oben_old_state ==false)
    {
        AnythingChanged = true;
        ButtonTimeLeftUP = millis();
        if(IgnoreUpdateLeftBecauseSwap)
        IgnoreUpdateLeftBecauseSwap = false;
        else if(ModifyButtonState != 2)
        left_number_value = constrain(left_number_value+1,0,99);
        else
        left_set_value = constrain(left_set_value+1,0,3);
    }
  }
  //down left
  if (b_links_unten_old_state != digitalRead(b_links_unten))
  {
    b_links_unten_old_state = digitalRead(b_links_unten);
    Serial.print("b_links_unten|");
    Serial.println(b_links_unten_old_state);
    if(ButtonTimeLeftDown +200 < millis()  && b_links_unten_old_state ==false)
    {
      
        AnythingChanged = true;
        ButtonTimeLeftDown = millis();
                if(ModifyButtonState != 2)
        left_number_value = constrain(left_number_value-1,0,99);
                else
          left_set_value = constrain(left_set_value-1,0,3);
    }
  }

  /*save and reset*/
  
  if(b_links_unten_old_state && b_unten_rechts_old_state) //reset funktion - hold both lower buttons for 3 secs
  {
    if(millis() > ButtonTimeReset+10000) //safety allow it only all 10 secs
    {
        ButtonTimeReset = millis(); //get current time
    }
    else
    {
      if(ButtonTimeReset +3000 < millis()) //reset after pressing all buttons 2 secs
      {
        right_nmber_value =0;
        right_set_value =0;
        left_number_value =0;
        left_set_value =0;
        ModifyButtonState =0;
        AnythingChanged = true;
        ButtonTimeReset =millis()+100000; //set it increadible high so it wont be triggered again if buttons are not released (wait 100 secs)
      }
    }
  }
  else
    ButtonTimeReset =0;

    /*swap places fct (press both up buttons for 2 secs)
    /must tell up-buttons to not increase so points stay the same
    */
    if(b_links_oben_old_state && b_oben_rechts_old_state) //reset funktion
  {
    if(millis() > ButtonTimeSwap+10000) //safety allow it only all 10 secs
    {
        ButtonTimeSwap = millis(); //get current time
    }
    else
    {
      if(ButtonTimeSwap +2000 < millis()) //swap after pressing all buttons 3 secs
      {
        //save two values
        int mem1 = right_nmber_value;
        int mem2 =right_set_value ;
        //overwrite "swap" all 4 values
        right_nmber_value = left_number_value;
        right_set_value = left_set_value;
        left_number_value =mem1;
        left_set_value = mem2;
        //this is to make it unable to increase value after releasing buttons
        IgnoreUpdateLeftBecauseSwap = true;
        IgnoreUpdateRightBecauseSwap = true; 
        AnythingChanged = true;
        //force non update if not pressed buttons agains
        ButtonTimeSwap =millis()+100000; //set it increadible high so it wont be triggered again if buttons are not released (wait 100 secs)
      }
    }
  }
  else
    ButtonTimeSwap =0;
  return     AnythingChanged;
}


void loop()
{
  CurrentTime = millis() - StartTime;
  //Any Button or other input event?
  bool UpdateNeeded = CheckInputState();
  //set numbers ... but only if input changed?
  //can be messy because we have a blink animation
  if(ModifyButtonState != 0 || UpdateNeeded)
  {
    SetNewNumber();
    DrawOnDisplay();
  }

  //delay(10);
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
  *  8192 Special Pixels -1-
  *  16384 Special Pixels -7-
  */
  bool OldStyle = true;
  if(OldStyle)
  {
  if (Input == 1)
    return(1 + 32 + 256 + 2048 + 4096+8192);
  if (Input == 2)
    return(1 + 2 + 4 + 8 + 16 + 32 + 64 + 128 + 256 + 1024 + 2048);
  if (Input == 3)
    return(1 + 2 + 4 + 8 + 16 + 32 + 64 + 128 + 256 + 2048 + 4096);
  if (Input == 4)
    return(1 + 8 + 16 + 32 + 64 + 256 + 512 + 2048 + 4096);
  if (Input == 5)
    return(1 + 2 + 4 + 8 + 16 + 32 + 64 + 128 + 256 + 4096 + 512);
  if (Input == 6)
    return(1 + 2 + 4 + 8 + 16 + 32 + 64 + 128 + 256 + 512 + 1024 + 4096);
  if (Input == 7)
    //return(1 + 32 +64+ 128 + 256 + 2048 + 4096+16384);
    return(1 + 32 +64+ 128 + 256 + 2048 + 4096/*+16384*/);
  if (Input == 8)
    return(1 + 2 + 4 + 8 + 16 + 32 + 64 + 128 + 256 + 512 + 1024 + 2048 + 4096);
  if (Input == 9)
    return(1 + 2 + 4 + 8 + 16 + 32 + 64 + 128 + 256 + 512 + 2048 + 4096);

  if (Input == 0)
    return(1 + 2 + 4 + 8 + 32 + 64 + 128 + 256 + 512 + 1024 + 2048 + 4096);
  }
  else
  {
      if (Input == 1)
    return(1 + 32 + 256 + 2048 + 4096);
  if (Input == 2)
    return(1 + 2 + 4 + 8 + 16 + 32 + 64 + 128 + 256 + 1024 + 2048);
  if (Input == 3)
    return(1 + 2 + 4 + 8 + 16 + 32 + 64 + 128 + 256 + 2048 + 4096);
  if (Input == 4)
    return(1 + 8 + 16 + 32 + 64 + 256 + 512 + 2048 + 4096);
  if (Input == 5)
    return(1 + 2 + 4 + 8 + 16 + 32 + 64 + 128 + 256 + 4096 + 512);
  if (Input == 6)
    return(1 + 2 + 4 + 8 + 16 + 32 + 64 + 128 + 256 + 512 + 1024 + 4096);
  if (Input == 7)
    return(1 + 32 + 128 + 256 + 2048 + 4096);
  if (Input == 8)
    return(2  + 16  + 128 + 512 + 1024 + 2048 + 4096);
  if (Input == 9)
    return(1 + 2 + 4 + 8 + 16 + 32 + 64 + 128 + 256 + 512 + 2048 + 4096);

  if (Input == 0)
    return(2 + 8 + 32 + 128 + 512 + 1024 + 2048 + 4096);
  }
  return(0);
}

void ShowMiniNumber(int Number, int Verschiebung, CRGB Color)
{
  for (int x = 0; x <= 20; x++) //Reset Leds
  {
    leds[Verschiebung + x] = CRGB::Black;
  }
  //Panel = 3* 5
  if (Number == 0)
  {
    leds[Verschiebung + 0] = Color;
    leds[Verschiebung + 1] = Color;
    leds[Verschiebung + 2] = Color;
    leds[Verschiebung + 3] = Color;
    leds[Verschiebung + 4] = Color;
    leds[Verschiebung + 11] = Color;
    leds[Verschiebung + 15] = Color;
    leds[Verschiebung + 16] = Color;
    leds[Verschiebung + 17] = Color;
    leds[Verschiebung + 18] = Color;
    leds[Verschiebung + 19] = Color;
    leds[Verschiebung + 20] = Color;
  }
  else if (Number == 1)
  {
    leds[Verschiebung + 14] = Color;
    leds[Verschiebung + 16] = Color;
    leds[Verschiebung + 17] = Color;
    leds[Verschiebung + 18] = Color;
    leds[Verschiebung + 19] = Color;
    leds[Verschiebung + 20] = Color;
  }
  else if (Number == 2)
  {
    leds[Verschiebung + 0] = Color;
    leds[Verschiebung + 3] = Color;
    leds[Verschiebung + 4] = Color;
    leds[Verschiebung + 11] = Color;
    leds[Verschiebung + 13] = Color;
    leds[Verschiebung + 15] = Color;
    leds[Verschiebung + 16] = Color;
    leds[Verschiebung + 17] = Color;
    leds[Verschiebung + 18] = Color;
    leds[Verschiebung + 20] = Color;

  }
    else if (Number == 3)
  {
    leds[Verschiebung + 0] = Color;
    leds[Verschiebung + 2] = Color;
    leds[Verschiebung + 4] = Color;
    leds[Verschiebung + 11] = Color;
    leds[Verschiebung + 13] = Color;
    leds[Verschiebung + 15] = Color;
    leds[Verschiebung + 16] = Color;
    leds[Verschiebung + 17] = Color;
    leds[Verschiebung + 18] = Color;
    leds[Verschiebung + 19] = Color;
    leds[Verschiebung + 20] = Color;

  }
}
void ShowMiniNumber_Inverted(int Number, int Verschiebung, CRGB Color) //Platzverschiebung, da led Leiste von unten kommt
{
  for (int x = 0; x <= 23; x++) //Reset Leds
  {
    leds[Verschiebung + x] = CRGB::Black;
  }
  if (Number == 0)
  {
    leds[Verschiebung + 3] = Color;
    leds[Verschiebung + 4] = Color;
    leds[Verschiebung + 5] = Color;
    leds[Verschiebung + 6] = Color;
    leds[Verschiebung + 7] = Color;
    leds[Verschiebung + 8] = Color;
    leds[Verschiebung + 12] = Color;
    leds[Verschiebung + 19] = Color;
    leds[Verschiebung + 20] = Color;
    leds[Verschiebung + 21] = Color;
    leds[Verschiebung + 22] = Color;
    leds[Verschiebung + 23] = Color;
  }
  else if (Number == 1)
  {
    leds[Verschiebung + 9] = Color;
    leds[Verschiebung + 19] = Color;
    leds[Verschiebung + 20] = Color;
    leds[Verschiebung + 21] = Color;
    leds[Verschiebung + 22] = Color;
    leds[Verschiebung + 23] = Color;
  }
  else if (Number == 2)
  {
    leds[Verschiebung + 3] = Color;
    leds[Verschiebung + 4] = Color;
    leds[Verschiebung + 7] = Color;
    leds[Verschiebung + 8] = Color;
    leds[Verschiebung + 10] = Color;
    leds[Verschiebung + 12] = Color;
    leds[Verschiebung + 19] = Color;
    leds[Verschiebung + 21] = Color;
    leds[Verschiebung + 22] = Color;
    leds[Verschiebung + 23] = Color;

  }

    else if (Number == 3)
  {
    leds[Verschiebung + 3] = Color;
    leds[Verschiebung + 5] = Color;
    leds[Verschiebung + 7] = Color;
    leds[Verschiebung + 8] = Color;
    leds[Verschiebung + 10] = Color;
    leds[Verschiebung + 12] = Color;
    leds[Verschiebung + 19] = Color;
    leds[Verschiebung + 20] = Color;
    leds[Verschiebung + 21] = Color;
    leds[Verschiebung + 22] = Color;
    leds[Verschiebung + 23] = Color;

  }
  
}
void SetLeftLEDPad(int CompareVal, int Verschiebung, CRGB currentPalette)
{
  if (CompareVal & 1) //unten Rechts
  {
    leds[39 + Verschiebung] = currentPalette;
  }
  else
  {
    leds[39 + Verschiebung] = CRGB::Black;
  }
  if (CompareVal & 2) //unten
  {
    leds[23 + Verschiebung] = currentPalette;
    leds[24 + Verschiebung] = currentPalette;
  }
  else
  {
    leds[23 + Verschiebung] = CRGB::Black;
    leds[24 + Verschiebung] = CRGB::Black;
  }
  if (CompareVal & 4) //unten links
  {
    leds[8 + Verschiebung] = currentPalette;
  }
  else
  {
    leds[8 + Verschiebung] = CRGB::Black;
  }
  //Mitte
  if (CompareVal & 8)//Mitte links
  {
    leds[11 + Verschiebung] = currentPalette;
    leds[12 + Verschiebung] = currentPalette;
  }
  else
  {
    leds[11 + Verschiebung] = CRGB::Black;
    leds[12 + Verschiebung] = CRGB::Black;
  }


  if (CompareVal & 16)//Mitte
  {
    leds[19 + Verschiebung] = currentPalette;
    leds[20 + Verschiebung] = currentPalette;
    leds[27 + Verschiebung] = currentPalette;
    leds[28 + Verschiebung] = currentPalette;
  }
  else
  {
    leds[19 + Verschiebung] = CRGB::Black;
    leds[20 + Verschiebung] = CRGB::Black;
    leds[27 + Verschiebung] = CRGB::Black;
    leds[28 + Verschiebung] = CRGB::Black;
  }

  if (CompareVal & 32)//Mitte rechts
  {
    leds[35 + Verschiebung] = currentPalette;
    leds[36 + Verschiebung] = currentPalette;
  }
  else
  {
    leds[35 + Verschiebung] = CRGB::Black;
    leds[36 + Verschiebung] = CRGB::Black;
  }

  //oben
  if (CompareVal & 64)//links
  {
    leds[15 + Verschiebung] = currentPalette;
  }
  else
  {
    leds[15 + Verschiebung] = CRGB::Black;
  }



  if (CompareVal & 128)//Mitte
  {
    leds[16 + Verschiebung] = currentPalette;
    leds[31 + Verschiebung] = currentPalette;
  }
  else
  {
    leds[16 + Verschiebung] = CRGB::Black;
    leds[31 + Verschiebung] = CRGB::Black;
  }

  if (CompareVal & 256)// rechts
  {
    leds[32 + Verschiebung] = currentPalette;
  }
  else
  {
    leds[32 + Verschiebung] = CRGB::Black;
  }


  //Streifen links

  if (CompareVal & 512)// s_links_oben
  {
    leds[13 + Verschiebung] = currentPalette;
    leds[14 + Verschiebung] = currentPalette;
  }
  else
  {
    leds[13 + Verschiebung] = CRGB::Black;
    leds[14 + Verschiebung] = CRGB::Black;
  }

  if (CompareVal & 1024)//s_links_unten
  {
    leds[9 + Verschiebung] = currentPalette;
    leds[10 + Verschiebung] = currentPalette;
  }
  else
  {
    leds[9 + Verschiebung] = CRGB::Black;
    leds[10 + Verschiebung] = CRGB::Black;
  }

  //Streifen rechts


  if (CompareVal & 4096)//rechts unten
  {
    leds[37 + Verschiebung] = currentPalette;
    leds[38 + Verschiebung] = currentPalette;
  }
  else
  {
    leds[37 + Verschiebung] = CRGB::Black;
    leds[38 + Verschiebung] = CRGB::Black;
  }


  if (CompareVal & 2048)// s_rechts_oben
  {
    leds[33 + Verschiebung] = currentPalette;
    leds[34 + Verschiebung] = currentPalette;
  }
  else
  {
    leds[33 + Verschiebung] = CRGB::Black;
    leds[34 + Verschiebung] = CRGB::Black;
  }

  /*   *  8192 Special Pixels -1-
  *  16384 Special Pixels -7-
  *  
   */

  if (CompareVal & 8192)// 
  {
    leds[30 + Verschiebung] = currentPalette;
    leds[18 + Verschiebung] = currentPalette;
    leds[12 + Verschiebung] = currentPalette;
  }
  else
  {
    leds[30 + Verschiebung] = CRGB::Black;
    leds[18 + Verschiebung] = CRGB::Black;
  }
  return;
  //not implented
    if (CompareVal & 16384)// 16384 Special Pixels -7- overlapping with middle - never make em black
  {
   /* leds[9 + Verschiebung] = CRGB::Green;
    leds[8 + Verschiebung] = CRGB::Green;
    leds[20 + Verschiebung] = CRGB::Green;
    leds[21 + Verschiebung] = CRGB::Green;*/
    leds[19 + Verschiebung]= currentPalette;
    //leds[27 + Verschiebung] = currentPalette;
    leds[28 + Verschiebung] = currentPalette;
    //leds[19 + Verschiebung] = currentPalette;

    //leds[29 + Verschiebung] = CRGB::Red;
    //leds[30 + Verschiebung] = CRGB::Red;
    //leds[33 + Verschiebung] = CRGB::Red;
    //leds[34 + Verschiebung] = CRGB::Red;
  }
   
}
