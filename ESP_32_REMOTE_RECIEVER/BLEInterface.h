#include <BLEDevice.h>
#include <BLEServer.h>
//#include <BLEUtils.h>
#include <BLE2902.h>
#include <Globals.h>
BLEServer* pServer = NULL;
BLECharacteristic* pSensorCharacteristic = NULL;

BLECharacteristic* pt1Characteristic = NULL;
BLECharacteristic* pt2Characteristic = NULL;
BLECharacteristic* ps1Characteristic = NULL;
BLECharacteristic* ps2Characteristic = NULL;
BLECharacteristic* cmdCharacteristic = NULL;
BLECharacteristic* TeamNameCharacteristic = NULL;
BLECharacteristic* TeamMathNumCharacteristics = NULL;
BLECharacteristic* TeamNameCharacteristic_Field2 = NULL;
BLECharacteristic* TeamMathNumCharacteristics_Field2 = NULL;

BLECharacteristic* TeamStatsFromRadio_Match2 = NULL;
BLECharacteristic* points1Characteristic_Match2 = NULL;
BLECharacteristic* points2Characteristic_Match2 = NULL;
BLECharacteristic* sets1Characteristic_Match2 = NULL;
BLECharacteristic* sets2Characteristic_Match2 = NULL;


BLECharacteristic* LEDScrrenModeCharaceristic =NULL;
#define SERVICE_UUID "19b10000-e8f2-537e-4f6c-d104768a1214"
#define SENSOR_CHARACTERISTIC_UUID "19b10001-e8f2-537e-4f6c-d104768a1214"
#define T1_CHARACTERISTIC_UUID "19b10002-e8f2-537e-4f6c-d104768a1214"
#define T2_CHARACTERISTIC_UUID "19b10003-e8f2-537e-4f6c-d104768a1214"
#define S1_CHARACTERISTIC_UUID "19b10004-e8f2-537e-4f6c-d104768a1214"
#define S2_CHARACTERISTIC_UUID "19b10005-e8f2-537e-4f6c-d104768a1214"
#define CMD_CHARACTERISTIC_UUID "19b10005-e8f2-537e-4f6c-d104768a1215"
//this is for field 1
#define mTeamNameCharaceristic "19b10006-e8f2-537e-4f6c-d104768a1214"
#define mTeamMatchNumCharaceristic "19b10007-e8f2-537e-4f6c-d104768a1214"
#define mTeamNameCharaceristic_Field2 "19b10008-e8f2-537e-4f6c-d104768a1214"
#define mTeamMatchNumCharaceristic_Field2 "19b10009-e8f2-537e-4f6c-d104768a1214"
#define mLEDScrrenModeCharaceristic "19b10010-e8f2-537e-4f6c-d104768a1214"

//Match2 UUIDs
	#define mPunkte_Team1_Match2 "19b10011-e8f2-537e-4f6c-d104768a1214"
	#define mPunkte_Team2_Match2 "19b10012-e8f2-537e-4f6c-d104768a1214"
	#define mSets_Team1_Match2 "19b10013-e8f2-537e-4f6c-d104768a1214"
	#define mSets_Team2_Match2 "19b10014-e8f2-537e-4f6c-d104768a1214"
	#define mRadioResult_Match2 "19b10015-e8f2-537e-4f6c-d104768a1214"

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    NeedToNotifyBLE_BothMatches = true;  //keep us updated pls
  };

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
  }
};

class MyCharacteristicCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pt1Characteristic) {
    String value = pt1Characteristic->getValue();
    if (value.length() > 0) {
      Serial.print("Characteristic event, written: ");
      Serial.println(static_cast<int>(value[0]));  // Print the integer value

      int receivedValue = static_cast<int>(value[0]);
      Serial.println(receivedValue);
      mRadioScores[0].mT1 = constrain(receivedValue, 0, 99);

              NeedToNotifyTFT1 = true;
              NeedToNotifyRadio1 = true;
              NeedToNotifyBLE_BothMatches = true;
    }
  }
};
class MyCharacteristicCallbacks2 : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pt2Characteristic) {
    String value = pt2Characteristic->getValue();
    if (value.length() > 0) {
      Serial.print("Characteristic event, written: ");
      Serial.println(static_cast<int>(value[0]));  // Print the integer value

      int receivedValue = static_cast<int>(value[0]);
      Serial.println(receivedValue);
      mRadioScores[0].mT2 = constrain(receivedValue, 0, 99);
              NeedToNotifyTFT1 = true;
              NeedToNotifyRadio1 = true;

      NeedToNotifyBLE_BothMatches = true;
    }
  }
};

class SetPointsTeam1_Match2Callback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* points1Characteristic_Match2) {
    String value = points1Characteristic_Match2->getValue();
    if (value.length() > 0) {
      Serial.print("Characteristic event, written: ");
      Serial.println(static_cast<int>(value[0]));  // Print the integer value

      int receivedValue = static_cast<int>(value[0]);
      Serial.println(receivedValue);
      mRadioScores[1].mT1 = constrain(receivedValue, 0, 99);
              NeedToNotifyTFT2 = true;
              NeedToNotifyRadio2 = true;

      NeedToNotifyBLE_BothMatches = true;
    }
  }
};
class SetPointsTeam2_Match2Callback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* points2Characteristic_Match2) {
    String value = points2Characteristic_Match2->getValue();
    if (value.length() > 0) {
      Serial.print("Characteristic event, written: ");
      Serial.println(static_cast<int>(value[0]));  // Print the integer value

      int receivedValue = static_cast<int>(value[0]);
      Serial.println(receivedValue);
      mRadioScores[1].mT2 = constrain(receivedValue, 0, 99);
              NeedToNotifyTFT2= true;
              NeedToNotifyRadio2 = true;

      NeedToNotifyBLE_BothMatches = true;
    }
  }
};


class MyCharacteristicCallbacks3 : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* ps1Characteristic) {
    String value = ps1Characteristic->getValue();
    if (value.length() > 0) {
      Serial.print("Characteristic event, written: ");
      Serial.println(static_cast<int>(value[0]));  // Print the integer value

      int receivedValue = static_cast<int>(value[0]);
      Serial.println(receivedValue);
      mRadioScores[0].mS1 = constrain(receivedValue, 0, 3);
              NeedToNotifyTFT1 = true;
              NeedToNotifyRadio1 = true;
           
      NeedToNotifyBLE_BothMatches = true;
    }
  }
};
class MyCharacteristicCallbacks4 : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* ps2Characteristic) {
    String value = ps2Characteristic->getValue();
    if (value.length() > 0) {
      Serial.print("Characteristic event, written: ");
      Serial.println(static_cast<int>(value[0]));  // Print the integer value

      int receivedValue = static_cast<int>(value[0]);
      Serial.println(receivedValue);
      mRadioScores[0].mS2 = constrain(receivedValue, 0, 3);
              NeedToNotifyTFT1 = true;
              NeedToNotifyRadio1 = true;
      NeedToNotifyBLE_BothMatches = true;
    }
  }
};

class Set_Sets_Team1_Match2 : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* sets1Characteristic_Match2) {
    String value = sets1Characteristic_Match2->getValue();
    if (value.length() > 0) {
      Serial.print("Characteristic event, written: ");
      Serial.println(static_cast<int>(value[0]));  // Print the integer value

      int receivedValue = static_cast<int>(value[0]);
      Serial.println(receivedValue);
      mRadioScores[1].mS1 = constrain(receivedValue, 0, 3);
              NeedToNotifyTFT2 = true;
              NeedToNotifyRadio2 = true;
      NeedToNotifyBLE_BothMatches = true;
    }
  }
};

class Set_Sets_Team2_Match2 : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* sets2Characteristic_Match2) {
    String value = sets2Characteristic_Match2->getValue();
    if (value.length() > 0) {
      Serial.print("Characteristic event, written: ");
      Serial.println(static_cast<int>(value[0]));  // Print the integer value

      int receivedValue = static_cast<int>(value[0]);
      Serial.println(receivedValue);
      mRadioScores[1].mS2 = constrain(receivedValue, 0, 3);
              NeedToNotifyTFT2 = true;
              NeedToNotifyRadio2 = true;
      NeedToNotifyBLE_BothMatches = true;
    }
  }
};

class MyCharacteristicCallbacksCMD //This is for Sending Softreset/Hardreset/Partymode
  : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* cmdCharacteristic) {
    String value = cmdCharacteristic->getValue();
    if (value.length() <= 0)
    return;

      Serial.print("Characteristic event (button), written: ");
      Serial.println(static_cast<int>(value[0]));  // Print the integer value

      int mreceivedValue = static_cast<int>(value[0]);
      Serial.println(mreceivedValue);
      if (mreceivedValue == 0)  //soft
      {
        CommandMode_field1.CommandId = 2;
        CommandMode_field1.Arg1 = 0;
        CommandMode_field1.MessageRecieved = true;
      } 
      else if (mreceivedValue == 1)
       {
        CommandMode_field1.CommandId = 2;
        CommandMode_field1.Arg1 = 1;
        CommandMode_field1.MessageRecieved = true;
      } else if (mreceivedValue == 2)  //PartyModeOn
      {
        CommandMode_field1.CommandId = 4;
        CommandMode_field1.Arg1 = 1;
        CommandMode_field1.MessageRecieved = true;
    }
  }
};

class MyCharacteristicCallbacksTeamMatch1
 : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* TeamMathNumCharacteristics) {
    String value = TeamMathNumCharacteristics->getValue();
    int mreceivedValue = 0;
    if (value.length() > 0) {
     mreceivedValue = static_cast<int>(value[0]);
      if(value.length() == 2)
      {
        mreceivedValue = mreceivedValue*10;
      mreceivedValue +=static_cast<int>(value[1]);
      }
        Serial.print("recieved  new current match 1 value");
      Serial.println(mreceivedValue);
      currentMatch_Field1 = mreceivedValue;
      NeedToSendMachData_1_CurrentMatch = true;
       NeedToSendMachData_1_NextMatch =true;
    }
  }
 };

class LEDScrrenModeCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* LEDScrrenModeCharaceristic) {
  String value = LEDScrrenModeCharaceristic->getValue();
  Serial.println(static_cast<int>(value[0]));
  LEDScreenMode = static_cast<int>(value[0]);
  NeedToUpdateScreenMode = true;
  }
  };

class MyCharacteristicCallbacksTeamNames
  : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* TeamNameCharacteristic) {
    String value = TeamNameCharacteristic->getValue();
    if (value.length() > 0) {
      //write everything into MatchNameData_Field1
      int TeamnameNumber =0;
      //Serial.print("Team Names written: ");
      //Serial.println(value);  // Print the integer value
      Serial.println(TeamNameCharacteristic->getLength());
      char TeamName[TeamnameLength+1];
      int LastFinish = 0;
      for(size_t t= 0 ; t < TeamNameCharacteristic->getLength();t++)
      {
        //Serial.print(t);
        //Serial.print(" ");
        //Serial.println(value[t]);
        char CurrentVal = value[t];
        if(CurrentVal ==':')
        {
          for(size_t u = t-LastFinish ; u < TeamnameLength;u++)
          {
            TeamName[u] = ' ';
            MatchNameData_Field1[u][TeamnameNumber] = ' ';
          }
          TeamName[TeamnameLength] = '\0';
          Serial.println(TeamName);
          LastFinish =t+1;
          TeamnameNumber++;
          continue;
        }
        else if (CurrentVal =='\n')
        {
          Serial.print("found new line ");
                    for(size_t u = t-LastFinish ; u < TeamnameLength;u++)
                              {
            TeamName[u] = ' ';
            MatchNameData_Field1[u][TeamnameNumber] = ' ';
            
          }
          TeamName[TeamnameLength] = '\0';
          Serial.println(TeamName);
          TeamnameNumber++;

          LastFinish =t+1;
          continue;
        }
        else 
        {
          if(t-LastFinish > TeamnameLength-1) //Limit of 12 chars
          continue;
          TeamName[t-LastFinish] = value[t];
          MatchNameData_Field1[t-LastFinish][TeamnameNumber] = value[t];
          
        }
      }
      Serial.print("We have ");
      Serial.print(TeamnameNumber);
      Serial.print(" teams");
      mValidTeamNamesField1 = TeamnameNumber;
      if(currentMatch_Field1 ==-1)
      {
        currentMatch_Field1 = 0;
      }
       Serial.print("Our current match is");
      Serial.println(currentMatch_Field1);
       NeedToSendMachData_1_CurrentMatch = true;
       NeedToSendMachData_1_NextMatch =true;

      //SetCurrentMatchField1(currentMatch_Field1);
      Serial.print("setted matchdata field 1");
    }
  }
};

class MyCharacteristicCallbacksTeamMatch_Field2
 : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* TeamMathNumCharacteristics_Field2) {
    String value = TeamMathNumCharacteristics_Field2->getValue();
    int mreceivedValue = 0;
    if (value.length() > 0) {
     mreceivedValue = static_cast<int>(value[0]);
      if(value.length() == 2)
      {
        mreceivedValue = mreceivedValue*10;
      mreceivedValue +=static_cast<int>(value[1]);
      }
        Serial.print("recieved  new current match 1 value");
      Serial.println(mreceivedValue);
      currentMatch_Field2 = mreceivedValue;
      NeedToSendMachData_2_CurrentMatch = true;
       NeedToSendMachData_2_NextMatch =true;
    }
  }
 };

class MyCharacteristicCallbacksTeamNames_Field2
  : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* TeamNameCharacteristic_Field2) {
    String value = TeamNameCharacteristic_Field2->getValue();
    if (value.length() > 0) {
      //write everything into MatchNameData_Field1
      int TeamnameNumber =0;
      //Serial.print("Team Names written: ");
      //Serial.println(value);  // Print the integer value
      Serial.println(TeamNameCharacteristic_Field2->getLength());
      char TeamName[TeamnameLength+1];
      int LastFinish = 0;
      for(size_t t= 0 ; t < TeamNameCharacteristic_Field2->getLength();t++)
      {
        //Serial.print(t);
        //Serial.print(" ");
        //Serial.println(value[t]);
        char CurrentVal = value[t];
        if(CurrentVal ==':')
        {
          for(size_t u = t-LastFinish ; u < TeamnameLength;u++)
          {
            TeamName[u] = ' ';
            MatchNameData_Field2[u][TeamnameNumber] = ' ';
          }
          TeamName[TeamnameLength] = '\0';
          Serial.println(TeamName);
          LastFinish =t+1;
          TeamnameNumber++;
          continue;
        }
        else if (CurrentVal =='\n')
        {
          Serial.print("found new line ");
                    for(size_t u = t-LastFinish ; u < TeamnameLength;u++)
                              {
            TeamName[u] = ' ';
            MatchNameData_Field2[u][TeamnameNumber] = ' ';
            
          }
          TeamName[TeamnameLength] = '\0';
          Serial.println(TeamName);
          TeamnameNumber++;

          LastFinish =t+1;
          continue;
        }
        else 
        {
          if(t-LastFinish > TeamnameLength-1) //Limit of 12 chars
          continue;
          TeamName[t-LastFinish] = value[t];
          MatchNameData_Field2[t-LastFinish][TeamnameNumber] = value[t];
          
        }
      }
      Serial.print("We have ");
      Serial.print(TeamnameNumber);
      Serial.print(" teams");
      mValidTeamNamesField2 = TeamnameNumber;
      if(currentMatch_Field2 ==-1)
      {
        currentMatch_Field2 = 0;
      }
       Serial.print("Our current match is");
      Serial.println(currentMatch_Field2);
       NeedToSendMachData_2_CurrentMatch = true;
       NeedToSendMachData_2_NextMatch =true;

      //SetCurrentMatchField1(currentMatch_Field1);
      Serial.print("setted matchdata field 2");
    }
  }
};

class BLEInterface
{
   public :
void BleInterface();
void initDevice();
void SendScroreToWebBLE(String Input);
void SendSwapTeamsToWebBle();
void StartAdvertising();
};

void BLEInterface::BleInterface()
{}

void BLEInterface::initDevice()
{
	BLEDevice::init("ESP32");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService* pService = pServer->createService(BLEUUID(SERVICE_UUID), 60, (uint8_t)0);

  // Create a BLE Characteristic
  pSensorCharacteristic = pService->createCharacteristic(SENSOR_CHARACTERISTIC_UUID,BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_INDICATE);
  // Create the ON button Characteristic
  pt1Characteristic = pService->createCharacteristic(T1_CHARACTERISTIC_UUID,BLECharacteristic::PROPERTY_WRITE);
  pt2Characteristic = pService->createCharacteristic(T2_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_WRITE);
  ps1Characteristic = pService->createCharacteristic(S1_CHARACTERISTIC_UUID,BLECharacteristic::PROPERTY_WRITE);
  ps2Characteristic = pService->createCharacteristic(S2_CHARACTERISTIC_UUID,BLECharacteristic::PROPERTY_WRITE);
  cmdCharacteristic = pService->createCharacteristic(CMD_CHARACTERISTIC_UUID,BLECharacteristic::PROPERTY_WRITE);
  TeamNameCharacteristic = pService->createCharacteristic(mTeamNameCharaceristic,BLECharacteristic::PROPERTY_WRITE);
  TeamMathNumCharacteristics = pService->createCharacteristic(mTeamMatchNumCharaceristic,BLECharacteristic::PROPERTY_WRITE);

//Field 2
TeamStatsFromRadio_Match2 = pService->createCharacteristic(mRadioResult_Match2,BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_INDICATE);
points1Characteristic_Match2= pService->createCharacteristic(mPunkte_Team1_Match2,BLECharacteristic::PROPERTY_WRITE);
points2Characteristic_Match2=pService->createCharacteristic(mPunkte_Team2_Match2,BLECharacteristic::PROPERTY_WRITE);
sets1Characteristic_Match2=pService->createCharacteristic(mSets_Team1_Match2,BLECharacteristic::PROPERTY_WRITE);
sets2Characteristic_Match2= pService->createCharacteristic(mSets_Team2_Match2,BLECharacteristic::PROPERTY_WRITE);

    TeamNameCharacteristic_Field2 = pService->createCharacteristic(mTeamNameCharaceristic_Field2,BLECharacteristic::PROPERTY_WRITE);
  TeamMathNumCharacteristics_Field2 = pService->createCharacteristic(mTeamMatchNumCharaceristic_Field2,BLECharacteristic::PROPERTY_WRITE);

  LEDScrrenModeCharaceristic = pService->createCharacteristic(mLEDScrrenModeCharaceristic,BLECharacteristic::PROPERTY_WRITE);
  // Register the callback for the ON button characteristic
  pt1Characteristic->setCallbacks(new MyCharacteristicCallbacks());
  pt2Characteristic->setCallbacks(new MyCharacteristicCallbacks2());
  ps1Characteristic->setCallbacks(new MyCharacteristicCallbacks3());
  ps2Characteristic->setCallbacks(new MyCharacteristicCallbacks4());
  cmdCharacteristic->setCallbacks(new MyCharacteristicCallbacksCMD());
  TeamNameCharacteristic->setCallbacks(new MyCharacteristicCallbacksTeamNames());
  TeamMathNumCharacteristics->setCallbacks(new MyCharacteristicCallbacksTeamMatch1());

  TeamNameCharacteristic_Field2->setCallbacks(new MyCharacteristicCallbacksTeamNames_Field2());
  TeamMathNumCharacteristics_Field2->setCallbacks(new MyCharacteristicCallbacksTeamMatch_Field2());
  //TeamStatsFromRadio_Match2->setCallbacks(new ());
  points1Characteristic_Match2->setCallbacks(new SetPointsTeam1_Match2Callback());
  points2Characteristic_Match2->setCallbacks(new SetPointsTeam2_Match2Callback());
  sets1Characteristic_Match2->setCallbacks(new Set_Sets_Team1_Match2());
  sets2Characteristic_Match2->setCallbacks(new Set_Sets_Team2_Match2());

  LEDScrrenModeCharaceristic->setCallbacks(new LEDScrrenModeCallback());
  // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
  // Create a BLE Descriptor
  pSensorCharacteristic->addDescriptor(new BLE2902());
  pt1Characteristic->addDescriptor(new BLE2902());
  pt2Characteristic->addDescriptor(new BLE2902());
  ps1Characteristic->addDescriptor(new BLE2902());
  ps2Characteristic->addDescriptor(new BLE2902());
  cmdCharacteristic->addDescriptor(new BLE2902());
  TeamNameCharacteristic->addDescriptor(new BLE2902());
  TeamMathNumCharacteristics->addDescriptor(new BLE2902());
TeamNameCharacteristic_Field2->addDescriptor(new BLE2902());
  TeamMathNumCharacteristics_Field2->addDescriptor(new BLE2902());
  LEDScrrenModeCharaceristic->addDescriptor(new BLE2902());

    TeamStatsFromRadio_Match2->addDescriptor(new BLE2902());
  points1Characteristic_Match2->addDescriptor(new BLE2902());
  points2Characteristic_Match2->addDescriptor(new BLE2902());
  sets1Characteristic_Match2->addDescriptor(new BLE2902());
  sets2Characteristic_Match2->addDescriptor(new BLE2902());
  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
}

void SendSwapTeamsToWebBle()
{
  //copy from sendScore to webble but send a 1 instead of a 0 as first digit
}

void BLEInterface::SendScroreToWebBLE(String Input)
{
	      pSensorCharacteristic->setValue(Input.c_str());
      pSensorCharacteristic->notify();
}
void BLEInterface::StartAdvertising()
{
	pServer->startAdvertising();
}