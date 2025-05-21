#pragma once
#include "settings.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ST7789.h" // Hardware-specific library for ST7789
#include "..\..\libraries\Adafruit_GFX_Library\Fonts\FreeSansBold12pt7b.h"
#include "..\..\libraries\Adafruit_GFX_Library\Fonts\FreeSansBold24pt7b.h"
#include "..\..\libraries\Adafruit_GFX_Library\Fonts\FreeSansBold18pt7b.h"


class SquareDisplay {
public:
SquareDisplay();
void InitDisplay();
void DrawFox();
void DrawVolleyball();
void DrawOnDisplay(VolleyBallHistory History[10],int leftset,int rightset,int leftscore,int rightscore,bool NeedToUpdateSet,bool &DoNotUpdateHistoryOnDisplay);
void UpdateSetCounter(int leftnum,int rightnum);
bool UpdateHistory(VolleyBallHistory History[10]); //todo upgrade clear progress to be done not too often
void UpdateScore(int left,int right);
bool ClearAndUpdateHistory();
void DrawFarbe(bool rechts);
void DrawBeachfreunde(bool draw);
void DrawBrightness();
void DrawSets();
void SetTeam1();
void SetTeam2();
void SetBrightness(int mNewBrightness);
void SetModifyButtonState(int MBS);
private:
int ConvertIntToStringWithDoubleDigit(char* array, int num);
int mBrightness;
int OldNumSets;
int mOldModifyButtonState;
int ModifyButtonState;
char TeamName1[50];
int SetCounterLeft;
int SetCounterRight;
  GFXcanvas1* canvas;
  //(56, 42);         // 128x32 pixel canvas
GFXcanvas1* canvas_single;
//(32, 36);  // 128x32 pixel canvas
GFXcanvas1* canvas_history;
//(105, 20);
GFXcanvas1* canvas_beachfreunde;//(165, 20);
GFXcanvas1*  canvas_team1;
Adafruit_ST7789* tft;
int counter;
enum BottomText
{
    Beachfreunde,
    Farbe_links,
    Satzverhaeltnis,
    Farbe_rechts,
    Helligkeit,
    Reset

};
BottomText mBottomtext;
bool m_lines_allready_erased;
};