#include "tm1638.h"
#include <TM1638plus_Model2.h>


tm1638::tm1638()
{       
  tm =new TM1638plus_Model2(17,19,18,false,false);
  tm->displayBegin();
  m_ButtonTimeReset = 0;
  m_ButtonTimeSwap = 0;
  IgnoreUpdateLeftBecauseSwap = false;
  IgnoreUpdateRightBecauseSwap = false;
}

void tm1638::SetNewScore(int leftscore,int rightscore,int setsl,int setsr)
{
  char text[7];
  BuildString(leftscore,rightscore,setsl,setsr,text);
  tm->DisplayStr(text,16);
}
void tm1638::BuildString(int PointsLeft,int PointsRight,int SetsLeft,int SetsRight,char *text)
{
  char Buffer[2];
  itoa(PointsLeft, Buffer, 10);
  if(PointsLeft < 10)
  {
    text[0] = '0';
    text[1] = Buffer[0]; 
  }
  else
  {
    text[0] = Buffer[0];
    text[1] = Buffer[1];  
  }
  text[2] = ' ';
  itoa(SetsLeft, Buffer, 10);
  text[3] = Buffer[0];

  //Right Side
  itoa(PointsRight, Buffer, 10);
  if(PointsRight < 10)
  {
    text[6] ='0';
    text[7] = Buffer[0]; 
  }
  else
  {
    text[6] = Buffer[0];
    text[7] = Buffer[1];  
  }
  text[5] = ' ';
  itoa(SetsRight, Buffer, 10);
  text[4] = Buffer[0];

}

int tm1638::EventKeyTriggered(int ModifyButtonState)
{
  int returnvalue = 0;
  bool AnythingChanged = false;
  uint16_t i = tm->ReadKey16Two();

  /*  unten rechts */
  if (DownRightButton.pressed != GetDownRightButtonState(i)) {
    DownRightButton.pressed = GetDownRightButtonState(i);
    Serial.print("b_unten_rechts FB|");
    Serial.println(DownRightButton.pressed);
    if (DownRightButton.ButtonTime + 200 < millis() && DownRightButton.pressed == false) {
      AnythingChanged = true;
      DownRightButton.ButtonTime = millis();
      if (ModifyButtonState != 2)
      {
          returnvalue = returnvalue+1;
        //right_nmber_value = constrain(right_nmber_value - 1, 0, 99);
        //WhatIsTheReason.ScoreChanged = true;
      }
      else {
        returnvalue = returnvalue+2;
        //WhatIsTheReason.SetsChanged = true;
        //WhatIsTheReason.UpdateHistoryOnDisplay = true;
        //right_set_value = constrain(right_set_value - 1, 0, 3);
      }
    }
  }
  // down left
    if (DownLeftButton.pressed != GetDownLeftButtonState(i)) {
    DownLeftButton.pressed = GetDownLeftButtonState(i);
    Serial.print("b_unten_links FB|");
    Serial.println(DownRightButton.pressed);
    if (DownRightButton.ButtonTime + 200 < millis() && DownLeftButton.pressed == false) {
      AnythingChanged = true;
      DownLeftButton.ButtonTime = millis();
      if (ModifyButtonState != 2)
      {
          returnvalue = returnvalue+4;
        //right_nmber_value = constrain(right_nmber_value - 1, 0, 99);
        //WhatIsTheReason.ScoreChanged = true;
      }
      else {
        returnvalue = returnvalue+8;
        //WhatIsTheReason.SetsChanged = true;
        //WhatIsTheReason.UpdateHistoryOnDisplay = true;
        //right_set_value = constrain(right_set_value - 1, 0, 3);
      }
    }
  }

  //top right
   if (TopRightButton.pressed != GetTopRightButtonState(i)) {
    TopRightButton.pressed = GetTopRightButtonState(i);
    Serial.print("b_oben rechts FB|");
    Serial.println(DownRightButton.pressed);
    if (TopRightButton.ButtonTime + 200 < millis() && TopRightButton.pressed == false) {
      AnythingChanged = true;
      TopRightButton.ButtonTime = millis();
            if (IgnoreUpdateRightBecauseSwap)
        IgnoreUpdateRightBecauseSwap = false;
      else if (ModifyButtonState != 2)
      {
          returnvalue = returnvalue+16;
        //right_nmber_value = constrain(right_nmber_value - 1, 0, 99);
        //WhatIsTheReason.ScoreChanged = true;
      }
      else {
        returnvalue = returnvalue+32;
        //WhatIsTheReason.SetsChanged = true;
        //WhatIsTheReason.UpdateHistoryOnDisplay = true;
        //right_set_value = constrain(right_set_value - 1, 0, 3);
      }
    }
  }
  //top left
     if (TopLeftButton.pressed != GetTopLeftButtonState(i)) {
    TopLeftButton.pressed = GetTopLeftButtonState(i);
    Serial.print("b_oben_links FB|");
    Serial.println(DownRightButton.pressed);
    if (TopLeftButton.ButtonTime + 200 < millis() && TopLeftButton.pressed == false) {
      AnythingChanged = true;
      TopLeftButton.ButtonTime = millis();
            if (IgnoreUpdateLeftBecauseSwap)
        IgnoreUpdateLeftBecauseSwap = false;
      else if (ModifyButtonState != 2)
      {
          returnvalue = returnvalue+64;
        //right_nmber_value = constrain(right_nmber_value - 1, 0, 99);
        //WhatIsTheReason.ScoreChanged = true;
      }
      else {
        returnvalue = returnvalue+128;
        //WhatIsTheReason.SetsChanged = true;
        //WhatIsTheReason.UpdateHistoryOnDisplay = true;
        //right_set_value = constrain(right_set_value - 1, 0, 3);
      }
    }
  }

  //two button pressed

  /*save and reset*/
  if (!DownRightButton.pressed || !DownLeftButton.pressed)  //safety allow it only all 10 secs
  {
    m_ButtonTimeReset = millis();  //get current time
  }
  if (DownRightButton.pressed || DownLeftButton.pressed)  //reset funktion - hold both lower buttons for 3 secs
  {
    if (m_ButtonTimeReset + 1500 < millis())  //reset after pressing all buttons 2 secs
    {
      
        m_ButtonTimeReset = millis() + 100000;  //set it increadible high so it wont be triggered again if buttons are not released (wait 100 secs)
        
        Serial.println("FB Reset");
        returnvalue = returnvalue+256;
      }
    }

  /*swap places fct (press both up buttons for 2 secs)
    /must tell up-buttons to not increase so points stay the same
    */
  if (TopRightButton.pressed && TopLeftButton.pressed)  //reset funktion
  {
    if (millis() > m_ButtonTimeSwap + 10000)  //safety allow it only all 10 secs
    {
      m_ButtonTimeSwap = millis();  //get current time
    } else {
      if (m_ButtonTimeSwap + 2000 < millis())  //swap after pressing all buttons 3 secs
      {
        IgnoreUpdateLeftBecauseSwap = true;
        IgnoreUpdateRightBecauseSwap = true;
        returnvalue = returnvalue+512;
        m_ButtonTimeSwap = millis() + 100000;  //set it increadible high so it wont be triggered again if buttons are not released (wait 100 secs)
      }
    }
  } else
    m_ButtonTimeSwap = 0;
  return returnvalue;
}
 bool tm1638::GetTopRightButtonState(uint16_t i)
 {
    return (i & 2048); //2048..8
 }
 bool tm1638::GetTopLeftButtonState(uint16_t i)
 {
    return (i & 1);
 }
 bool tm1638::GetDownRightButtonState(uint16_t i)
 {
    return (i & 32768); 
 }
 bool tm1638::GetDownLeftButtonState(uint16_t i)
 {
    return (i & 16); //4096..16
 }
