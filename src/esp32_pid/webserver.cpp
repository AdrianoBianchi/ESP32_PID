#include "Arduino.h"
#include "webserver.h"
#include "data_logger.h"

#define WIFI_CONNECTION_CHECK_TIMEOUT 10000

// Set web server port number to 80
WiFiServer myserver(80);



void route_home(WebServer& s){
  s.client.println("<!doctype html>");
  s.client.println("<html lang=\"en\">");
  s.client.println("<head>");
  s.client.println("<meta charset=\"utf-8\">");
  s.client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1, shrink-to-fit=no\">");
  s.client.println("<title>PID Web Access</title>");
  s.client.println("<!-- Bootstrap core CSS -->");
  s.client.println("<link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css\" integrity=\"sha384-JcKb8q3iqJ61gNV9KGb8thSsNjpSL0n8PARn9HuZOnIxN0hoP+VmmDGMN5t9UJ0Z\" crossorigin=\"anonymous\">");
  s.client.println("</head>");
  s.client.println("<body>");
  s.client.println("<style>");
  s.client.println("body {");
  s.client.println("padding-top: 5rem;");
  s.client.println("}");
  s.client.println(".starter-template {");
  s.client.println("padding: 3rem 1.5rem;");
  s.client.println("}");
  s.client.println(".text-center {text-align: center;}");
  s.client.println(".gauge-chart{");
  s.client.println("width:100%;");
  s.client.println("}");
  s.client.println("#pid-internal-values{");
  s.client.println("margin-top: -20px;");
  s.client.println("color: #9f9f9f;");
  s.client.println("}");
  s.client.println("</style>");
  s.client.println("<nav class=\"navbar navbar-expand-md navbar-dark bg-dark fixed-top\">");
  s.client.println("<a class=\"navbar-brand\" href=\"#\">PID Controller</a>");
  s.client.println("<button class=\"navbar-toggler\" type=\"button\" data-toggle=\"collapse\" data-target=\"#navbarsExampleDefault\" aria-controls=\"navbarsExampleDefault\" aria-expanded=\"false\" aria-label=\"Toggle navigation\">");
  s.client.println("<span class=\"navbar-toggler-icon\"></span>");
  s.client.println("</button>");
  s.client.println("<div class=\"collapse navbar-collapse\" id=\"navbarsExampleDefault\">");
  s.client.println("<ul class=\"navbar-nav mr-auto\">");
  s.client.println("<li class=\"nav-item active\">");
  s.client.println("<a class=\"nav-link\" href=\"#\">Home <span class=\"sr-only\">(current)</span></a>");
  s.client.println("</li>");
  s.client.println("</ul>");
  s.client.println("</div>");
  s.client.println("</nav>");
  s.client.println("<main role=\"main\" class=\"container\">");
  s.client.println("<div class=\"starter-template\">");
  s.client.println("<div class=\"row\">");
  s.client.println("<div class=\"col-md-6\">");
  s.client.println("<h1 class=\"text-center\">Current/Setpoint </h1>");
  s.client.println("</div>");
  s.client.println("<div class=\"col-md-6\">");
  s.client.println("<h1 class=\"text-center\">Output</h1>");
  s.client.println("</div>");
  s.client.println("</div>");
  s.client.println("<div class=\"row\">");
  s.client.println("<div class=\"col-md-1\">");
  s.client.println("</div>");
  s.client.println("<div class=\"col-md-4\">");
  s.client.println("<canvas id=\"setpoint-gauge-chart\" class=\"gauge-chart\"></canvas>");
  s.client.println("<p class=\"text-center\"><span id=\"input\"></span> / <span id=\"setpoint\"></span></p>");
  s.client.println("</div>");
  s.client.println("<div class=\"col-md-2\">");
  s.client.println("</div>");
  s.client.println("<div class=\"col-md-4\">");
  s.client.println("<canvas id=\"output-gauge-chart\" class=\"gauge-chart\"></canvas>");
  s.client.println("<p class=\"text-center\"><span id=\"output\"></span><span id=\"outputMode\"></span></p>");
  s.client.println("<p id=\"pid-internal-values\" class=\"text-center\">(<span id=\"PidOutputP\"></span>/<span id=\"PidOutputI\"></span>/<span id=\"PidOutputD\"></span>) <span id=\"pid-in-use\"></span></p>");
  s.client.println("</div>");
  s.client.println("<div class=\"col-md-1\">");
  s.client.println("</div>");
  s.client.println("</div>");
  s.client.println("<canvas id=\"myChart\"></canvas>");
  s.client.println("<canvas id=\"myChart2\"></canvas>");
  s.client.println("<!--         <h2>Settings</h2>");
  s.client.println("<form>");
  s.client.println("<h3>Pid 1</h3>");
  s.client.println("<div class=\"form-row\">");
  s.client.println("<div class=\"form-group col-md-4\">");
  s.client.println("<label for=\"Kp\">Kp</label>");
  s.client.println("<input type=\"text\" class=\"form-control\" id=\"Kp\">");
  s.client.println("</div>");
  s.client.println("<div class=\"form-group col-md-4\">");
  s.client.println("<label for=\"Ki\">Ki</label>");
  s.client.println("<input type=\"text\" class=\"form-control\" id=\"Ki\">");
  s.client.println("</div>");
  s.client.println("<div class=\"form-group col-md-4\">");
  s.client.println("<label for=\"Kd\">Kd</label>");
  s.client.println("<input type=\"text\" class=\"form-control\" id=\"Kd\">");
  s.client.println("</div>");
  s.client.println("</div>");
  s.client.println("<h3>Pid 2</h3>");
  s.client.println("<div class=\"form-row\">");
  s.client.println("<div class=\"form-group col-md-4\">");
  s.client.println("<label for=\"Kp2\">Kp</label>");
  s.client.println("<input type=\"text\" class=\"form-control\" id=\"Kp2\">");
  s.client.println("</div>");
  s.client.println("<div class=\"form-group col-md-4\">");
  s.client.println("<label for=\"Ki2\">Ki</label>");
  s.client.println("<input type=\"text\" class=\"form-control\" id=\"Ki2\">");
  s.client.println("</div>");
  s.client.println("<div class=\"form-group col-md-4\">");
  s.client.println("<label for=\"Kd2\">Kd</label>");
  s.client.println("<input type=\"text\" class=\"form-control\" id=\"Kd2\">");
  s.client.println("</div>");
  s.client.println("</div>");
  s.client.println("<div class=\"form-row\">");
  s.client.println("<div class=\"form-group col-md-4\">");
  s.client.println("<label for=\"pid2Band\">Band</label>");
  s.client.println("<input type=\"text\" class=\"form-control\" id=\"pid2Band\">");
  s.client.println("</div>");
  s.client.println("</div>");
  s.client.println("</form>");
  s.client.println("<p>PidDirection: <span id=\"PidDirection\"></span></p>");
  s.client.println("<p>OperatingMode: <span id=\"OperatingMode\"></span></p>");
  s.client.println("<p>SampleTime: <span id=\"SampleTime\"></span></p>");
  s.client.println("<p>dataLogDelay: <span id=\"dataLogDelay\"></span></p>");
  s.client.println("<p>SettingAdjustmentMultiple: <span id=\"SettingAdjustmentMultiple\"></span></p> -->");
  s.client.println("</div>");
  s.client.println("</main><!-- /.container -->");
  s.client.println("<!-- Bootstrap core JavaScript");
  s.client.println("================================================== -->");
  s.client.println("<!-- Placed at the end of the document so the pages load faster -->");
  s.client.println("<script src=\"https://code.jquery.com/jquery-3.5.1.min.js\" integrity=\"sha256-9/aliU8dGd2tb6OSsuzixeV4y/faTqgFtohetphbbj0=\" crossorigin=\"anonymous\"></script>");
  s.client.println("<script src=\"https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/js/bootstrap.bundle.min.js\" integrity=\"sha384-LtrjvnR4Twt/qOuYxE721u19sVFLVSA4hf/rRt6PrZTmiPltdZcI7q7PXQBYTKyf\" crossorigin=\"anonymous\"></script>");
  s.client.println("<script src=\"https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.9.4/Chart.js\" integrity=\"sha512-hZf9Qhp3rlDJBvAKvmiG+goaaKRZA6LKUO35oK6EsM0/kjPK32Yw7URqrq3Q+Nvbbt8Usss+IekL7CRn83dYmw==\" crossorigin=\"anonymous\"></script>");
  s.client.println("<script src=\"https://bernii.github.io/gauge.js/dist/gauge.min.js\" crossorigin=\"anonymous\"></script>");
  s.client.println("<script>");
  s.client.println("var MAX_CHART_LEN = 200;");
  s.client.println("var API_URL = \"\";");
  s.client.println("var GAUGE_SP;");
  s.client.println("var GAUGE_OUTPUT;");
  s.client.println("function updateGauges(setpoint, input, output){");
  s.client.println("// GAUGE_OUTPUT");
  s.client.println("var diff = Math.abs(input - setpoint);");
  s.client.println("var min = Math.floor(setpoint - diff * 1.1);");
  s.client.println("var max = Math.ceil(setpoint + diff * 1.1);");
  s.client.println("if(GAUGE_SP.maxValue != max){");
  s.client.println("GAUGE_SP.maxValue = max; // set max gauge value");
  s.client.println("}");
  s.client.println("//");
  s.client.println("if(GAUGE_SP.minValue != min){");
  s.client.println("GAUGE_SP.setMinValue(min);");
  s.client.println("}");
  s.client.println("//   // Prefer setter over GAUGE_SP.minValue = 0");
  s.client.println("// GAUGE_SP.animationSpeed = 32; // set animation speed (32 is default value)");
  s.client.println("GAUGE_SP.set(input); // set actual value");
  s.client.println("GAUGE_SP.options.staticLabels.labels = [min,max,setpoint];");
  s.client.println("GAUGE_SP.options.renderTicks.divisions = max-min;");
  s.client.println("GAUGE_OUTPUT.set(output); // set actual value");
  s.client.println("GAUGE_OUTPUT.options.staticLabels.labels = [output];");
  s.client.println("}");
  s.client.println("function updateVariables(){");
  s.client.println("$.getJSON( API_URL + \"/variables\", function( data ) {");
  s.client.println("var items = [];");
  s.client.println("console.log(data);");
  s.client.println("$(\"#input\").text(data.Input);");
  s.client.println("$(\"#setpoint\").text(data.Settings.SetPoint);");
  s.client.println("// $(\"#PidDirection\").text(data.Settings.PidDirection);");
  s.client.println("if(data.Settings.OperatingMode){");
  s.client.println("updateGauges(data.Settings.SetPoint,data.Input,data.pidState.PidOutputSum);");
  s.client.println("$(\"#outputMode\").text(\"\");");
  s.client.println("$(\"#output\").text(data.pidState.PidOutputSum);");
  s.client.println("$(\"#PidOutputP\").text(data.pidState.PidOutputP);");
  s.client.println("$(\"#PidOutputI\").text(data.pidState.PidOutputI);");
  s.client.println("$(\"#PidOutputD\").text(data.pidState.PidOutputD);");
  s.client.println("if(data.pidState.UsePrimaryPID){");
  s.client.println("$(\"#pid-in-use\").text(\"Normal\");");
  s.client.println("}");
  s.client.println("else{");
  s.client.println("$(\"#pid-in-use\").text(\"Agressive\");");
  s.client.println("}");
  s.client.println("$(\"#pid-status\").show();");
  s.client.println("$(\"#pid-internal-values\").show();");
  s.client.println("}");
  s.client.println("else{");
  s.client.println("$(\"#outputMode\").text(\" (Manual)\");");
  s.client.println("$(\"#pid-status\").hide();");
  s.client.println("$(\"#pid-internal-values\").hide();");
  s.client.println("$(\"#output\").text(data.pidState.ManualOutput);");
  s.client.println("updateGauges(data.Settings.SetPoint,data.Input,data.pidState.ManualOutput);");
  s.client.println("}");
  s.client.println("// $(\"#Kp\").val(data.Settings.Kp);");
  s.client.println("// $(\"#Ki\").val(data.Settings.Ki);");
  s.client.println("// $(\"#Kd\").val(data.Settings.Kd);");
  s.client.println("// $(\"#Kp2\").val(data.Settings.Kp2);");
  s.client.println("// $(\"#Ki2\").val(data.Settings.Ki2);");
  s.client.println("// $(\"#Kd2\").val(data.Settings.Kd2);");
  s.client.println("// $(\"#pid2Band\").val(data.Settings.pid2Band);");
  s.client.println("// $(\"#SampleTime\").text(data.Settings.SampleTime);");
  s.client.println("// $(\"#dataLogDelay\").text(data.Settings.dataLogDelay);");
  s.client.println("// $(\"#SettingAdjustmentMultiple\").text(data.Settings.SettingAdjustmentMultiple);");
  s.client.println("});");
  s.client.println("}");
  s.client.println("$( document ).ready(function() {");
  s.client.println("console.log(\"Ready\");");
  s.client.println("updateVariables();");
  s.client.println("var ctx = document.getElementById('myChart').getContext('2d');");
  s.client.println("setInterval(updateVariables, 3000);");
  s.client.println("var opts = {");
  s.client.println("angle: -0.2, // The span of the gauge arc");
  s.client.println("lineWidth: 0.2, // The line thickness");
  s.client.println("radiusScale: 1, // Relative radius");
  s.client.println("pointer: {");
  s.client.println("length: 0.6, // // Relative to gauge radius");
  s.client.println("strokeWidth: 0.042, // The thickness");
  s.client.println("color: '#000000' // Fill color");
  s.client.println("},");
  s.client.println("limitMax: false,     // If false, max value increases automatically if value > maxValue");
  s.client.println("limitMin: false,     // If true, the min value of the gauge will be fixed");
  s.client.println("colorStart: '#6F6EA0',   // Colors");
  s.client.println("colorStop: '#C0C0DB',    // just experiment with them");
  s.client.println("strokeColor: '#EEEEEE',  // to see which ones work best for you");
  s.client.println("generateGradient: true,");
  s.client.println("highDpiSupport: true,     // High resolution support");
  s.client.println("// renderTicks is Optional");
  s.client.println("renderTicks: {");
  s.client.println("divisions: 0,");
  s.client.println("divWidth: 0.1,");
  s.client.println("divLength: 0.7,");
  s.client.println("divColor: '#333333',");
  s.client.println("subDivisions: 4,");
  s.client.println("subLength: 0.2,");
  s.client.println("subWidth: 0.6,");
  s.client.println("subColor: '#666666'");
  s.client.println("},");
  s.client.println("staticLabels: {");
  s.client.println("font: \"16px sans-serif\",  // Specifies font");
  s.client.println("labels: [],  // Print labels at these values");
  s.client.println("color: \"#000000\",  // Optional: Label text color");
  s.client.println("fractionDigits: 0  // Optional: Numerical precision. 0=round off.");
  s.client.println("}");
  s.client.println("};");
  s.client.println("var target = document.getElementById('setpoint-gauge-chart'); // your canvas element");
  s.client.println("GAUGE_SP = new Gauge(target).setOptions(opts); // create sexy gauge!");
  s.client.println("var opts = {");
  s.client.println("angle: -0.2, // The span of the gauge arc");
  s.client.println("lineWidth: 0.2, // The line thickness");
  s.client.println("radiusScale: 1, // Relative radius");
  s.client.println("pointer: {");
  s.client.println("length: 0.6, // // Relative to gauge radius");
  s.client.println("strokeWidth: 0.042, // The thickness");
  s.client.println("color: '#000000' // Fill color");
  s.client.println("},");
  s.client.println("limitMax: false,     // If false, max value increases automatically if value > maxValue");
  s.client.println("limitMin: false,     // If true, the min value of the gauge will be fixed");
  s.client.println("colorStart: '#6F6EA0',   // Colors");
  s.client.println("colorStop: '#C0C0DB',    // just experiment with them");
  s.client.println("strokeColor: '#EEEEEE',  // to see which ones work best for you");
  s.client.println("generateGradient: true,");
  s.client.println("highDpiSupport: true,     // High resolution support");
  s.client.println("// renderTicks is Optional");
  s.client.println("renderTicks: {");
  s.client.println("divisions: 10,");
  s.client.println("divWidth: 0.1,");
  s.client.println("divLength: 0.7,");
  s.client.println("divColor: '#333333',");
  s.client.println("subDivisions: 2,");
  s.client.println("subLength: 0.2,");
  s.client.println("subWidth: 0.6,");
  s.client.println("subColor: '#666666'");
  s.client.println("},");
  s.client.println("staticLabels: {");
  s.client.println("font: \"16px sans-serif\",  // Specifies font");
  s.client.println("labels: [],  // Print labels at these values");
  s.client.println("color: \"#000000\",  // Optional: Label text color");
  s.client.println("fractionDigits: 0  // Optional: Numerical precision. 0=round off.");
  s.client.println("}");
  s.client.println("};");
  s.client.println("var target = document.getElementById('output-gauge-chart'); // your canvas element");
  s.client.println("GAUGE_OUTPUT = new Gauge(target).setOptions(opts); // create sexy gauge!");
  s.client.println("GAUGE_OUTPUT.maxValue = 100; // set max gauge value");
  s.client.println("GAUGE_OUTPUT.setMinValue(0);  // Prefer setter over GAUGE_OUTPUT.minValue = 0");
  s.client.println("GAUGE_OUTPUT.animationSpeed = 32; // set animation speed (32 is default value)");
  s.client.println("$.getJSON( API_URL + \"/chart_data\", function( data ) {");
  s.client.println("var chart = new Chart(ctx, {");
  s.client.println("// The type of chart we want to create");
  s.client.println("type: 'line',");
  s.client.println("// The data for our dataset");
  s.client.println("data: {");
  s.client.println("labels: Array.from(Array(data.outputData.length).keys()),");
  s.client.println("datasets: [");
  s.client.println("{");
  s.client.println("label: 'Input',");
  s.client.println("// backgroundColor: 'rgb(255, 99, 132)',");
  s.client.println("borderColor: 'rgb(255, 0, 0)',");
  s.client.println("data: data.inputData,");
  s.client.println("yAxisID: 'B',");
  s.client.println("},");
  s.client.println("{");
  s.client.println("label: 'Setpoint',");
  s.client.println("// backgroundColor: 'rgb(255, 99, 132)',");
  s.client.println("borderColor: 'rgb(255, 155, 0)',");
  s.client.println("data: data.setpointData,");
  s.client.println("yAxisID: 'B',");
  s.client.println("},");
  s.client.println("{");
  s.client.println("label: 'Output',");
  s.client.println("// backgroundColor: 'rgb(255, 99, 132)',");
  s.client.println("borderColor: 'rgb(0, 255, 0)',");
  s.client.println("data: data.outputData,");
  s.client.println("yAxisID: 'A',");
  s.client.println("}");
  s.client.println("]");
  s.client.println("},");
  s.client.println("// Configuration options go here");
  s.client.println("options: {");
  s.client.println("scales: {");
  s.client.println("yAxes: [{");
  s.client.println("id: 'A',");
  s.client.println("type: 'linear',");
  s.client.println("position: 'left',");
  s.client.println("ticks: {");
  s.client.println("min: 0,");
  s.client.println("max: 100");
  s.client.println("}");
  s.client.println("}, {");
  s.client.println("id: 'B',");
  s.client.println("type: 'linear',");
  s.client.println("position: 'right',");
  s.client.println("}]");
  s.client.println("}");
  s.client.println("}");
  s.client.println("});");
  s.client.println("function addData() {");
  s.client.println("console.log(\"adding data\");");
  s.client.println("$.getJSON( API_URL + \"/last_chart_data\", function( data ) {");
  s.client.println("console.log(\"adding data cb\");");
  s.client.println("l = chart.data.labels.length;");
  s.client.println("chart.data.labels.push(chart.data.labels[l-1]+1);");
  s.client.println("chart.data.datasets[0].data.push(data.inputData);");
  s.client.println("chart.data.datasets[1].data.push(data.setpointData);");
  s.client.println("chart.data.datasets[2].data.push(data.outputData);");
  s.client.println("if(l >MAX_CHART_LEN){");
  s.client.println("chart.data.labels.shift();");
  s.client.println("chart.data.datasets[0].data.shift();");
  s.client.println("chart.data.datasets[1].data.shift();");
  s.client.println("chart.data.datasets[2].data.shift();");
  s.client.println("}");
  s.client.println("chart.update();");
  s.client.println("});");
  s.client.println("}");
  s.client.println("setInterval(addData, 10000);");
  s.client.println("});");
  s.client.println("});");
  s.client.println("</script>");
  s.client.println("</body>");
  s.client.println("</html>");

}


void chart_data(WebServer& s){
  
  double inputData[DATA_ARRAY_SIZE];
  double outputData[DATA_ARRAY_SIZE];
  double setpointData[DATA_ARRAY_SIZE];

  for(int i = 0; i < DATA_ARRAY_SIZE; i++) {
      inputData[i] = s._inputLog->getItem(i);
    }
  
  for(int i = 0; i < DATA_ARRAY_SIZE; i++) {
      outputData[i] = s._outputLog->getItem(i);
    }
  
  for(int i = 0; i < DATA_ARRAY_SIZE; i++) {
      setpointData[i] = s._setpointLog->getItem(i);
    }



  s.client.println("{");
  s.client.println(" \"inputData\": [");
  for(int i = 0; i < DATA_ARRAY_SIZE; i++) {
      if(inputData[i]!=EMPTY_DATA){
        s.client.println(String(inputData[i]));
        if(i!=DATA_ARRAY_SIZE-1){ s.client.println(",");}
      }
  }
  s.client.println("],");

  s.client.println(" \"outputData\": [");
  for(int i = 0; i < DATA_ARRAY_SIZE; i++) {
      if(outputData[i]!=EMPTY_DATA){
        s.client.println(String(outputData[i]));
        if(i!=DATA_ARRAY_SIZE-1){ s.client.println(",");}
      }
  }
  s.client.println("],");

  s.client.println(" \"setpointData\": [");
  for(int i = 0; i < DATA_ARRAY_SIZE; i++) {
      if(setpointData[i]!=EMPTY_DATA){
        s.client.println(String(setpointData[i]));
        if(i!=DATA_ARRAY_SIZE-1){ s.client.println(",");}
      }
  }
  s.client.println("]");
  s.client.println("}");
}

void last_chart_data(WebServer& s){
  s.client.println("{");
  s.client.println(" \"inputData\": " + String(s._inputLog->getItem(DATA_ARRAY_SIZE-1)) +",");
  s.client.println(" \"outputData\": " + String(s._outputLog->getItem(DATA_ARRAY_SIZE-1)) +",");
  s.client.println(" \"setpointData\": " + String(s._setpointLog->getItem(DATA_ARRAY_SIZE-1)));
  s.client.println("}");
}


void route_test(WebServer& s){
  s.client.println("Hi");

}

void variables_get(WebServer& s){
  s.client.println("");
  s.client.println("{");
  s.client.println("\"Input\" : " + String(*s._Input) + ",");
  s.client.println("\"Settings\" : {");
  s.client.println("\"SetPoint\" : " + String(s._settings->SetPoint) + ",");
  s.client.println("\"PidDirection\" : " + String(s._settings->PidDirection) + ",");
  s.client.println("\"OperatingMode\" : " + String(s._settings->OperatingMode) + ",");
  s.client.println("\"SampleTime\" : " + String(s._settings->SampleTime) + ",");
  s.client.println("\"Kp\" : " + String(s._settings->Kp) + ",");
  s.client.println("\"Ki\" : " + String(s._settings->Ki) + ",");
  s.client.println("\"Kd\" : " + String(s._settings->Kd) + ",");
  s.client.println("\"Kp2\" : " + String(s._settings->Kp2) + ",");
  s.client.println("\"Ki2\" : " + String(s._settings->Ki2) + ",");
  s.client.println("\"Kd2\" : " + String(s._settings->Kd2) + ",");
  s.client.println("\"pid2Band\" : " + String(s._settings->pid2Band) + ",");
  s.client.println("\"dataLogDelay\" : " + String(s._settings->dataLogDelay) + ",");
  s.client.println("\"SettingAdjustmentMultiple\" : " + String(s._settings->SettingAdjustmentMultiple) + "");
  s.client.println("},");
  s.client.println("\"pidState\" : {");
  s.client.println("\"PidOutputSum\" : " + String(s._pidState->PidOutputSum) + ",");
  s.client.println("\"PidOutputP\" : " + String(s._pidState->PidOutputP) + ",");
  s.client.println("\"PidOutputI\" : " + String(s._pidState->PidOutputI) + ",");
  s.client.println("\"PidOutputD\" : " + String(s._pidState->PidOutputD) + ",");
  s.client.println("\"UsePrimaryPID\" : " + String(s._pidState->UsePrimaryPID) + ",");
  s.client.println("\"ManualOutput\" : " + String(s._pidState->ManualOutput) + "");
  s.client.println("}");
  s.client.println("}");
  s.client.println("");



}

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

      Serial.println('Found Matching Route');
      Serial.println(myRoutes[i].requestType + " " + myRoutes[i].route);
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


  client = myserver.available();   // Listen for incoming clients

  if (client) {
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");



    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            parseRoute();
            client.println();
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
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}