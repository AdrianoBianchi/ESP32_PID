#include "Arduino.h"
#include "esp32_pid.h"
#include "display.h"
#include "pid.h"
#include "controls.h"
#include "data_logger.h"
#include "settings_store.h"

// INITIAL SETTINGS PARAMETERS
double SetPoint = 80;
bool PidDirection = DIRECT;
int SampleTime = 1000;
double Kp=5, Ki=0, Kd=0;
double Kp2=10, Ki2=0, Kd2=0;
int pid2Band = 10;
int dataLogDelay = 10000; // 10 seconds - gives about 40 minutes of chart
int SettingAdjustmentMultiple = 1;


Display *oLed;
PID *myPID;
Controls *controls;
DataLogger *inputLog;
DataLogger *setPointLog;
DataLogger *outputLog;
SettingsStore *mySettings;

double pOutput, iOutput, dOutput;
double lastKp=Kp, lastKi=Ki, lastKd=Kd;
double lastKp2=Kp2, lastKi2=Ki2, lastKd2=Kd2;
bool tick = false;
bool resetPid = false;
bool saveSettings = false;
bool OperatingMode = AUTOMATIC;
bool UsePrimaryPID = true;
bool lastUsePrimaryPID = true;
bool LastOperatingMode = OperatingMode;
bool LastPidDirection = PidDirection;
int LastSampleTime = SampleTime;
int ManualOutput = 0;
int ControlState;



ESP32PID::ESP32PID(double *Input, double *Output, bool debug)
{
  _Input = Input;
  _Output = Output;
  
  mySettings = new SettingsStore(&SetPoint, &Kp, &Ki, &Kd, &Kp2, &Ki2, &Kd2, &PidDirection, &SampleTime, &pid2Band, &SettingAdjustmentMultiple);
  mySettings->load();
  if(debug){dataLogDelay = 500;}
  inputLog = new DataLogger(dataLogDelay);
  setPointLog = new DataLogger(dataLogDelay);
  outputLog = new DataLogger(dataLogDelay);
  oLed = new Display(200, Input, &SetPoint, &PidOutput, &Kp, &Ki, &Kd, &tick, &Kp2, &Ki2, &Kd2, &pid2Band, &OperatingMode, &SampleTime, &PidDirection, &pOutput, &iOutput, &dOutput, &ManualOutput, &resetPid, &saveSettings, &UsePrimaryPID, &SettingAdjustmentMultiple, inputLog, setPointLog, outputLog);
  controls = new Controls(0,35,100,500);
  myPID = new PID(Input, &PidOutput, &SetPoint, &pOutput, &iOutput, &dOutput, Kp, Ki, Kd, PidDirection);
  myPID->SetMode(OperatingMode);
  myPID->SetSampleTime(SampleTime);
  myPID->SetOutputLimits(0, 100);
}


void ESP32PID::loop()
{
  syncSettings();
  if( _failsafeEnabled && OperatingMode == AUTOMATIC &&
      (*_Input <= _failsafeMinInput || *_Input >= _failsafeMaxInput) ){
    // BAD READING FROM INPUT SENSOR - DISABLE OUTPUT
    *_Output = _failsafeOutputValue;
    outputLog->logData(0);
  }
  else if(OperatingMode == AUTOMATIC){
    myPID->Compute();
    *_Output = PidOutput;
    outputLog->logData(PidOutput);
  }
  else if(OperatingMode == MANUAL){
    *_Output = double(ManualOutput);
    outputLog->logData(double(ManualOutput));
  }
  oLed->processControlInput(controls->getState());
  oLed->update_();
  inputLog->logData(*_Input);
  setPointLog->logData(SetPoint);

}


void ESP32PID::setFailsafe(int outputState, int min, int max){
  _failsafeEnabled = true;
  _failsafeMinInput = min;
  _failsafeMaxInput = max;
  _failsafeOutputValue = outputState;

}

void ESP32PID::syncSettings(){

  if(abs(*_Input - SetPoint) < pid2Band ){ UsePrimaryPID = true;} else{UsePrimaryPID = false;}

  if(resetPid){
    myPID->Reset();
    resetPid = false;
  }
  if(saveSettings){
    mySettings->save();
    saveSettings = false;
  }
  if(LastOperatingMode != OperatingMode){
     myPID->SetMode(OperatingMode);
     LastOperatingMode = OperatingMode;
  }
  if(LastPidDirection != PidDirection){
    myPID->SetControllerDirection(PidDirection);
    LastPidDirection = PidDirection;
  }
  if(LastSampleTime != SampleTime){
    myPID->SetSampleTime(SampleTime);
    LastSampleTime = SampleTime;
  } 

  if( (lastKp != Kp || lastKi != Ki || lastKd != Kd || lastUsePrimaryPID != UsePrimaryPID) && UsePrimaryPID){
    myPID->SetTunings(Kp,Ki,Kd);
    lastKp = Kp;
    lastKi = Ki;
    lastKd = Kd;
  }

  if((lastKp2 != Kp2 || lastKi2 != Ki2 || lastKd2 != Kd2 || lastUsePrimaryPID != UsePrimaryPID) && !UsePrimaryPID){
    myPID->SetTunings(Kp2,Ki2,Kd2);
    lastKp2 = Kp2;
    lastKi2 = Ki2;
    lastKd2 = Kd2;
  }
  lastUsePrimaryPID = UsePrimaryPID;
}








