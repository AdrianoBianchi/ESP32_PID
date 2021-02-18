#include "Arduino.h"
#include "settings_store.h"
#include <EEPROM.h>

#define ARRAY_SIZE 11
#define STARTING_EEPROM_ADDRESS 10
#define SAVED_STATUS_ADDRESS 1


void writeIntArrayIntoEEPROM(int address, int numbers[], int arraySize)
{
  int addressIndex = address;
  for (int i = 0; i < arraySize; i++) 
  {
    EEPROM.write(addressIndex, numbers[i] >> 8);
    EEPROM.write(addressIndex + 1, numbers[i] & 0xFF);
    addressIndex += 2;
  }
}
void readIntArrayFromEEPROM(int address, int numbers[], int arraySize)
{
  int addressIndex = address;
  for (int i = 0; i < arraySize; i++)
  {
    numbers[i] = (EEPROM.read(addressIndex) << 8) + EEPROM.read(addressIndex + 1);
    addressIndex += 2;
  }
}



SettingsStore::SettingsStore(double *SetPoint, double *Kp, double *Ki, double *Kd, double *Kp2, double *Ki2, double *Kd2, bool *PidDirection, int *SampleTime, int *pid2Band, int *SettingAdjustmentMultiple)
{
  _SetPoint = SetPoint;
  _Kp = Kp;
  _Ki = Ki;
  _Kd = Kd;
  _Kp2 = Kp2;
  _Ki2 = Ki2;
  _Kd2 = Kd2;
  _PidDirection = PidDirection;
  _SampleTime = SampleTime;
  _pid2Band = pid2Band;
  _SettingAdjustmentMultiple = SettingAdjustmentMultiple;
  EEPROM.begin(100);

}


bool SettingsStore::save(){
  // write to variables
  // Serial.println("Saving settings to EEPROM");
  int numbers[ARRAY_SIZE] = {
              int(*_SetPoint),
              int(*_Kp),
              int(*_Ki*100),
              int(*_Kd),
              int(*_Kp2),
              int(*_Ki2*100),
              int(*_Kd2),
              int(*_PidDirection),
              *_SampleTime,
              *_pid2Band,
              *_SettingAdjustmentMultiple
   };
  writeIntArrayIntoEEPROM(STARTING_EEPROM_ADDRESS, numbers, ARRAY_SIZE);
  EEPROM.write(SAVED_STATUS_ADDRESS, 1);
  EEPROM.commit();
}

bool SettingsStore::load(){
  Serial.println(EEPROM.read(SAVED_STATUS_ADDRESS));
  if(!EEPROM.read(SAVED_STATUS_ADDRESS)){
    // Serial.println("No settings in EEPROM, cannot load");
    return false;
  }
  // Serial.println("Loading settings from EEPROM");
  // retrieve data and save to variables
  int newNumbers[ARRAY_SIZE];
  readIntArrayFromEEPROM(STARTING_EEPROM_ADDRESS, newNumbers, ARRAY_SIZE);


  // Serial.println(newNumbers[0]);
  // Serial.println(newNumbers[1]);
  // Serial.println(newNumbers[2]);
  // Serial.println(newNumbers[3]);
  // Serial.println(newNumbers[4]);
  // Serial.println(newNumbers[5]);
  // Serial.println(newNumbers[6]);
  // Serial.println(newNumbers[7]);
  // Serial.println(newNumbers[8]);
  // Serial.println(newNumbers[9]);
  // Serial.println(newNumbers[10]);

  *_SetPoint =                  newNumbers[0];
  *_Kp =                        newNumbers[1];
  *_Ki =                        newNumbers[2]/100.0;
  *_Kd =                        newNumbers[3];
  *_Kp2 =                       newNumbers[4];
  *_Ki2 =                       newNumbers[5]/100.0;
  *_Kd2 =                       newNumbers[6];
  *_PidDirection =              bool(newNumbers[7]);
  *_SampleTime =                newNumbers[8];
  *_pid2Band =                  newNumbers[9];
  *_SettingAdjustmentMultiple = newNumbers[10];
  return true;


}


