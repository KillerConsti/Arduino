#pragma once
#define RADIO 2
//do you wanna mirror leds
#define Mirror_LEDS
//either definec:\Users\49151\Documents\Arduino\beachfreunde_anzeige_mit_runddisplay_gc901a_copy_no_blink\devices\led_anzeige.h use esp32 or it will use the uno
//this set USE ESP32 when we dont detect an uno board
#ifndef ARDUINO_UNOWIFIR4
#define USE_ESP32
#endif
#define SwapColor 

//Thomas Lösung
  //Rotation tft.setRotation(2) -> geblockt
#define DisplayRotation180
  //7+8 und 5+6 getauscht
#define SwapWiringButtons


//Auswahl Display (rund oder 2" eckig)
#define UseRoundDisplay


/**************************Technische Sachen******************/
//Display Pinout

#ifdef USE_ESP32
#include <C:\Users\49151\AppData\Local\Arduino15\packages\esp32\hardware\esp32\2.0.11\libraries\SPI\src\SPI.h>
/*SPI PINS for Reference
    SCK (SCL)	13 (fest)
    SDA /MOSI	11 (fest)
    MISO	12 (fest)*/
#define TFT_DC 16
#define TFT_CS 22
#define TFT_RS 4
#else /* UNO */
#include "SPI.h"
#define TFT_DC 14
#define TFT_CS 15
#define TFT_RS 16
#endif
#include <stdint.h> 

//Colors for display
const uint16_t KC_red = 63488;
const uint16_t KC_grey = 2113;
const uint16_t KC_dirty_white = 61309;
const uint16_t KC_DARKGREY = 31727;
const uint16_t KC_dark_blue = 53;


#if RADIO == 1
const static uint16_t KC_BackgroundColor = 3697;
#endif
#if RADIO == 2
const static uint16_t KC_BackgroundColor = 61521;
#endif
#if RADIO == 3
const static uint16_t KC_BackgroundColor = 53;
#endif


typedef struct
{
  uint8_t Team1;
  uint8_t Team2;
  uint8_t Satz;
  uint8_t Winner;  //0 is no winner 1 -> T1 ; 2 ->t2;
  bool Set = false;
} VolleyBallHistory;
