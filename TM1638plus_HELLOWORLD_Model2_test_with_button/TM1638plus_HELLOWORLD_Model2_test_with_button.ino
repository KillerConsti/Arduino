/*!
	@file     TM1638plus_HELLOWORLD_Model2.ino
	@author   Gavin Lyons
	@brief 
		A Basic test file for model 2, TM1638 module(16 KEY 16 pushbuttons).
		Carries out series most basic test , "hello world" helo wrld
*/

#include <TM1638plus_Model2.h>

// GPIO I/O pins on the Arduino connected to strobe, clock, data, pick on any I/O pin you want.
#define  STROBE_TM 5  // strobe = GPIO connected to strobe line of module
#define  CLOCK_TM 22  // clock = GPIO connected to clock line of module
#define  DIO_TM 21 // data = GPIO connected to data line of module
bool swap_nibbles = false; //Default is false if left out, see issues section in readme at URL
bool high_freq = false; //default false, If using a high freq CPU > ~100 MHZ set to true. 
int counter = -1;

int points_left;
int points_right;
// Constructor object Init the module
TM1638plus_Model2 tm(STROBE_TM, CLOCK_TM , DIO_TM, swap_nibbles, high_freq);


void setup() {
  Serial.begin(9600);
 tm.displayBegin();
 UpdateDisplay();
}

void BuildString(int PointsLeft,int PointsRight,int SetsLeft,int SetsRight,char *text)
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
void loop() {
  if(counter == tm.ReadKey16Two())
  return;
  if(tm.ReadKey16Two() == 0) //execute at button up
  {
    onPressButton();
    counter = tm.ReadKey16Two();
  }
  else
  {
    //start timer
  }
  counter = tm.ReadKey16Two();
  onPressButton();
	//tm.DisplayDecNum(counter,0);  // Display "helowrld" in seven segments with zero decimal points set.

	delay(100);
}

void onPressButton()
{
  switch (counter)
  {
    case 1:
    T1Point(1);
    return;
    case 4096:
    T1Point(-1);
    return;
    case 8:
    T2Point(1);
    return;
    case 32768:
    T2Point(-1);
    return;
  }
}

void T1Point(int point)
{
  if(point > 0)
  {
    points_left += point;
  }
  else
  {
    points_left += point;
    if(points_left < 0)
    {
      points_left = 0;
    }
  }
  UpdateDisplay();
}

void T2Point(int point)
{
  if(point > 0)
  {
    points_right += point;
  }
  else
  {
    points_right += point;
    if(points_right < 0)
    {
      points_right = 0;
    }
  }
  UpdateDisplay();
}

void UpdateDisplay()
{
  char MyText[8];
  BuildString(points_left,points_right,0,0,MyText);
  tm.DisplayStr(MyText,0);
  Serial.print(MyText);
}
