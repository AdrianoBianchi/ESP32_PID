#include "Arduino.h"
#include "esp32_pid.h"


double Input = 0;
double (*readInputFunction)();
TaskHandle_t Core2Task;


void core2Loop( void * pvParameters ){
  for(;;) {
    Input = (*readInputFunction)();
    vTaskDelay( pdMS_TO_TICKS( 10 ) );
  }
  vTaskDelete( NULL );
}


ESP32PID::ESP32PID(double (*readInputF)(), void (*setOutputF)(double), struct esp32_pid_settings settings)
{
  readInputFunction = readInputF;
  _setOutputF = setOutputF;
  _settings = settings;
  initialize();
}

ESP32PID::ESP32PID(double (*readInputF)(), void (*setOutputF)(double) )
{
  readInputFunction = readInputF;
  _setOutputF = setOutputF;
  initialize();
}

void ESP32PID::initialize(){
  last.Kp = _settings.Kp;
  last.Ki = _settings.Ki;
  last.Kd = _settings.Kd; 
  last.Kp2 = _settings.Kp2;
  last.Ki2 = _settings.Ki2; 
  last.Kd2 = _settings.Kd2; 
  last.OperatingMode = _settings.OperatingMode;
  last.PidDirection = _settings.PidDirection;
  last.SampleTime = _settings.SampleTime;

  mySettings = new SettingsStore(&_settings.SetPoint, &_settings.Kp, &_settings.Ki, &_settings.Kd, &_settings.Kp2, &_settings.Ki2, &_settings.Kd2, &_settings.PidDirection, &_settings.SampleTime, &_settings.pid2Band, &_settings.SettingAdjustmentMultiple);
  mySettings->load();
  // if(debug){_settings.dataLogDelay = 500;}
  inputLog = new DataLogger(_settings.dataLogDelay);
  setPointLog = new DataLogger(_settings.dataLogDelay);
  outputLog = new DataLogger(_settings.dataLogDelay);
  oLed = new Display(200, &Input, &_settings.SetPoint, &PidOutputSum, &_settings.Kp, &_settings.Ki, &_settings.Kd, &_settings.Kp2, &_settings.Ki2, &_settings.Kd2, &_settings.pid2Band, &_settings.OperatingMode, &_settings.SampleTime, &_settings.PidDirection, &PidOutputP, &PidOutputI, &PidOutputD, &ManualOutput, &resetPid, &saveSettings, &UsePrimaryPID, &_settings.SettingAdjustmentMultiple, inputLog, setPointLog, outputLog);
  controls = new Controls(0,35,100,500);
  myPID = new PID(&Input, &PidOutputSum, &_settings.SetPoint, &PidOutputP, &PidOutputI, &PidOutputD, _settings.Kp, _settings.Ki, _settings.Kd, _settings.PidDirection);
  myPID->SetMode(_settings.OperatingMode);
  myPID->SetSampleTime(_settings.SampleTime);
  myPID->SetOutputLimits(0, 100);
  xTaskCreatePinnedToCore(core2Loop, "Core2Task", 10000, NULL, 0, &Core2Task, 0);
}

void ESP32PID::loop()
{
  syncSettings();
  if( failsafe.Enabled && _settings.OperatingMode == AUTOMATIC &&
      (Input <= failsafe.MinInput || Input >= failsafe.MaxInput) ){
    // BAD READING FROM INPUT SENSOR - DISABLE OUTPUT
    myOutput = failsafe.OutputValue;
    outputLog->logData(0);
  }
  else if(_settings.OperatingMode == AUTOMATIC){
    myPID->Compute();
    myOutput = PidOutputSum;
    outputLog->logData(PidOutputSum);
  }
  else if(_settings.OperatingMode == MANUAL){
    myOutput = double(ManualOutput);
    outputLog->logData(double(ManualOutput));
  }
  oLed->processControlInput(controls->getState());
  oLed->update_();
  inputLog->logData(Input);
  setPointLog->logData(_settings.SetPoint);
  _setOutputF(myOutput);

}


void ESP32PID::setFailsafe(int outputState, int min, int max){
  failsafe.Enabled = true;
  failsafe.MinInput = min;
  failsafe.MaxInput = max;
  failsafe.OutputValue = outputState;

}

void ESP32PID::syncSettings(){

  if(abs(Input - _settings.SetPoint) < _settings.pid2Band ){ UsePrimaryPID = true;} else{UsePrimaryPID = false;}

  if(resetPid){
    myPID->Reset();
    resetPid = false;
  }
  if(saveSettings){
    mySettings->save();
    saveSettings = false;
  }
  if(last.OperatingMode != _settings.OperatingMode){
     myPID->SetMode(_settings.OperatingMode);
     last.OperatingMode = _settings.OperatingMode;
  }
  if(last.PidDirection != _settings.PidDirection){
    myPID->SetControllerDirection(_settings.PidDirection);
    last.PidDirection = _settings.PidDirection;
  }
  if(last.SampleTime != _settings.SampleTime){
    myPID->SetSampleTime(_settings.SampleTime);
    last.SampleTime = _settings.SampleTime;
  } 

  if( (last.Kp != _settings.Kp || last.Ki != _settings.Ki || last.Kd != _settings.Kd || last.UsePrimaryPID != UsePrimaryPID) && UsePrimaryPID){
    myPID->SetTunings(_settings.Kp,_settings.Ki,_settings.Kd);
    last.Kp = _settings.Kp;
    last.Ki = _settings.Ki;
    last.Kd = _settings.Kd;
  }

  if((last.Kp2 != _settings.Kp2 || last.Ki2 != _settings.Ki2 || last.Kd2 != _settings.Kd2 || last.UsePrimaryPID != UsePrimaryPID) && !UsePrimaryPID){
    myPID->SetTunings(_settings.Kp2,_settings.Ki2,_settings.Kd2);
    last.Kp2 = _settings.Kp2;
    last.Ki2 = _settings.Ki2;
    last.Kd2 = _settings.Kd2;
  }
  last.UsePrimaryPID = UsePrimaryPID;
}







