/**
   A BLE client example that is rich in capabilities.
   There is a lot new capabilities implemented.
   author unknown
   updated by chegewara
*/

#include "BLEDevice.h"
//#include "BLEScan.h"
#include <PS4Controller.h>
#include <PowerFunctions.h>

PowerFunctions pf(26, 0);
PowerFunctions pf_channel_4(26, 3);
/*lego app
  _UART_UUID = bluetooth.UUID("6E400001-B5A3-F393-E0A9-E50E24DCCA9E")
  _UART_TX = (
    bluetooth.UUID("6E400003-B5A3-F393-E0A9-E50E24DCCA9E"),
    _FLAG_READ | _FLAG_NOTIFY,
  )
  _UART_RX = (
    bluetooth.UUID("6E400002-B5A3-F393-E0A9-E50E24DCCA9E"),
    _FLAG_WRITE | _FLAG_WRITE_NO_RESPONSE,
*/
// The remote service we wish to connect to.
//static BLEUUID serviceUUID("4fafc201-1fb5-459e-8fcc-c5c9c331914b");
static BLEUUID serviceUUID("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");
// The characteristic of the remote service we are interested in.
//static BLEUUID    charUUID("beb5483e-36e1-4688-b7f5-ea07361b26a8");
static BLEUUID    charUUID("6E400002-B5A3-F393-E0A9-E50E24DCCA9E");

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;
bool PS4_ControllerConnected = false;
static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
  Serial.print("Notify callback for characteristic ");
  Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
  Serial.print(" of data length ");
  Serial.println(length);
  Serial.print("data: ");
  Serial.println((char*)pData);
}

class MyClientCallback : public BLEClientCallbacks {
    void onConnect(BLEClient* pclient) {
    }

    void onDisconnect(BLEClient* pclient) {
      connected = false;
      Serial.println("onDisconnect");
    }
};

bool connectToServer() {
  Serial.print("Forming a connection to ");
  Serial.println(myDevice->getAddress().toString().c_str());

  BLEClient*  pClient  = BLEDevice::createClient();
  Serial.println(" - Created client");

  pClient->setClientCallbacks(new MyClientCallback());

  // Connect to the remove BLE Server.
  pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
  Serial.println(" - Connected to server");

  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr) {
    Serial.print("Failed to find our service UUID: ");
    Serial.println(serviceUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our service");


  // Obtain a reference to the characteristic in the service of the remote BLE server.
  pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
  if (pRemoteCharacteristic == nullptr) {
    Serial.print("Failed to find our characteristic UUID: ");
    Serial.println(charUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our characteristic");

  // Read the value of the characteristic.
  if (pRemoteCharacteristic->canRead()) {
    String value = pRemoteCharacteristic->readValue();
    Serial.print("The characteristic value was: ");
    Serial.println(value.c_str());
  }

  if (pRemoteCharacteristic->canNotify())
    pRemoteCharacteristic->registerForNotify(notifyCallback);

  connected = true;
  return true;
}
/**
   Scan for BLE servers and find the first one that advertises the service we are looking for.
*/
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    /**
        Called for each advertising BLE server.
    */
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      Serial.print("BLE Advertised Device found: ");
      Serial.println(advertisedDevice.toString().c_str());

      // We have found a device, let us now see if it contains the service we are looking for.
      if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {

        BLEDevice::getScan()->stop();
        myDevice = new BLEAdvertisedDevice(advertisedDevice);
        doConnect = true;
        doScan = true;

      } // Found our server
    } // onResult
}; // MyAdvertisedDeviceCallbacks



// PS4



//byte 0-5
// 0-255
uint8_t l2_val = 0;
uint8_t r2_val = 0;
//-128 bis 127
int8_t left_stick_x = 0;
int8_t left_stick_y = 0;
int8_t right_stick_x = 0;
int8_t right_stick_y = 0;


//next byte is for analog Pad (6)
byte AnalogPad;

/*
         up 0
         down 1
         left 2
         right 3
         upright 4
         upleft 5
         downleft 6
         downright 7
*/

//(Buttons 7)
byte Buttons1;
/*
                           triangle 0
                           circle 1
                           cross 2
                           square3
                           l1 4
                           r1 5
                           l3 6
                           r3 7

*/
//More Buttons (8)
byte Buttons2;

/*

                    share 0
                    options 1
                    ps 2
                    touchpad 3

*/
unsigned long previousMillis = 0;        // will store last time LED was updated
void SetButtons()


{
  l2_val = PS4.data.analog.button.l2;
  if (abs(l2_val) < 15)
    l2_val = 0;

  r2_val = PS4.data.analog.button.r2;
  if (abs(r2_val) < 15)
    r2_val = 0;

  left_stick_x = PS4.data.analog.stick.lx;
  if (abs(left_stick_x) < 6)
    left_stick_x = 0;


  left_stick_y = PS4.data.analog.stick.ly;
  if (abs(left_stick_y) < 6)
    left_stick_y = 0;

  right_stick_x = PS4.data.analog.stick.rx;
  if (abs(right_stick_x) < 6)
    right_stick_x = 0;

  right_stick_y = PS4.data.analog.stick.ry;
  if (abs(right_stick_y)  < 6)
    right_stick_y = 0;

  AnalogPad = 0;
  if ( PS4.data.button.up )
    AnalogPad += 1;
  if ( PS4.data.button.down )
    AnalogPad += 2;
  if ( PS4.data.button.left )
    AnalogPad += 4;
  if ( PS4.data.button.right )
    AnalogPad += 8;

  if ( PS4.data.button.upright )
    AnalogPad += 16;
  if ( PS4.data.button.upleft )
    AnalogPad += 32;
  if ( PS4.data.button.downleft )
    AnalogPad += 64;
  if ( PS4.data.button.downright )
    AnalogPad += 128;

  //
  Buttons1 = 0;

  if ( PS4.data.button.triangle )
    Buttons1 += 1;
  if ( PS4.data.button.circle )
    Buttons1 += 2;
  if ( PS4.data.button.cross )
    Buttons1 += 4;
  if ( PS4.data.button.square )
    Buttons1 += 8;

  if ( PS4.data.button.l1 )
    Buttons1 += 16;
  if ( PS4.data.button.r1 )
    Buttons1 += 32;

  if ( PS4.data.button.l3 )
    Buttons1 += 64;
  if ( PS4.data.button.r3 )
    Buttons1 += 128;

  Buttons2 = 0;
  if ( PS4.data.button.share )
    Buttons2 += 1;
  if ( PS4.data.button.options )
    Buttons2 += 2;

  if ( PS4.data.button.ps )
    Buttons2 += 4;
  if ( PS4.data.button.touchpad )
    Buttons2 += 8;
}

void onConnection() {

  if (PS4.isConnected()) {
    Serial.println("Controller connected.");
  }
}

bool BLEMode = false;
void onEvent() {

  if (PS4.event.button_down.triangle) {
    Serial.println("triangle");
    BLEMode = !BLEMode;
  }

}

const int ledPin =  25;// the number of the LED pin
int ledState = LOW;



void step(uint8_t output, uint8_t pwm,  uint16_t time) {
  pf.single_pwm(output, pwm);
  //delay(time);
  //pf.single_pwm(output, PWM_BRK);
  //delay(30);
  //pf.single_pwm(output, PWM_FLT);
}


void setup() {
  Serial.begin(9600);
  PS4_ControllerConnected = false;
  //PS4 Stuff
  PS4.begin("b0:05:94:3c:25:d5");
  Serial.println("Initialization ready!");

  PS4.attach(onEvent);
  PS4.attachOnConnect(onConnection);

  //Mindstoms stuff

  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");

  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 5 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
  Serial.println(ESP.getFreeHeap());
  //button
} // End of setup.


const long interval = 100;           // interval at which to blink (milliseconds)
// This is the Arduino main loop function.
void loop() {
  //Serial.println(ESP.getFreeHeap());
  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are
  // connected we set the connected flag to be true.
  if (doConnect == true) {
    if (connectToServer()) {
      Serial.println("We are now connected to the BLE Server.");
    } else {
      Serial.println("We have failed to connect to the server; there is nothin more we will do.");
    }
    doConnect = false;
  }

  // If we are connected to a peer BLE Server, update the characteristic each time we are reached
  // with the current time since boot.

  if (connected && BLEMode == true) {

    SetButtons();

    byte inputValues [9];

    inputValues[0] = l2_val;
    inputValues[1] = r2_val;
    //-128 bis 127
    inputValues[2] = left_stick_x;
    inputValues[3] = left_stick_y;
    inputValues[4] = right_stick_x;
    inputValues[5] = right_stick_y;
    inputValues[6] = AnalogPad;
    inputValues[7] = Buttons1;
    inputValues[8] = Buttons2;
    //Rückwärts fahren
    if (l2_val - r2_val > 0)
      ledState = HIGH;
    else
      ledState = LOW;
    digitalWrite(ledPin, ledState);
    //BBbbbbBBB
    Serial.println("send data now");
    //B is unsigned Char 0 -255
    //b is signed Char -128 to 127
    pRemoteCharacteristic->writeValue(inputValues, sizeof(inputValues));
    //return;
  }
  else //if (!connected && BLEMode == false)
  {

    //Maybe store send signals?
    SetButtons();
    Serial.println(-1*left_stick_x,DEC);
    //Turn with x-Axis
    if (left_stick_x >= 108)
      step(RED, PWM_REV7, 500);
    else if (left_stick_x >= 90)
      step(RED, PWM_REV6, 500);
    else if (left_stick_x >= 72)
      step(RED, PWM_REV5, 500);
    else if (left_stick_x >= 54)
      step(RED, PWM_REV4, 500);
    else if (left_stick_x >= 32)
      step(RED, PWM_REV3, 500);

    else if (left_stick_x >= -32)
      step(RED, PWM_BRK, 500);
    else if (left_stick_x >= -54)
      step(RED, PWM_FWD4, 500);
    else if (left_stick_x >= -72)
      step(RED, PWM_FWD5, 500);
    else if (left_stick_x >= -90)
      step(RED, PWM_FWD6, 500);
    else
      step(RED, PWM_FWD7, 500);

      //Blue Cannel extend crane
      //L2- R2 - so right Button makes it extend
      int value = l2_val - r2_val;
          if (value >= 216)
      step(BLUE, PWM_FWD7, 500);
    else if (value >= 180)
      step(BLUE, PWM_FWD6, 500);
    else if (value >= 144)
      step(BLUE, PWM_FWD5, 500);
    else if (value >= 108)
      step(BLUE, PWM_FWD4, 500);
    else if (value >= 64)
      step(BLUE, PWM_FWD3, 500);

    else if (value >= -64)
      step(BLUE, PWM_BRK, 500);
    else if (value >= -128)
      step(BLUE, PWM_REV4, 500);
    else if (value >= -144)
      step(BLUE, PWM_REV5, 500);
    else if (value >= -180)
      step(BLUE, PWM_REV6, 500);
    else
      step(BLUE, PWM_REV7, 500);

    //Channel 4: rope
    if(PS4.data.button.r1 && !PS4.data.button.l1)
    pf_channel_4.single_pwm(RED,PWM_REV7);
    else if (!PS4.data.button.r1 && PS4.data.button.l1) 
    pf_channel_4.single_pwm(RED,PWM_FWD7);
    else
    pf_channel_4.single_pwm(RED,PWM_BRK);

    //Channel 4: yaw
    if (right_stick_y >= 108)
         pf_channel_4.single_pwm(BLUE, PWM_REV7);

     else if (right_stick_y >= 90)
      pf_channel_4.single_pwm(BLUE,  PWM_REV6);
    else if (right_stick_y >= 72)
      pf_channel_4.single_pwm(BLUE,  PWM_REV5);
    else if (right_stick_y >= 54)
      pf_channel_4.single_pwm(BLUE,  PWM_REV4);
    else if (right_stick_y >= 32)
      pf_channel_4.single_pwm(BLUE,  PWM_REV3);

    else if (right_stick_y >= -32)
      pf_channel_4.single_pwm(BLUE,  PWM_BRK);
    else if (right_stick_y >= -54)
      pf_channel_4.single_pwm(BLUE,  PWM_FWD4);
    else if (right_stick_y >= -72)
      pf_channel_4.single_pwm(BLUE,  PWM_FWD5);
    else if (right_stick_y >= -90)
      pf_channel_4.single_pwm(BLUE,  PWM_FWD6); 
    else if(right_stick_y < -108)
      pf_channel_4.single_pwm(BLUE,PWM_FWD7);
    
  }
  //K is
 // else if (doScan) {
   // BLEDevice::getScan()->start(0);  // this is just example to start scan after disconnect, most likely there is better way to do it in arduino
  //}
  delay(250); // Delay a second between loops.
  //delay(50); //=20Hz
} // End of loop
