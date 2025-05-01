#pragma once
#include <TM1638plus_Model2.h>


class tm1638 {
public:
//if pin2 == -1 it will not be assigned
tm1638();
TM1638plus_Model2* tm;
void SetNewScore(int leftscore,int rightscore,int setsl,int setsr);
int EventKeyTriggered(int ModifyButtonState);
void BuildString(int PointsLeft,int PointsRight,int SetsLeft,int SetsRight,char *text);
 struct ButtonData
 {
    bool pressed = false;
    uint32_t ButtonTime =0;
 };
  ButtonData TopLeftButton;
  ButtonData TopRightButton;
  ButtonData DownLeftButton;
  ButtonData DownRightButton;
 private:
 bool GetTopRightButtonState(uint16_t i);
 bool GetTopLeftButtonState(uint16_t i);
 bool GetDownRightButtonState(uint16_t i);
 bool GetDownLeftButtonState(uint16_t i);
 uint64_t m_ButtonTimeReset;
 uint64_t m_ButtonTimeSwap;
 bool IgnoreUpdateLeftBecauseSwap;
bool IgnoreUpdateRightBecauseSwap;
};
