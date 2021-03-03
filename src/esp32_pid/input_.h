#ifndef Input__
#define Input__

#include "Arduino.h"


struct input_state{
  double value;
  int error = 0;
  unsigned int last_update;
};

class Input_{
  public:
    #define REDUNDANT_SENSOR_ERROR 1
    #define FAILSAFE_SENSOR_ERROR 2

    Input_(double (*readInputFunction_)());
    
    input_state read();
    void useRedundantInput(double (*readInputFunction_)(), int maxDifference, bool useAverage=false);
    void setFailsafe(int min, int max);
  private:
    double (*readInputA)();
    double (*readInputB)();

    bool redundantInputEnabled = false;
    bool redundantUseAverage = false;
    int redundantMaxDifference;

    bool failsafeEnabled = false;
    int failsafeMinInput = -10000;
    int failsafeMaxInput = 10000;

};

#endif
