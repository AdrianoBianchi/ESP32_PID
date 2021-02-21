#ifndef ESP32PID_
#define ESP32PID_
#include "Arduino.h"
#include "pid.h"
#include "controls.h"
#include "data_logger.h"
#include "settings_store.h"
#include "display.h"

struct esp32_pid_settings{
  // INITIAL SETTINGS PARAMETERS
  double SetPoint = 80;
  bool PidDirection = DIRECT;
  bool OperatingMode = AUTOMATIC;
  int SampleTime = 1000;
  double Kp=5;
  double Ki=0;
  double Kd=0;
  double Kp2=10;
  double Ki2=0;
  double Kd2=0;
  int pid2Band = 10;
  int dataLogDelay = 10000; // 10 seconds - gives about 40 minutes of chart
  int SettingAdjustmentMultiple = 1;
};

struct last_values{
    double Kp, Ki, Kd;
    double Kp2, Ki2, Kd2;
    bool OperatingMode;
    bool PidDirection;
    int SampleTime;
    bool UsePrimaryPID = true;
};

struct failsafe_values{
    bool Enabled = false;
    int MinInput = -10000;
    int MaxInput = 10000;
    int OutputValue = 0;
};

class ESP32PID{
  public:   
    ESP32PID(double (*readInputFunction)(), void (*setOutputFunction)(double), struct esp32_pid_settings settings);
    ESP32PID(double (*readInputFunction)(), void (*setOutputFunction)(double));
    void setFailsafe(int outputState, int min, int max=10000);
    void loop();
  private:
    // PID Values
    double PidOutputSum=0, PidOutputP, PidOutputI, PidOutputD, myOutput=0;
    bool UsePrimaryPID = true;
    int ManualOutput = 0;

    struct failsafe_values failsafe;
    struct esp32_pid_settings _settings;

    // Internal Value Tracking
    struct last_values last;
    bool resetPid = false;
    bool saveSettings = false;

    void (*_setOutputF)(double);
    void initialize();
    void syncSettings();

    // Objects
    Display *oLed;
    PID *myPID;
    Controls *controls;
    DataLogger *inputLog;
    DataLogger *setPointLog;
    DataLogger *outputLog;
    SettingsStore *mySettings;
};

#endif
