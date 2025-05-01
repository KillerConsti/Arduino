#include "led_anzeige.h"
#include <FastLED.h>

FASTLED_USING_NAMESPACE
CRGB leds[NUM_LEDS_lib];

LED_MATRIX::LED_MATRIX()
{
  gCurrentPatternNumber = 0;  // Index number of which pattern is current
  gHue = 0;        
  
}
void LED_MATRIX::Setup(int DataPin1,bool Mirror_Board, int DataPin2)
{
  //we do different datapins here
  #ifndef ARDUINO_UNOWIFIR4
  FastLED.addLeds<LED_TYPE, 3, COLOR_ORDER>(leds, NUM_LEDS_lib).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, 21, COLOR_ORDER>(leds, NUM_LEDS_lib).setCorrection(TypicalLEDStrip);
  #else
  FastLED.addLeds<LED_TYPE, 0, COLOR_ORDER>(leds, NUM_LEDS_lib).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, 1, COLOR_ORDER>(leds, NUM_LEDS_lib).setCorrection(TypicalLEDStrip);
  #endif
  mMirrored =Mirror_Board;
}
void LED_MATRIX::Set_Brightness(int Helligkeit)
{
  FastLED.setBrightness(Helligkeit);
}

void LED_MATRIX::Show()
{
  FastLED.show();
}

void LED_MATRIX::SetNewNumberLeft(int leftscore, int currentpaletteindex)
{
  int lastdigit = leftscore % 10;
  int firstdigit = (leftscore - lastdigit) / 10;
  if(!mMirrored)
  {
    SetLeftLEDPad(TranslateNumbers(firstdigit), 0, ColorFromPalette(RainbowColors_p, currentpaletteindex));
    SetLeftLEDPad(TranslateNumbers(lastdigit), 48, ColorFromPalette(RainbowColors_p, currentpaletteindex));
  }
  else
  {
    SetLeftLEDPad(TranslateNumbers(firstdigit), 48 + 96 + 16, ColorFromPalette(RainbowColors_p, currentpaletteindex));
    SetLeftLEDPad(TranslateNumbers(lastdigit), 96 + 96 + 16, ColorFromPalette(RainbowColors_p, currentpaletteindex));
  }
}
void LED_MATRIX::SetNewNumberRight(int rightscore, int currentpaletteindex)
{
  int lastdigit = rightscore % 10;
  int firstdigit = (rightscore - lastdigit) / 10;
  if(!mMirrored)
  {
    SetLeftLEDPad(TranslateNumbers(firstdigit), 48 + 96 + 16, ColorFromPalette(RainbowColors_p, currentpaletteindex));
    SetLeftLEDPad(TranslateNumbers(lastdigit), 96 + 96 + 16, ColorFromPalette(RainbowColors_p, currentpaletteindex));
  }
  else
  {
    SetLeftLEDPad(TranslateNumbers(firstdigit), 0, ColorFromPalette(RainbowColors_p, currentpaletteindex));
    SetLeftLEDPad(TranslateNumbers(lastdigit), 48, ColorFromPalette(RainbowColors_p, currentpaletteindex));
  }
}

void LED_MATRIX::SetNewNumberMiddle(int leftscore,int rightscore, int currentpaletteindex)
{
      ShowMiniNumber(mMirrored ? rightscore : leftscore, 96, ColorFromPalette(RainbowColors_p, currentpaletteindex));
      ShowMiniNumber_Inverted(mMirrored ? leftscore : rightscore, 96 + 40, ColorFromPalette(RainbowColors_p, currentpaletteindex));
}

int LED_MATRIX::TranslateNumbers(int Input) {
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
  if (OldStyle) {
    if (Input == 1)
      return (1 + 32 + 256 + 2048 + 4096 + 8192);
    if (Input == 2)
      return (1 + 2 + 4 + 8 + 16 + 32 + 64 + 128 + 256 + 1024 + 2048);
    if (Input == 3)
      return (1 + 2 + 4 + 8 + 16 + 32 + 64 + 128 + 256 + 2048 + 4096);
    if (Input == 4)
      return (1 + 8 + 16 + 32 + 64 + 256 + 512 + 2048 + 4096);
    if (Input == 5)
      return (1 + 2 + 4 + 8 + 16 + 32 + 64 + 128 + 256 + 4096 + 512);
    if (Input == 6)
      return (1 + 2 + 4 + 8 + 16 + 32 + 64 + 128 + 256 + 512 + 1024 + 4096);
    if (Input == 7)
      //return(1 + 32 +64+ 128 + 256 + 2048 + 4096+16384);
      return (1 + 32 + 64 + 128 + 256 + 2048 + 4096 /*+16384*/);
    if (Input == 8)
      return (1 + 2 + 4 + 8 + 16 + 32 + 64 + 128 + 256 + 512 + 1024 + 2048 + 4096);
    if (Input == 9)
      return (1 + 2 + 4 + 8 + 16 + 32 + 64 + 128 + 256 + 512 + 2048 + 4096);

    if (Input == 0)
      return (1 + 2 + 4 + 8 + 32 + 64 + 128 + 256 + 512 + 1024 + 2048 + 4096);
  } else {
    if (Input == 1)
      return (1 + 32 + 256 + 2048 + 4096);
    if (Input == 2)
      return (1 + 2 + 4 + 8 + 16 + 32 + 64 + 128 + 256 + 1024 + 2048);
    if (Input == 3)
      return (1 + 2 + 4 + 8 + 16 + 32 + 64 + 128 + 256 + 2048 + 4096);
    if (Input == 4)
      return (1 + 8 + 16 + 32 + 64 + 256 + 512 + 2048 + 4096);
    if (Input == 5)
      return (1 + 2 + 4 + 8 + 16 + 32 + 64 + 128 + 256 + 4096 + 512);
    if (Input == 6)
      return (1 + 2 + 4 + 8 + 16 + 32 + 64 + 128 + 256 + 512 + 1024 + 4096);
    if (Input == 7)
      return (1 + 32 + 128 + 256 + 2048 + 4096);
    if (Input == 8)
      return (2 + 16 + 128 + 512 + 1024 + 2048 + 4096);
    if (Input == 9)
      return (1 + 2 + 4 + 8 + 16 + 32 + 64 + 128 + 256 + 512 + 2048 + 4096);

    if (Input == 0)
      return (2 + 8 + 32 + 128 + 512 + 1024 + 2048 + 4096);
  }
  return (0);
}

void LED_MATRIX::ShowMiniNumber(int Number, int Verschiebung, CRGB Color) {
  for (int x = 0; x <= 20; x++)  //Reset Leds
  {
    leds[Verschiebung + x] = CRGB::Black;
  }
  //Panel = 3* 5
  if (Number == 0) {
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
  } else if (Number == 1) {
    leds[Verschiebung + 14] = Color;
    leds[Verschiebung + 16] = Color;
    leds[Verschiebung + 17] = Color;
    leds[Verschiebung + 18] = Color;
    leds[Verschiebung + 19] = Color;
    leds[Verschiebung + 20] = Color;
  } else if (Number == 2) {
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

  } else if (Number == 3) {
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
void LED_MATRIX::ShowMiniNumber_Inverted(int Number, int Verschiebung, CRGB Color)  //Platzverschiebung, da led Leiste von unten kommt
{
  for (int x = 0; x <= 23; x++)  //Reset Leds
  {
    leds[Verschiebung + x] = CRGB::Black;
  }
  if (Number == 0) {
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
  } else if (Number == 1) {
    leds[Verschiebung + 9] = Color;
    leds[Verschiebung + 19] = Color;
    leds[Verschiebung + 20] = Color;
    leds[Verschiebung + 21] = Color;
    leds[Verschiebung + 22] = Color;
    leds[Verschiebung + 23] = Color;
  } else if (Number == 2) {
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

  else if (Number == 3) {
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
void LED_MATRIX::SetLeftLEDPad(int CompareVal, int Verschiebung, CRGB currentPalette) {
  if (CompareVal & 1)  //unten Rechts
  {
    leds[39 + Verschiebung] = currentPalette;
  } else {
    leds[39 + Verschiebung] = CRGB::Black;
  }
  if (CompareVal & 2)  //unten
  {
    leds[23 + Verschiebung] = currentPalette;
    leds[24 + Verschiebung] = currentPalette;
  } else {
    leds[23 + Verschiebung] = CRGB::Black;
    leds[24 + Verschiebung] = CRGB::Black;
  }
  if (CompareVal & 4)  //unten links
  {
    leds[8 + Verschiebung] = currentPalette;
  } else {
    leds[8 + Verschiebung] = CRGB::Black;
  }
  //Mitte
  if (CompareVal & 8)  //Mitte links
  {
    leds[11 + Verschiebung] = currentPalette;
    leds[12 + Verschiebung] = currentPalette;
  } else {
    leds[11 + Verschiebung] = CRGB::Black;
    leds[12 + Verschiebung] = CRGB::Black;
  }


  if (CompareVal & 16)  //Mitte
  {
    leds[19 + Verschiebung] = currentPalette;
    leds[20 + Verschiebung] = currentPalette;
    leds[27 + Verschiebung] = currentPalette;
    leds[28 + Verschiebung] = currentPalette;
  } else {
    leds[19 + Verschiebung] = CRGB::Black;
    leds[20 + Verschiebung] = CRGB::Black;
    leds[27 + Verschiebung] = CRGB::Black;
    leds[28 + Verschiebung] = CRGB::Black;
  }

  if (CompareVal & 32)  //Mitte rechts
  {
    leds[35 + Verschiebung] = currentPalette;
    leds[36 + Verschiebung] = currentPalette;
  } else {
    leds[35 + Verschiebung] = CRGB::Black;
    leds[36 + Verschiebung] = CRGB::Black;
  }

  //oben
  if (CompareVal & 64)  //links
  {
    leds[15 + Verschiebung] = currentPalette;
  } else {
    leds[15 + Verschiebung] = CRGB::Black;
  }



  if (CompareVal & 128)  //Mitte
  {
    leds[16 + Verschiebung] = currentPalette;
    leds[31 + Verschiebung] = currentPalette;
  } else {
    leds[16 + Verschiebung] = CRGB::Black;
    leds[31 + Verschiebung] = CRGB::Black;
  }

  if (CompareVal & 256)  // rechts
  {
    leds[32 + Verschiebung] = currentPalette;
  } else {
    leds[32 + Verschiebung] = CRGB::Black;
  }


  //Streifen links

  if (CompareVal & 512)  // s_links_oben
  {
    leds[13 + Verschiebung] = currentPalette;
    leds[14 + Verschiebung] = currentPalette;
  } else {
    leds[13 + Verschiebung] = CRGB::Black;
    leds[14 + Verschiebung] = CRGB::Black;
  }

  if (CompareVal & 1024)  //s_links_unten
  {
    leds[9 + Verschiebung] = currentPalette;
    leds[10 + Verschiebung] = currentPalette;
  } else {
    leds[9 + Verschiebung] = CRGB::Black;
    leds[10 + Verschiebung] = CRGB::Black;
  }

  //Streifen rechts


  if (CompareVal & 4096)  //rechts unten
  {
    leds[37 + Verschiebung] = currentPalette;
    leds[38 + Verschiebung] = currentPalette;
  } else {
    leds[37 + Verschiebung] = CRGB::Black;
    leds[38 + Verschiebung] = CRGB::Black;
  }


  if (CompareVal & 2048)  // s_rechts_oben
  {
    leds[33 + Verschiebung] = currentPalette;
    leds[34 + Verschiebung] = currentPalette;
  } else {
    leds[33 + Verschiebung] = CRGB::Black;
    leds[34 + Verschiebung] = CRGB::Black;
  }

  /*   *  8192 Special Pixels -1-
  *  16384 Special Pixels -7-
  *  
   */

  if (CompareVal & 8192)  //
  {
    leds[30 + Verschiebung] = currentPalette;
    leds[18 + Verschiebung] = currentPalette;
    leds[12 + Verschiebung] = currentPalette;
  } else {
    leds[30 + Verschiebung] = CRGB::Black;
    leds[18 + Verschiebung] = CRGB::Black;
  }
  return;
  //not implented
  if (CompareVal & 16384)  // 16384 Special Pixels -7- overlapping with middle - never make em black
  {
    /* leds[9 + Verschiebung] = CRGB::Green;
    leds[8 + Verschiebung] = CRGB::Green;
    leds[20 + Verschiebung] = CRGB::Green;
    leds[21 + Verschiebung] = CRGB::Green;*/
    leds[19 + Verschiebung] = currentPalette;
    //leds[27 + Verschiebung] = currentPalette;
    leds[28 + Verschiebung] = currentPalette;
    //leds[19 + Verschiebung] = currentPalette;

    //leds[29 + Verschiebung] = CRGB::Red;
    //leds[30 + Verschiebung] = CRGB::Red;
    //leds[33 + Verschiebung] = CRGB::Red;
    //leds[34 + Verschiebung] = CRGB::Red;
  }
}

void LED_MATRIX::DrawMatrixSign(int i) {
  Serial.print("DrawMatrix ");
  Serial.println(i);
  if (i == 0) {
    for (int t = 0; t < 3; t++) {
      leds[t + 117] = CRGB::Black;
      leds[t + 133] = CRGB::Black;
    }
    leds[121] = CRGB::Black;
  } else if (i == 1) {
    for (int t = 0; t < 8; t++) {
      leds[t] = CRGB::White;
      leds[t + 88] = CRGB::White;
    }
  } else if (i == 2) {
    for (int t = 0; t < 8; t++) {
      leds[t] = CRGB::Black;
      leds[t + 88] = CRGB::Black;
    }
    for (int t = 0; t < 5; t++) {
      leds[t + 160] = CRGB::White;
      leds[t + 88 + 3] = CRGB::White;
    }
  } else if (i == 3) {
    for (int t = 0; t < 8; t++) {
      leds[160 + t] = CRGB::White;
      leds[248 + t] = CRGB::White;
    }
    for (int t = 0; t < 5; t++) {
      leds[t + 88 + 3] = CRGB::Black;
    }
  } else if (i == 4) {
    for (int t = 0; t < 8; t++) {
      leds[160 + t] = CRGB::Black;
      leds[248 + t] = CRGB::Black;
    }
    for (int t = 0; t < 3; t++) {
      leds[t + 117] = CRGB::White;
      leds[t + 133] = CRGB::White;
    }
    leds[121] = CRGB::White;
  }
}

//Party

void LED_MATRIX::DoParty() {
  // Call the current pattern function once, updating the 'leds' array
  switch (gCurrentPatternNumber)
  {
  case 0:
  {
      LED_MATRIX::rainbow();
      break;
  }
  case 1:
  {
    LED_MATRIX::rainbowWithGlitter();
    break;
  }
  case 2: 
  {
    LED_MATRIX::confetti();
    break;
  }
    case 3: 
  {
    LED_MATRIX::sinelon();
    break;
  }
    case 4: 
  {
    LED_MATRIX::bpm();
    break;
  }
    case 5: 
  {
    LED_MATRIX::juggle();
    break;
  }
  default:
  {
    break;
  }
  }

  // send the 'leds' array out to the actual LED strip
  FastLED.show();
  // insert a delay to keep the framerate modest
  FastLED.delay(1000 / FRAMES_PER_SECOND);

  // do some periodic updates
  EVERY_N_MILLISECONDS(20) {
    gHue++;
  }  // slowly cycle the "base color" through the rainbow
  EVERY_N_SECONDS(10) {
    nextPattern();
  }  // change patterns periodically
}

void LED_MATRIX::FadeBlack()
{
  for (int i = 0; i < 256; i++)
        leds[i] = CRGB::Black;
      FastLED.show();
}
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void LED_MATRIX::nextPattern() {
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % 6;
}

void LED_MATRIX::rainbow() {
  // FastLED's built-in rainbow generator
  fill_rainbow(leds, NUM_LEDS_lib, gHue, 7);
}

void LED_MATRIX::rainbowWithGlitter() {
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void LED_MATRIX::addGlitter(fract8 chanceOfGlitter) {
  if (random8() < chanceOfGlitter) {
    leds[random16(NUM_LEDS_lib)] += CRGB::White;
  }
}

void LED_MATRIX::confetti() {
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy(leds, NUM_LEDS_lib, 10);
  int pos = random16(NUM_LEDS_lib);
  leds[pos] += CHSV(gHue + random8(64), 200, 255);
}

void LED_MATRIX::sinelon() {
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy(leds, NUM_LEDS_lib, 20);
  int pos = beatsin16(13, 0, NUM_LEDS_lib - 1);
  leds[pos] += CHSV(gHue, 255, 192);
}

void LED_MATRIX::bpm() {
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
  for (int i = 0; i < NUM_LEDS_lib; i++) {  //9948
    leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
  }
}

void LED_MATRIX::juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy(leds, NUM_LEDS_lib, 20);
  uint8_t dothue = 0;
  for (int i = 0; i < 8; i++) {
    leds[beatsin16(i + 7, 0, NUM_LEDS_lib - 1)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}