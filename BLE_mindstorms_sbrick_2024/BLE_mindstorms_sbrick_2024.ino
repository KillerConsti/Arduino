/**
   A BLE client example that is rich in capabilities.
   There is a lot new capabilities implemented.
   author unknown
   updated by chegewara
*/

#include "BLEDevice.h"
#include <PS4Controller.h>

//sbrick
static BLEUUID sbrick_serviceUUID("4dc591b0-857c-41de-b5f1-15abda665b0c");
static BLEUUID sbrick_charUUID("489a6ae0-c1ab-4c9c-bdb2-11d373c1b7fb");
static BLEAdvertisedDevice* myDevice;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
//mindstorms
bool TriangleOldState = false;
static BLEUUID spike_serviceUUID("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");
static BLEUUID spike_charUUID("6E400002-B5A3-F393-E0A9-E50E24DCCA9E");

static boolean mindstorms_doConnect = false;
static boolean mindstorms_Connected = false;
static boolean mindstorms_doScan = false;

static BLEAdvertisedDevice* myDevice_Mindstorms;
static BLERemoteCharacteristic* Spike_pRemoteCharacteristic;

bool PS4_ControllerConnected = false;

int oldport1,oldport2,oldport3,oldport4 = 0;
  int port1 = 0;
  int port2 = 0;
  int port3 = 0;
  int port4 = 0;
  //either 0 or 1 ... changes direcetion (first bit is the direction bit
  int umpol_1 = 0;
  int umpol_2 = 1;
  int umpol_3 = 1;
  int umpol_4 = 1;

static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
  /*Serial.print("Notify callback for characteristic ");
  Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
  Serial.print(" of data length ");
  Serial.println(length);
  Serial.print("data: ");
  Serial.println((char*)pData);*/
}

class MyClientCallback : public BLEClientCallbacks {
    void onConnect(BLEClient* pclient) {
    }

    void onDisconnect(BLEClient* pclient) {
      connected = false;
      Serial.println("SbrickonDisconnect");
    }
};

class MyClientCallbackMS : public BLEClientCallbacks {
    void onConnect(BLEClient* pclient) {
    }

    void onDisconnect(BLEClient* pclient) {
      connected = false;
      Serial.println("MS onDisconnect");
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
  BLERemoteService* pRemoteService = pClient->getService(sbrick_serviceUUID);
  if (pRemoteService == nullptr) {
    Serial.print("Failed to find our service UUID: ");
    Serial.println(sbrick_serviceUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println("Found our service (sbrick)");


  // Obtain a reference to the characteristic in the service of the remote BLE server.
  pRemoteCharacteristic = pRemoteService->getCharacteristic(sbrick_charUUID);
  if (pRemoteCharacteristic == nullptr) {
    Serial.print("Failed to find our characteristic UUID (spike): ");
    Serial.println(sbrick_charUUID.toString().c_str());
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

bool connectmindstormsToServer() {
  Serial.print("Forming a connection to ");
  Serial.println(myDevice_Mindstorms->getAddress().toString().c_str());

  BLEClient*  pClient  = BLEDevice::createClient();
  Serial.println(" - Created client");

  pClient->setClientCallbacks(new MyClientCallbackMS());

  // Connect to the remove BLE Server.
  pClient->connect(myDevice_Mindstorms);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
  Serial.println(" - Connected to server");

  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService* pRemoteService = pClient->getService(spike_serviceUUID);
  if (pRemoteService == nullptr) {
    Serial.print("Failed to find our service UUID: ");
    Serial.println(spike_serviceUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our MS service");


  // Obtain a reference to the characteristic in the service of the remote BLE server.
  Spike_pRemoteCharacteristic = pRemoteService->getCharacteristic(spike_charUUID);
  if (Spike_pRemoteCharacteristic == nullptr) {
    Serial.print("Failed to find our characteristic UUID: ");
    Serial.println(spike_charUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our characteristic");

  // Read the value of the characteristic.
  if (Spike_pRemoteCharacteristic->canRead()) {
    String value = Spike_pRemoteCharacteristic->readValue();
    Serial.print("The characteristic value was: ");
    Serial.println(value.c_str());
  }

  if (Spike_pRemoteCharacteristic->canNotify())
    Spike_pRemoteCharacteristic->registerForNotify(notifyCallback);

  mindstorms_Connected = true;
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
       String Name = advertisedDevice.getName().c_str();
    if(Name.indexOf("SBrick") != -1){
    //if("SBrick" == advertisedDevice.getName().c_str()){
      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;
      Serial.println("found an sbrick");
    } // Found our server
  } // onResult
}; // MyAdvertisedDeviceCallbacks

class MyAdvertisedDeviceCallbacksMindstorms: public BLEAdvertisedDeviceCallbacks {
    /**
        Called for each advertising BLE server.
    */
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      Serial.print("BLE Advertised Device found: ");
      Serial.println(advertisedDevice.toString().c_str());

      // We have found a device, let us now see if it contains the service we are looking for.
      if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(spike_serviceUUID)) {

        BLEDevice::getScan()->stop();
        myDevice_Mindstorms = new BLEAdvertisedDevice(advertisedDevice);
        mindstorms_doConnect = true;
        mindstorms_doScan = true;

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
    if(BLEMode)
      Serial.println("BLE Mode on");
    else
    Serial.println("BLE Mode off");
  }

}

const int REVERSE_DRIVE_ledPin =  25;// the number of the LED pin
int ledState = LOW;





void setup() {
  Serial.begin(9600);
  PS4_ControllerConnected = false;
  //PS4 Stuff
  PS4.begin("b0:05:94:3c:25:d5");
  Serial.println("Initialization ready!");
  mindstorms_doScan = true;
  //PS4.attach(onEvent);
  //PS4.attachOnConnect(onConnection);

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
  pBLEScan->start(50, false);

  /*BLEScan* pBLEScan2 = BLEDevice::getScan();
    pBLEScan2->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacksMindstorms());
  pBLEScan2->setInterval(1349);
  pBLEScan2->setWindow(449);
  pBLEScan2->setActiveScan(true);
  pBLEScan2->start(5, false);*/
  //button
  pinMode(REVERSE_DRIVE_ledPin, OUTPUT);
} // End of setup.

// This is the Arduino main loop function.
void loop() {
  Serial.print("o");
  if (doConnect) {
    if (connectToServer()) {
      Serial.println("(Sbrick) We are now connected to the BLE Server.");
    } else {
      Serial.println("We have failed to connect to the server; there is nothin more we will do.");
    }
    doConnect = false;
  }

  //Mindstorms
  if (mindstorms_doConnect) {
    if (connectmindstormsToServer()) {
      Serial.println("(MS) We are now connected to the BLE Server.");
    } else {
      Serial.println("We have failed to connect to the server; there is nothin more we will do.");
    }
    mindstorms_doConnect = false;
  }
  //scan for mindstorms if our sbrick is connected
  else if (mindstorms_doScan && connected)
  {
    BLEScan* pBLEScan2 = BLEDevice::getScan();
    pBLEScan2->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacksMindstorms());
    pBLEScan2->setInterval(1349);
    pBLEScan2->setWindow(449);
    pBLEScan2->setActiveScan(true);
    pBLEScan2->start(5, false);
    mindstorms_doScan =false;
  }

  // If we are connected to a peer BLE Server, update the characteristic each time we are reached
  // with the current time since boot.
  CheckTriangle();
  if (mindstorms_Connected && BLEMode == true && PS4.isConnected()) {
    
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
    digitalWrite(REVERSE_DRIVE_ledPin, ledState);
    //BBbbbbBBB
    //Serial.println("send data now");
    //B is unsigned Char 0 -255
    //b is signed Char -128 to 127
    Spike_pRemoteCharacteristic->writeValue(inputValues, 9);
    //Serial.println("Time to send sth");
    //return;
  }
  if (connected && PS4.isConnected())
  {
    if( BLEMode == false)
    {
    //Maybe store send signals?
    SetPortData();
    //value[]={port1,port2,port3,port4}; 
    //1 turning motor (port 2)

    byte value[]={port4,port1,port2,port3};
    /*
    /*Serial.print("write value to characteristic ");
    Serial.print(port1);
    Serial.print(" ");
    Serial.print(port2);
    Serial.print(" ");
    Serial.print(port3);
    Serial.print(" ");
    Serial.println(port4);*/
    pRemoteCharacteristic->writeValue(value,sizeof(value));
    }
    else
    {
      byte value[]={0,0,0,0};
      pRemoteCharacteristic->writeValue(value,sizeof(value));
    }
  }
  /*else if (doScan) {
    BLEDevice::getScan()->start(0);  // this is just example to start scan after disconnect, most likely there is better way to do it in arduino
  }*/
  /*else if (mindstorms_doScan)
  {
    BLEDevice::getScan()->start(0);
  }*/
  delay(20); // Delay a second between loops.
  //delay(50); //=20Hz
} // End of loop

//Change mode when triangle is pressed
void CheckTriangle()
{
    if(PS4.isConnected()) 
{
  if(PS4.Triangle() != TriangleOldState)
  {
    TriangleOldState = PS4.Triangle();
    if(TriangleOldState)
    {
      BLEMode = !BLEMode;
      Serial.println("BLE Mode");
      Serial.println(BLEMode);
    }
  }
}
}
void SetPortData()
{
  if(PS4.isConnected()) 
{
 

      //Right Analog Stick x
      port1 = PS4.data.analog.stick.rx;
      //if(abs(port1) < 10)
      //port1 = 0;
      if(abs(port1) < 20)
       port1 =0; 
      if(port1 >= 0)
      {
        port1 = constrain(port1*2+1+umpol_1, 0,253+umpol_1);
      }
      else
      {
        port1 =constrain(abs(port1*2+umpol_1), 0,254+umpol_1);
      }
      if(port1 != oldport1)
      {
        oldport1 = port1;
        Serial.print("Port1 (rx): ");
        Serial.println(port1);
      }

      //Left Analog Stick y

      port2 = PS4.data.analog.stick.ly;
      //if(abs(port1) < 10)
      //port1 = 0;
      if(abs(port2) < 20)
       port2 =0; 
      if(port2 >= 0)
      {
        port2 = constrain(port2*2+1+umpol_2, 0,253+umpol_2);
      }
      else
      {
        port2 =constrain(abs(port2*2+umpol_2), 0,254+umpol_2);
      }
      if(port2 != oldport2)
      {
        oldport2 = port2;
        Serial.print("Port2 (ly): ");
        Serial.println(port2);
      }
            //R Shoulder Keys
      /*if(PS4.data.analog.button.r2  > 10 && PS4.data.button.r1 ==0)
        port3= 253+umpol_3;
       else if (PS4.data.analog.button.r2 <10 && PS4.data.button.r1 ==1)
       port3= 254-umpol_3;
       else
       port3 =2;
            if(port3 != oldport3)
      {
        oldport3 = port3;
        Serial.print("Port3 (Right Shoulder): ");
        Serial.println(port3);
      }
                   //L Shoulder Keys

      if(PS4.data.analog.button.l2  > 10 && PS4.data.button.l1 == 0)
        port4= 253+umpol_4;
       else if (PS4.data.analog.button.l2 < 10 && PS4.data.button.l1 ==1)
       port4= 254-umpol_4;
       else
       port4 =2;

                   if(port4 != oldport4)
      {
        oldport4 = port4;
        Serial.print("Port4 (Left Shoulder): ");
        Serial.println(port4);
      }*/

      //L1 AND R1
      if(PS4.data.button.r1  > 0 && PS4.data.button.l1 ==0)
        port3= 253+umpol_3;
       else if (PS4.data.button.l1 > 0 && PS4.data.button.r1 ==0)
       port3= 254-umpol_3;
       else
       port3 =2;
            if(port3 != oldport3)
      {
        oldport3 = port3;
        Serial.print("Port3 (l1_r1): ");
        Serial.println(port3);
      }

      //L2 AND R2
            port4 = PS4.data.analog.button.l2 - PS4.data.analog.button.r2;
      //if(abs(port1) < 10)
      //port1 = 0;
      if(abs(port4) < 20)
       port4 =0; 
      if(port4 >= 0)
      {
        port4 = constrain(port4*2+1+umpol_4, 0,253+umpol_4);
      }
      else 
      {
        port4 =constrain(abs(port4*2+umpol_4), 0,254+umpol_4);
      }
      if(port4 != oldport4)
      {
        oldport4 = port4;
        Serial.print("Port4 (shoulder l2_r2): ");
        Serial.println(port4);
      }
}
}

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