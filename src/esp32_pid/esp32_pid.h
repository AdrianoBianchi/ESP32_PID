#ifndef ESP32PID_
#define ESP32PID_
#include "Arduino.h"


class ESP32PID{
  public:   
    ESP32PID(double *Input, double *Output, bool debug=false);
    void setFailsafe(int outputState, int min, int max=10000);
    void loop();
  private:
    double *_Input;
    double *_Output;
    double PidOutput = 0;
    bool _failsafeEnabled = false;
    int _failsafeMinInput = -10000;
    int _failsafeMaxInput = 10000;
    int _failsafeOutputValue = 0;
    void syncSettings();
};

#endif
