
 #include <PS4Controller.h>

uint8_t previousVal = 0;
int ledChannel = 0;

 void onEvent(){
                                        Serial.println("Up Button");
                        if ( PS4.data.button.down )
                                        Serial.println("Down Button");
                        if ( PS4.data.button.left )
                                        Serial.println("Left Button");
                        if ( PS4.data.button.right )
                                        Serial.println("Right Button");

                        if ( PS4.data.button.upright )
                                        Serial.println("Up Right");
                        if ( PS4.data.button.upleft )
                                        Serial.println("Up Left");
                        if ( PS4.data.button.downleft )
                                        Serial.println("Down Left");
                        if ( PS4.data.button.downright )
                                        Serial.println("Down Right");

                        if ( PS4.data.button.triangle )
                                        Serial.println("Triangle Button");
                        if ( PS4.data.button.circle )
                                        Serial.println("Circle Button");
                        if ( PS4.data.button.cross )
                                        Serial.println("Cross Button");
                        if ( PS4.data.button.square )
                                        Serial.println("Square Button");

                        if ( PS4.data.button.l1 )
                                        Serial.println("l1 Button");
                        if ( PS4.data.button.r1 )
                                        Serial.println("r1 Button");

                        if ( PS4.data.button.l3 )
                                        Serial.println("l3 Button");
                        if ( PS4.data.button.r3 )
                                        Serial.println("r3 Button");

                        if ( PS4.data.button.share )
                                        Serial.println("Share Button");
                        if ( PS4.data.button.options )
                                        Serial.println("Options Button");

                        if ( PS4.data.button.ps )
                                        Serial.println("PS Button");
                        if ( PS4.data.button.touchpad )
                                        Serial.println("Touch Pad Button");

                        if ( PS4.data.button.l2 ) {
                                        Serial.print("l2 button at ");
                                        Serial.println(PS4.data.analog.button.l2, DEC);
                        }
                        if ( PS4.data.button.r2 ) {
                                        Serial.print("r2 button at ");
                                        Serial.println(PS4.data.analog.button.r2, DEC);
                        }

                        if ( PS4.event.analog_move.stick.lx ) {
                                        Serial.print("Left Stick x at ");
                                        Serial.println(PS4.data.analog.stick.lx, DEC);
                        }
                        if ( PS4.event.analog_move.stick.ly ) {
                                        Serial.print("Left Stick y at ");
                                        Serial.println(PS4.data.analog.stick.ly, DEC);
                        }
                        if ( PS4.event.analog_move.stick.rx ) {
                                        Serial.print("Right Stick x at ");
                                        Serial.println(PS4.data.analog.stick.rx, DEC);
                        }
                        if ( PS4.event.analog_move.stick.ry ) {
                                        Serial.print("Right Stick y at ");
                                        Serial.println(PS4.data.analog.stick.ry, DEC);
                        }
   
}
 
void onConnection() {
   
  if (PS4.isConnected()) {
    Serial.println("Controller connected.");
  }
}


void setup()
{
  Serial.begin(115200);

  ledcSetup(0, 5000, 8);
  ledcAttachPin(25, 0);
  
  PS4.begin("b0:05:94:3c:25:d5");
  Serial.println("Initialization ready!");

  PS4.attach(onEvent);
  PS4.attachOnConnect(onConnection);
}

 

void loop()
{
  if(PS4.isConnected()) {
    Serial.println("Controller connected");
  }
 
  delay(1000);
}
