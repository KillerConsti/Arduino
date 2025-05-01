/**
   A BLE client example that is rich in capabilities.
   There is a lot new capabilities implemented.
   author unknown
   updated by chegewara
*/

#include "BLEDevice.h"
//#include "BLEScan.h"
#include <PS4Controller.h>

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
    std::string value = pRemoteCharacteristic->readValue();
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

const int LEDPin = 25;
const int LEDPin2 = 4 ;
const int buttonPin = 32;     // the number of the pushbutton pin

int buttonState = 0;         // variable for reading the pushbutton status


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
    PS4_ControllerConnected = true;
    previousMillis = millis();
  }
}



const int ledPin =  26;// the number of the LED pin
int ledState = LOW;  
/**/
void setup() {
  Serial.begin(115200);
PS4_ControllerConnected = false;
  //PS4 Stuff
  PS4.begin("b0:05:94:3c:25:d5");
  Serial.println("Initialization ready!");

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
  pBLEScan->start(5, false);
Serial.println(ESP.getFreeHeap());
  //button
} // End of setup.


const long interval = 100;           // interval at which to blink (milliseconds)
// This is the Arduino main loop function.
void loop() {
 Serial.println(ESP.getFreeHeap());
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

  if (connected && PS4_ControllerConnected == true && previousMillis + 2000 < millis()) {
    
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
      pRemoteCharacteristic->writeValue(inputValues, 36);
      delay(500);
      //return;
    }
    //K is
   else if (doScan) {
    BLEDevice::getScan()->start(0);  // this is just example to start scan after disconnect, most likely there is better way to do it in arduino
  }
  Serial.println("not");
  delay(1000); // Delay a second between loops.
  //delay(50); //=20Hz
} // End of loop
