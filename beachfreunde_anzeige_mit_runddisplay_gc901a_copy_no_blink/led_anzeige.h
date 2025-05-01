#pragma once
#include <FastLED.h>

#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS_lib 288

#define FRAMES_PER_SECOND 120

class LED_MATRIX {
public:
//if pin2 == -1 it will not be assigned
LED_MATRIX();
void Setup(int DataPin1,bool Mirror_Board, int DataPin2 = -1);
void Set_Brightness(int Helligkeit);
void SetNewNumberLeft(int leftscore, int currentpaletteindex);
void SetNewNumberRight(int rightscore, int currentpaletteindex);
void SetNewNumberMiddle(int leftscore,int rightscore, int currentpaletteindex);
int TranslateNumbers(int Input);
void SetLeftLEDPad(int CompareVal, int Verschiebung, CRGB currentPalette);
void ShowMiniNumber_Inverted(int Number, int Verschiebung, CRGB Color);
void ShowMiniNumber(int Number, int Verschiebung, CRGB Color);
void Show();
void DrawMatrixSign(int i);
bool mMirrored;
//Party stuff
void FadeToBlack();
void DoParty();
void FadeBlack();

uint8_t gCurrentPatternNumber;  // Index number of which pattern is current
uint8_t gHue;
void nextPattern();
  void rainbow();
  void rainbowWithGlitter();
  void addGlitter(fract8 chanceOfGlitter);
  void confetti();
  void sinelon();
  void bpm();
  void juggle(); 
};
