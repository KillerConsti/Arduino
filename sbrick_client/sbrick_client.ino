/**
 * A BLE client example that is rich in capabilities.
 * There is a lot new capabilities implemented.
 * author unknown
 * updated by chegewara
 */
#include <PS4Controller.h>

#include "BLEDevice.h"
//#include "BLEScan.h"
//https://social.sbrick.com/custom/The_SBrick_BLE_Protocol.pdf
// The remote service we wish to connect to.
static BLEUUID serviceUUID("4dc591b0-857c-41de-b5f1-15abda665b0c");
// The characteristic of the remote service we are interested in.
//static BLEUUID    charUUID("b8cbcc-0e25-4bda-8790-a15f53e6010f");
//static BLEUUID    charUUID("02b8cbcc-0e25-4bda-8790-a15f53e6010f");
//Qucikdrive
//489a6ae0-c1ab-4c9c-bdb2-11d373c1b7fb"
static BLEUUID  charUUID("489a6ae0-c1ab-4c9c-bdb2-11d373c1b7fb");
static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = true;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;
int oldport1,oldport2,oldport3,oldport4;
  int port1 = 0;
  int port2 = 0;
  int port3 = 0;
  int port4 = 0;
  //either 0 or 1 ... changes direcetion (first bit is the direction bit
  int umpol_1 = 1;
  int umpol_2 = 1;
  int umpol_3 = 1;
  int umpol_4 = 0;
static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
    //diabled
    return;
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
    if(pRemoteCharacteristic->canRead()) {
      String value = pRemoteCharacteristic->readValue();
      Serial.print("The characteristic value was: ");
      Serial.println(value.c_str());
    }

    if(pRemoteCharacteristic->canNotify())
      pRemoteCharacteristic->registerForNotify(notifyCallback);

    connected = true;
    return true;
}
/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
 /**
   * Called for each advertising BLE server.
   */
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    //Serial.print("BLE Advertised Device found: ");
    //Serial.println(advertisedDevice.toString().c_str());
    //Serial.println(advertisedDevice.getName().c_str());
    // We have found a device, let us now see if it contains the service we are looking for.
    //if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {
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

class MyCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    String rxValue = pCharacteristic->getValue();

    if (rxValue.length() > 0) {
    Serial.println("*********");
    Serial.print("Received Value: ");
    for (int i = 0; i < rxValue.length(); i++)
      Serial.print(rxValue[i]);

    Serial.println();
    Serial.println("*********");
    }
  }
};



void setup() {
  Serial.begin(9600);
      PS4.begin("b0:05:94:3c:25:d5");
    Serial.println("Ready.");
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

  //PS4
} // End of setup.

void onConnection() {
   
  if (PS4.isConnected()) {
    Serial.println("Controller connected.");
  }
}
int a =1;
// This is the Arduino main loop function.
void loop() {

//read PS4 Controller
  if(PS4.isConnected()) 
{

      //Right Analog Stick x
      port1 = PS4.data.analog.stick.rx;
      //if(abs(port1) < 10)
      //port1 = 0;
      if(abs(port1) < 9)
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
      if(abs(port2) < 9)
       port2 =0; 
      if(port2 >= 0)
      {
        port2 = constrain(port2*2+1+umpol_2, 0,253+umpol_1);
      }
      else
      {
        port2 =constrain(abs(port2*2+umpol_2), 0,254+umpol_1);
      }
      if(port2 != oldport2)
      {
        oldport2 = port2;
        Serial.print("Port2 (ly): ");
        Serial.println(port2);
      }
            //R Shoulder Keys
      if(PS4.data.analog.button.r2  > 10 && PS4.data.button.r1 ==0)
        port3= 125+umpol_3;
       else if (PS4.data.analog.button.r2 <10 && PS4.data.button.r1 ==1)
       port3= 126-umpol_3;
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
        port4= 125+umpol_4;
       else if (PS4.data.analog.button.l2 < 10 && PS4.data.button.l1 ==1)
       port4= 126-umpol_4;
       else
       port4 =2;

                   if(port4 != oldport4)
      {
        oldport4 = port4;
        Serial.print("Port4 (Left Shoulder): ");
        Serial.println(port4);
      }

}

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
  if (connected) {
    String newValue = "Time since boot: " + String(millis()/1000);
    a= a+2;
    // Set the characteristic's value to be the array of bytes that is actually a string.

    //264 is max
    //ungerade nach rechts (1) und gerade nach links
    byte value[]={port1,port2,port3,port4}; 
    pRemoteCharacteristic->writeValue(value,sizeof(value));
    //pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());
  }else if(doScan){
    Serial.println("Shall scan");
    BLEDevice::getScan()->start(0);  // this is just eample to start scan after disconnect, most likely there is better way to do it in arduino
  }
  else
  {
    Serial.println("do not scan");
  }
  
  delay(50); // Delay a second between loops.
} // End of loop
