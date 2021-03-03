#ifndef ESP32PID_
#define ESP32PID_
#include "Arduino.h"
#include "pid.h"
#include "controls.h"
#include "data_logger.h"
#include "settings_store.h"
#include "display.h"
#include "webserver.h"
#include "datatypes.h"


class ESP32PID{
  public:
    ESP32PID(double (*readInputFunction_)(), void (*setOutputFunction_)(double), struct esp32_pid_settings settings);
    ESP32PID(double (*readInputFunction_)(), void (*setOutputFunction_)(double));
    void setFailsafe(int outputState, int min, int max=10000);
    void useOutputWindow(int windowPeriodSeconds);
    void loop();
    void enableWebServer();
    void useRedundantInput(double (*readInputFunction_)(), int maxDifference, bool useAverage=false, int outputState=0);
  private:
    struct failsafe_values failsafe;
    struct esp32_pid_settings _settings;
    struct output_window_values outputWindow; 
    struct pid_state_values pidState;
    struct last_values last;

    double myOutput=0;
    bool resetPid = false;
    bool saveSettings = false;
    bool webServerEnabled = false;

    bool redundantSensorUseAverage = false;
    int redundantSensorMaxDifference;
    int redundantSensorErrorOutput;

    void (*setOutputFunction)(double);
    void initialize();
    void syncSettings();
    double calculateWindowOutput(double output);

    // Objects
    Display *oLed;
    PID *myPID;
    Controls *controls;
    DataLogger *inputLog;
    DataLogger *setPointLog;
    DataLogger *outputLog;
    SettingsStore *mySettings;
    WebServer *myWebServer;
};

#endif
