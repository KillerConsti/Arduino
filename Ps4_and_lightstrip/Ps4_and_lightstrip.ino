
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


#define DATA_PIN    1
//#define CLK_PIN   4
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    10
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120
int index3;
CRGB currentPalette;
void setup()
{
 Serial.begin(9600);
  Serial.println("Starting Arduino BLE Client application...");
  index3 =0;
currentPalette = CRGB::Green;
  //FastLED
    delay(3000); // 3 second delay for recovery
  Serial.println("init led ..");
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
  Serial.println("finished");
}

void loop()
{
  index3++;
  if(index3 == NUM_LEDS)
  {
    index3 = 0;
    //leds[NUM_LEDS-1] = CRGB::Black;
  }
  else
  {
    //leds[index3-1] = CRGB::Black;
  }
  leds[index3] = CRGB::Green;
  FastLED.show();
  delay(50);
  // Now turn the LED off, then pause
  Serial.print("X is");
  Serial.println(index3); 
}
