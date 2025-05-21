#pragma once
#include "settings.h"
#include "Adafruit_GFX.h"
#include "Adafruit_GC9A01A.h"
#include "../../libraries\Adafruit_GFX_Library\Fonts\FreeSansBold12pt7b.h"
#include "../../Adafruit_GFX_Library\Fonts\FreeSansBold24pt7b.h"
#include "../../libraries\Adafruit_GFX_Library\Fonts\FreeSansBold18pt7b.h"




class RoundDisplay {
public:
RoundDisplay();
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

void SetBrightness(int mNewBrightness);
void SetModifyButtonState(int MBS);
private:
int ConvertIntToStringWithDoubleDigit(char* array, int num);
int mBrightness;
int OldNumSets;
int mOldModifyButtonState;
int ModifyButtonState;
  GFXcanvas1* canvas;
  //(56, 42);         // 128x32 pixel canvas
GFXcanvas1* canvas_single;
//(32, 36);  // 128x32 pixel canvas
GFXcanvas1* canvas_history;
//(105, 20);
GFXcanvas1* canvas_beachfreunde;//(165, 20);

Adafruit_GC9A01A* tft;
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