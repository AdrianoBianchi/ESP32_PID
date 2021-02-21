# About #

The goal of this project is to create a fully-feature yet simple to use PID controller based on the [TTGO ESP32 microcontroller](https://github.com/Xinyuan-LilyGO/TTGO-T-Display).  The project is made to be very generic with input and output devices, allowing the user to easily implement any type of devices they choose.

### Features ###
* Settings fully configurable through GUI
* Charts showing historical input and output values
* Persistent settings - save to EEPROM
* Dual PIDs - agressive and conservative
* Manual Override


# Getting Started #

### 1. Prerequisites ###
You will need to have your [Arduino IDE](https://www.arduino.cc/en/software) installed and have it [set up to work with the ESP32 controller](https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/).  You should also have some experience working with arduino sensors.


### 2. Configure ###
After downloading the project to your local machine, open the [esp32_pid/src/esp32_pid/esp32_pid.ino](https://bitbucket.org/jason955/esp32_pid/src/master/src/esp32_pid/esp32_pid.ino) file.

Configure and implement your input and output devices by using the 2 hook functions provided.


```c++
    #include "esp32_pid.h"
    ESP32PID *myESP32PID;


    double readInput(){
      // Place code here that returns the value of your input device
      // This gets called continuously in the background
      // return  mySensor.getTemp();
    }
    void setOutput(double PidOutput){
      // Place Code in this function to set the output value
      // the PidOutput variable from 0-100
      // myHeater->setOutput( PidOutput / 100.0 );
    }


    void setup(void){
      // Initialize your input and output sensors
      // mySensor = new Sensor();
      // myHeater = new OutputDevice();
      
      // Initialize ESP32PID
      myESP32PID = new ESP32PID(readInput, setOutput);
    }
    void loop(void){
      myESP32PID->loop();
    }

```

You can find a working example of a DS18B20 Dallas Temperature Sensor and a Solid State Relay in [examples/DallasTempSensor_and_PWM_SSR/esp32_pid.ino](https://bitbucket.org/jason955/esp32_pid/src/master/examples/DallasTempSensor_and_PWM_SSR/esp32_pid.ino).


### 3. Upload ###

Upload the code to your device and you should be all set!



# Interface #


![](https://bitbucket.org/jason955/esp32_pid/raw/master/img/Buttons.jpg)


**Button 1**

* Short Press - Move Up / Increase Value
* Long Press - Toggle Screen Mode (Home, Large Chart, Advanced Settings)


**Button 2**

* Short Press - Move Down / Decrease Value
* Long Press - Toggle Between Select Mode and Edit Mode



## Modes ##

**Select Mode**

Default mode. When in select mode, you will see one of the menu items on the screen selected in an Orange color.  For example, in the image above the Mode is selected.

You can Short Press Button 1 and Button 2 to move up and down through the selectable items.  The selectible items on this screen are Mode, Setpoint and Gains (P,I,D).

**Edit Mode**

Long Press Button 2 to change to Edit Mode for the selected parameter.  The selected item will turn a magenta color indicating you are in edit mode.  You can short press either of the buttons to increase or decrease the value of the parameter.

To exit Edit Mode and return to Select Mode, long press on either button.

**Screen Mode**

When in Select Mode you can Long Press Button 1 to toggle between screen modes.  There are 3 screens to toggle between (see below) Home, Large Chart and Advanced Settings.


## Screens ##


### Home Screen ###

![](https://bitbucket.org/jason955/esp32_pid/raw/master/img/ScreenHome.jpg)

1. **Operating Mode** - Can be toggled between Normal and Manual.
2. **PID Status** - Top line shows which PID is currently in use (Normal or Agressive) and the bottom line shows the values that each of P, I and D is contributing to the output.
3. **Output** - Value the PID is outputing (sum of the items in 2).  This value ranges from 0% - 100%
4. **Current Input Value**
5. **Setpoint**
6. **PID Gains** - These are the values the PID is using for the Normal PID.



### Large Chart ###

![](https://bitbucket.org/jason955/esp32_pid/raw/master/img/ScreenLargeChart.jpg)

1. **PID Output** - Value the PID is outputing (sum of the items in 2).  This value ranges from 0% - 100%
2. **Internal PID Values** - Indicates the values that each of P, I and D is contributing to the output.  This is used for troubleshooting your PID.
3. **PID In Use** - Indicates which PID is currently in use (Normal or Agressive).


### Advanced Settings ###

![](https://bitbucket.org/jason955/esp32_pid/raw/master/img/ScreenAdvSettings.jpg)

1. **PID Cycle Time** - This is the time in milliseconds the PID is waiting before performing a new calculation.  In this example, the pid is computing every 10 seconds.
2. **PID Direction** - Direct or Reverse.  If an increase in output increases the input value, select Direct.  If an increase in output decreases the input value, select Reverse.
3. **Agressive PID Gains** - These are the values of P, I and D for the Agressive PID.
4. **Agressive PID Operating Band** - This setting determines when the Agressive PID will kick in.  In the image the band is 5, meaning if the input is 5 units or more away from the output, the system will use the Agressive PID. To further clarify, if the band is set to 5, the output is set to 80 and the input is 86, the system will use the Agressive pid.  If the input is 84, the system will use the Normal PID (setting on the home screen). 
5. **Setting Adjustment Multiplier** - This is the amount the parameters will change in Edit Mode.  For example the default adjustment to the Output is 1 up or down.  You can change this setting to 10 and the parameter will change by 10 each adjustment up or down.
6. ....
7. **Reset PIDs** - Sometimes this needs to be cleared out if you have I windup.
8. **Save Settings** - Save all settings to EEPROM



# Examples #

### Solid State Relay Temp Controller ###

![](https://bitbucket.org/jason955/esp32_pid/raw/master/examples/DallasTempSensor_and_PWM_SSR/Strip1.jpg)
![](https://bitbucket.org/jason955/esp32_pid/raw/master/examples/DallasTempSensor_and_PWM_SSR/Strip2.jpg)

[examples/DallasTempSensor_and_PWM_SSR/esp32_pid.ino](https://bitbucket.org/jason955/esp32_pid/src/master/examples/DallasTempSensor_and_PWM_SSR/esp32_pid.ino)


All files including Fusion 360 and STL files can be found [here](https://bitbucket.org/jason955/esp32_pid/src/master/examples/DallasTempSensor_and_PWM_SSR/)