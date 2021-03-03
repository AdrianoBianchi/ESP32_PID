#include "Arduino.h"
#include "webserver.h"
#include "data_logger.h"
#include "webpages.h"

#define WIFI_CONNECTION_CHECK_TIMEOUT 10000

// Set web server port number to 80
WiFiServer myserver(80);


#define ROUTES_LEN 5
Route myRoutes[ROUTES_LEN] = {  
                                {"GET", "/variables", *variables_get},
                                {"GET", "/chart_data", *chart_data},
                                {"GET", "/last_chart_data", *last_chart_data},
                                {"GET", "/asdf", *route_test},
                                {"GET", "/ ", *route_home }
                             };




WebServer::WebServer( double *Input,
                      struct esp32_pid_settings *settings,
                      struct pid_state_values *pidState,
                      bool *resetPID,bool *saveSettings,
                      DataLogger *inputLog, DataLogger *setpointLog, DataLogger *outputLog)
{

  _Input = Input;
  _settings = settings;
  _pidState = pidState;
  _resetPID = resetPID;
  _saveSettings = saveSettings;
  _inputLog = inputLog;
  _setpointLog = setpointLog;
  _outputLog = outputLog;
}

void WebServer::sendResponseHeaders(){
  // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
  // and a content-type so the client knows what's coming, then a blank line:
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println("Access-Control-Allow-Origin: *");
  client.println("Connection: close");
  client.println();
};

void WebServer::send404(){
  Serial.println("No route found, sending 404");
  client.println("HTTP/1.1 404 Not Found");
  client.println();
}

void WebServer::parseRoute(){
  for(int i=0; i<ROUTES_LEN; i++){
    if(header.indexOf(myRoutes[i].requestType + " " + myRoutes[i].route) == 0 ){


      Serial.println("Found Matching Route: " + myRoutes[i].requestType + " " + myRoutes[i].route);
      sendResponseHeaders();
      (*myRoutes[i].handler)(*this);
      return;
    }
  }
  send404();

};

void WebServer::processRequests(){
  if(!wifiLastOnline && millis() - lastWifiConnectionCheck < WIFI_CONNECTION_CHECK_TIMEOUT){
    // Wifi wasn't online last time we checked, wait to recheck
    return;
  }
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Error Starting Server, No WIFI Connection!");
    lastWifiConnectionCheck = millis();
    return;
  }
  if(!serverStarted){
    myserver.begin();
    Serial.print("Server Started");
    serverStarted = true;
  }
  wifiLastOnline = true;

  clientListenStartTime = millis();
  client = myserver.available();   // Listen for incoming clients

  if (client) {
    currentTime = millis();
    requestStartTime = currentTime;
    previousTime = currentTime;
    // Serial.println("New Client.");



    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        //  Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            responseStartTime = millis();
            parseRoute();
            client.println();
            Serial.println("Total Time: " + String(millis()-clientListenStartTime) + "ms (" + 
                                            String(requestStartTime-clientListenStartTime) + ", " + 
                                            String(responseStartTime-requestStartTime) + ", " + 
                                            String(millis() - responseStartTime) + ")");
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    // Serial.println("Client disconnected.");
    // Serial.println("");
  }
}