#include "Arduino.h"
#include "input_.h"




Input_::Input_(double (*readInputFunction_)())
{
  readInputA = readInputFunction_;
}

input_state Input_::read()
{
  input_state myState;
  myState.useRedundantSensor = redundantInputEnabled;
  myState.useAverage = redundantUseAverage;
  double InputA = (*readInputA)();


  // Redundant Sensor
  if(redundantInputEnabled){
    double InputB = (*readInputB)();

    if(abs(InputA-InputB) > redundantMaxDifference){
      myState.error = REDUNDANT_SENSOR_ERROR;
      Serial.println("Sensor error.  Primary and redundant sensors not in agreement " + String(InputA) + " " + String(InputB) + ". Max difference = " + String(redundantMaxDifference) );
    }
    else{
      myState.error = 0;
    }
    if(redundantUseAverage){
      myState.input1 = InputA;
      myState.input2 = InputB;
      myState.value = (InputA+InputB)/2.0;
    }
    else{
      myState.value = InputA;
    }
    myState.last_update = millis();
    return myState;
  }

  // Failsafe
  if( failsafeEnabled && (InputA <= failsafeMinInput || InputA >= failsafeMaxInput) ){
    myState.error = FAILSAFE_SENSOR_ERROR;
    Serial.println("Sensor error.  Sensor outside of safe input " + String(failsafeMinInput) + ". Valid range - " + String(failsafeMinInput) + " - " + String(failsafeMaxInput) );
  }
  else{
    myState.value = InputA;
    myState.error = 0;
  }
  myState.last_update = millis();
  return myState;
}

void Input_::useRedundantInput(double (*readInputFunction_)(), int maxDifference, bool useAverage){
  readInputB = readInputFunction_;
  redundantMaxDifference = maxDifference;
  redundantUseAverage = useAverage;
  redundantInputEnabled = true;
}

void Input_::setFailsafe(int min, int max){
  failsafeMinInput = min;
  failsafeMaxInput = max;
  failsafeEnabled = true;
}