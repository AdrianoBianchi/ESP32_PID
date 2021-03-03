#include "Arduino.h"
#include "esp32_pid.h"


double Input = 0;
double InputA = 0;
double InputB = 0;
bool USE_REDUNDANT_SENSOR = false;
double (*readInputFunction)();
double (*readRedundantInputFunction)();
TaskHandle_t Core2Task;


void core2Loop( void * pvParameters ){
  for(;;) {
    InputA = (*readInputFunction)();
    if(USE_REDUNDANT_SENSOR){
      InputB = (*readRedundantInputFunction)();
    }
    vTaskDelay( pdMS_TO_TICKS( 10 ) );
  }
  vTaskDelete( NULL );
}


ESP32PID::ESP32PID(double (*readInputF)(), void (*setOutputF)(double), struct esp32_pid_settings settings)
{
  readInputFunction = readInputF;
  setOutputFunction = setOutputF;
  _settings = settings;
  initialize();
}

ESP32PID::ESP32PID(double (*readInputFunction_)(), void (*setOutputFunction_)(double) )
{
  readInputFunction = readInputFunction_;
  setOutputFunction = setOutputFunction_;
  initialize();
}

void ESP32PID::useOutputWindow(int windowLength){
  outputWindow.use = true;
  outputWindow.windowLengthMs = windowLength * 1000UL;
  outputWindow.cycleStartTime = millis();

}

void ESP32PID::useRedundantInput(double (*readInputFunction_)(), int maxDifference, bool useAverage, int outputState){
  
  readRedundantInputFunction = readInputFunction_;
  redundantSensorMaxDifference = maxDifference;
  redundantSensorErrorOutput = outputState;
  redundantSensorUseAverage = useAverage;
  USE_REDUNDANT_SENSOR = true;

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
  oLed = new Display(200, &Input, &_settings.SetPoint, &pidState.PidOutputSum, &_settings.Kp, &_settings.Ki, &_settings.Kd, &_settings.Kp2, &_settings.Ki2, &_settings.Kd2, &_settings.pid2Band, &_settings.OperatingMode, &_settings.SampleTime, &_settings.PidDirection, &pidState.PidOutputP, &pidState.PidOutputI, &pidState.PidOutputD, &pidState.ManualOutput, &resetPid, &saveSettings, &pidState.UsePrimaryPID, &_settings.SettingAdjustmentMultiple, inputLog, setPointLog, outputLog);
  controls = new Controls(0,35,100,500);
  myPID = new PID(&Input, &pidState.PidOutputSum, &_settings.SetPoint, &pidState.PidOutputP, &pidState.PidOutputI, &pidState.PidOutputD, _settings.Kp, _settings.Ki, _settings.Kd, _settings.PidDirection);
  myPID->SetMode(_settings.OperatingMode);
  myPID->SetSampleTime(_settings.SampleTime);
  myPID->SetOutputLimits(0, 100);
  xTaskCreatePinnedToCore(core2Loop, "Core2Task", 10000, NULL, 0, &Core2Task, 0);
}

void ESP32PID::loop()
{
  syncSettings();
  float output;


  // Redundancy check
  bool redundantSensorError = false;
  if(USE_REDUNDANT_SENSOR){
    if(abs(InputA-InputB)>redundantSensorMaxDifference){
      redundantSensorError = true;
    }
    if(redundantSensorUseAverage){
      Input = (InputA+InputB)/2.0;
    }
    else{
      Input = InputA;
    }
  }
  else{
    Input = InputA;
  }

  
  if(redundantSensorError && _settings.OperatingMode == AUTOMATIC){
    output = redundantSensorErrorOutput;
    Serial.println("Sensor error.  Primary and redundant sensors not in agreement " + String(InputA) + " " + String(InputB) );
  }
  else if( failsafe.Enabled && _settings.OperatingMode == AUTOMATIC &&
      (Input <= failsafe.MinInput || Input >= failsafe.MaxInput) ){
    // BAD READING FROM INPUT SENSOR - DISABLE OUTPUT
    output = failsafe.OutputValue;
  }
  else if(_settings.OperatingMode == AUTOMATIC){
    myPID->Compute();
    output = pidState.PidOutputSum;
  }
  else if(_settings.OperatingMode == MANUAL){
    output = double(pidState.ManualOutput);

  }
  oLed->processControlInput(controls->getState());
  oLed->update_();

  if(outputWindow.use){
    myOutput = calculateWindowOutput(output);
  }
  else{
    myOutput = output;
  }
  
  if(myOutput != last.myOutput){
    last.myOutput = myOutput;
    setOutputFunction(myOutput);
  }

  inputLog->logData(Input);
  setPointLog->logData(_settings.SetPoint);
  outputLog->logData(output);
}

double ESP32PID::calculateWindowOutput(double output){
  
    outputWindow.cycleTimeElapsed = millis() - outputWindow.cycleStartTime;


    if( outputWindow.cycleTimeElapsed >= outputWindow.windowLengthMs){ 
      outputWindow.cycleStartTime = millis();
      Serial.println("resetting cycle time");
      outputWindow.cycleOnTime = output/100.0*outputWindow.windowLengthMs;
      outputWindow.cycleTimeElapsed = millis() - outputWindow.cycleStartTime;
    }

    if (outputWindow.cycleTimeElapsed < outputWindow.cycleOnTime ){
      return 100;
    }else{
      return 0;
    }
  }


void ESP32PID::setFailsafe(int outputState, int min, int max){
  failsafe.Enabled = true;
  failsafe.MinInput = min;
  failsafe.MaxInput = max;
  failsafe.OutputValue = outputState;

}

void ESP32PID::syncSettings(){

  if(abs(Input - _settings.SetPoint) < _settings.pid2Band ){ pidState.UsePrimaryPID = true;} else{pidState.UsePrimaryPID = false;}

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

  if( (last.Kp != _settings.Kp || last.Ki != _settings.Ki || last.Kd != _settings.Kd || last.UsePrimaryPID != pidState.UsePrimaryPID) && pidState.UsePrimaryPID){
    myPID->SetTunings(_settings.Kp,_settings.Ki,_settings.Kd);
    last.Kp = _settings.Kp;
    last.Ki = _settings.Ki;
    last.Kd = _settings.Kd;
  }

  if((last.Kp2 != _settings.Kp2 || last.Ki2 != _settings.Ki2 || last.Kd2 != _settings.Kd2 || last.UsePrimaryPID != pidState.UsePrimaryPID) && !pidState.UsePrimaryPID){
    myPID->SetTunings(_settings.Kp2,_settings.Ki2,_settings.Kd2);
    last.Kp2 = _settings.Kp2;
    last.Ki2 = _settings.Ki2;
    last.Kd2 = _settings.Kd2;
  }
  last.UsePrimaryPID = pidState.UsePrimaryPID;
}







