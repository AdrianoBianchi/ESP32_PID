
#include "esp32_pid.h"
ESP32PID *myESP32PID;
TaskHandle_t Core2Task;
double Output = 0;
double Input = 0;


void mySetup(){
  // Initialize your input and output sensors
  //
  // mySensor = new Sensor();
  // myHeater = new OutputDevice();
}
void myloop(){
  // Place Code Here That Writes to the Output
  // use the global Output variable which ranges from 0-100
  //
  // myHeater->setOutput( Output / 100.0 );
}
void readInput(){
  // Place code here that reads your input and sets the global Input variable
  // This gets called continuously
  //
  //Input = mySensor.getTemp();
}




/////////////////////////////////////////////////////////////////////////



void setup(void){
  // Initialize ESP32PID and start Background task to continually update input on other core
  myESP32PID = new ESP32PID(&Input, &Output);
  mySetup();
  xTaskCreatePinnedToCore(core2Loop, "Core2Task", 10000, NULL, 0, &Core2Task, 0);
}
void loop(void){
  myESP32PID->loop();
  myloop();
}
void core2Loop( void * pvParameters ){
  for(;;) {
    readInput();
    vTaskDelay( pdMS_TO_TICKS( 10 ) );
  }
  vTaskDelete( NULL );
}