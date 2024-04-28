/*
intended overall code body and structure with cases
*/

#include <SPI.h>
#include <WiFiNINA.h>


char ssid[] = "nano-rp-zipline";        // network name (SSID)
char pass[] = "$McZip24!";              // network password (use for WPA for us, also can use as key for WEP)
int keyIndex = 0;                       // network key index number (only for WEP)

int state = 0;

// slider values for inputs
int x_width_value;
int y_length_value;
int num_rows_value;
int x_spacing_value;
int mode_value; //must be int for slider, convert to char later
// automatic more sliders
int x_dist_value; // frequency
int row_dist_value; //frequency

int status = WL_IDLE_STATUS;
WiFiServer server(80);


void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  
  Serial.println("Access Point Web Server");

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }


  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }


  // by default the local IP address will be 192.168.4.1
  // you can override it with the following:
  // WiFi.config(IPAddress(10, 0, 0, 1));


  // print the network name (SSID);
  Serial.print("Creating access point named: ");
  Serial.println(ssid);


  // Create open network. Change this line if you want to create an WEP network:
  status = WiFi.beginAP(ssid, pass);
  if (status != WL_AP_LISTENING) {
    Serial.println("Creating access point failed");
    // don't continue
    while (true);
  }


  // wait 10 seconds for connection:
  delay(10000);


  // start the web server on port 80
  server.begin();


  // you're connected now, so print out the status
  printWiFiStatus();
}

int getValueFromQueryString(String queryString, String parameter) {
  // identifier that separates the keys (part of url)
  String key = parameter + "=";
  // index to find position
  int keyIndex = queryString.indexOf(key);
  
  if (keyIndex != -1) {
      //store actual position
      int startIndex = keyIndex + key.length();
      // find the end of the string OR the next "=" key that separates them
      int endIndex = queryString.indexOf('&', startIndex);
      if (endIndex == -1) { //-1 means the end
          endIndex = queryString.length();
      }
      String valueString = queryString.substring(startIndex, endIndex); //take out value based on index
      return valueString.toInt(); //we know it's a value
  }
  // in case it is not found
  return 0;
}

void processInitInput(WiFiClient client) {
  Serial.println("enter process init input");

  // Send HTML content directly in the Arduino sketch
  client.println("<!DOCTYPE html>");
  client.println("<html>");
  client.println("<head>");
  client.println("<meta charset='UTF-8'>");
  client.println("<meta name='viewport' content='width=device-width, initial-scale=1.0'>");
  client.println("</head>");
  client.println("<body>");

  client.print("<style>");
  client.print(".container {margin: 0 auto; text-align: center;}"); //change margin-top  margin-top: 100px;
  client.print(".slideContainer {width: 100%;}");
  client.print("button {color: white; width: 100px; height: 100px;");
  client.print("border-radius: 50%; margin: 20px; border: none; font-size: 20px; outline: none; transition: all 0.2s;}");
  client.print(".buttonStyle{background-color: grey;}");
  client.print("</style>");

  //client.print("<style>");
  //client.print(".container {margin: 0 auto; text-align: center; margin-top: 100px;}");
  //client.print("button {color: white; width: 100px; height: 100px;");

  //client.print("border-radius: 50%; margin: 20px; border: none; font-size: 20px; outline: none; transition: all 0.2s;}");
  
  //client.print(".off{background-color: grey;}");
  //client.print("button:hover{cursor: pointer; opacity: 0.7;}"); //dont have this
  //client.print("</style>");

  client.println("<h1>Patchouli Farm Parameters</h1>");

  //assuming min 0.6 apart so max 84*0.6 = 140 rows
  client.print("<div class='slideContainer'>");
  client.println("<h2>Plot Width (X): <span id='x_width'>42</span></h2>"); //THIS IS THE PART SEEN ON THE SCREEN
  client.println("0<input type='range' min='0' max='84' value='42' id='slider1' list='XYmarkers' />84"); // slider for x width

  client.println("<h2>Plot Length (Y): <span id='y_length'>42</span></h2>"); 
  client.println("0<input type='range' min='0' max='84' value='42' id='slider2' list='XYmarkers' />84"); // slider for y length

  client.println("<h2>Number of Rows: <span id='num_rows'>70</span></h2>"); 
  client.println("0<input type='range' min='0' max='140' value='70' id='slider3' list='NUMmarkers' />140"); // slider for number of rows

  client.println("<h2>X Spacing [cm]: <span id='x_spacing'>100</span></h2>"); 
  client.println("60<input type='range' min='60' max='120' value='100' id='slider4' list='NUMmarkers' />120"); // slider for x distance

  client.println("<h2>Mode (Automatic or Manual): <span id='modeAM'>N</span></h2>");
  client.println("A<input type='range' min='0' max='1' value='0' id='slider5'>M"); // slider for mode

  client.println("</div>");

  client.println("<datalist id='XYmarkers'>");
  client.println("<option value='0'></option>");
  client.println("<option value='21'></option>");
  client.println("<option value='42'></option>");
  client.println("<option value='63'></option>");
  client.println("<option value='84'></option>");
  client.println("</datalist>");


  client.println("<datalist id='NUMmarkers'>");
  client.println("<option value='0'></option>");
  client.println("<option value='10'></option>");
  client.println("<option value='20'></option>");
  client.println("<option value='30'></option>");
  client.println("<option value='40'></option>");
  client.println("<option value='50'></option>");
  client.println("<option value='60'></option>");
  client.println("<option value='70'></option>");
  client.println("<option value='80'></option>");
  client.println("<option value='90'></option>");
  client.println("<option value='100'></option>");
  client.println("<option value='110'></option>");
  client.println("<option value='120'></option>");
  client.println("<option value='130'></option>");
  client.println("<option value='140'></option>");
  client.println("</datalist>");

  //submit button
  client.print("<div class='container'>");
  client.println("<button class = 'buttonStyle' onclick='sendSliderValues()'>SUBMIT</button>"); 
  client.println("</div>");

  //function to retrieve slider values
  client.println("<script>");
  //call the function to extract the values
  client.println("function sendSliderValues() {");
  //get elements by ID (cur value)
  client.println("var x_width_value = document.getElementById('slider1').value;");
  client.println("var y_length_value = document.getElementById('slider2').value;");
  client.println("var num_rows_value = document.getElementById('slider3').value;");
  client.println("var x_spacing_value = document.getElementById('slider4').value;");
  client.println("var mode_value = document.getElementById('slider5').value;");
  client.println("var xhttp = new XMLHttpRequest();");
  client.println("xhttp.onreadystatechange = function() {");
  client.println("if (this.readyState == 4 && this.status == 200) {");
  //update the text with the new values
  client.println("document.getElementById('x_width').innerText = x_width_value;"); // update slider 1 value
  client.println("document.getElementById('y_length').innerText = y_length_value;"); // update slider 2 value
  client.println("document.getElementById('num_rows').innerText = num_rows_value;"); // update slider 3 value
  client.println("document.getElementById('x_spacing').innerText = x_spacing_value;"); // update slider 4 value

  client.println("if (mode_value == 0) {");
  client.println("document.getElementById('modeAM').innerText = 'A';"); // update slider 5 value
  client.println("} else {"); //else if (mode_value == 1)
  client.println("document.getElementById('modeAM').innerText = 'M';"); // update slider 5 value
  client.println("}"); 

  client.println("}");
  client.println("};");
  // Send all five slider values to the server, this is the "url" separating values with "="
  client.println("xhttp.open('GET', '/inputSliders?slider1=' + x_width_value + '&slider2=' + y_length_value + '&slider3=' + num_rows_value + '&slider4=' + x_spacing_value + '&slider5=' + mode_value, true);");
  client.println("xhttp.send();");
  client.println("}");
  client.println("</script>");
  client.println("</body>");
  client.println("</html>");
  Serial.println("exit process init input");
}


void processAutoInput(WiFiClient client) {
  Serial.println("enter process auto input");

  // Send HTML content directly in the Arduino sketch
  client.println("<!DOCTYPE html>");
  client.println("<html>");
  client.println("<head>");
  client.println("<meta charset='UTF-8'>");
  client.println("<meta name='viewport' content='width=device-width, initial-scale=1.0'>");
  client.println("</head>");
  client.println("<body>");

  client.print("<style>");
  client.print(".container {margin: 0 auto; text-align: center;}"); //change margin-top  margin-top: 100px;
  client.print(".slideContainer {width: 100%;}");
  client.print("button {color: white; width: 100px; height: 100px;");
  client.print("border-radius: 50%; margin: 20px; border: none; font-size: 20px; outline: none; transition: all 0.2s;}");
  client.print(".buttonStyle{background-color: grey;}");
  client.print("</style>");

  client.println("<h1>Automatic Mode Parameters</h1>");

  //assuming min 0.6 apart so max 84*0.6 = 140 rows
  client.print("<div class='slideContainer'>");
  client.println("<h2>X Distance Between Plants [m]: <span id='x_dist'>42</span></h2>"); //THIS IS THE PART SEEN ON THE SCREEN
  client.println("0<input type='range' min='0' max='84' value='42' id='slider6' list='XYmarkers' />84"); // slider for x width

  client.println("<h2>Row Frequency: <span id='row_dist'>70</span></h2>"); 
  client.println("0<input type='range' min='0' max='140' value='1' id='slider7' list='NUMmarkers' />140"); // slider for y length

  client.println("</div>");

  client.println("<datalist id='XYmarkers'>");
  client.println("<option value='0'></option>");
  client.println("<option value='21'></option>");
  client.println("<option value='42'></option>");
  client.println("<option value='63'></option>");
  client.println("<option value='84'></option>");
  client.println("</datalist>");


  client.println("<datalist id='NUMmarkers'>");
  client.println("<option value='0'></option>");
  client.println("<option value='10'></option>");
  client.println("<option value='20'></option>");
  client.println("<option value='30'></option>");
  client.println("<option value='40'></option>");
  client.println("<option value='50'></option>");
  client.println("<option value='60'></option>");
  client.println("<option value='70'></option>");
  client.println("<option value='80'></option>");
  client.println("<option value='90'></option>");
  client.println("<option value='100'></option>");
  client.println("<option value='110'></option>");
  client.println("<option value='120'></option>");
  client.println("<option value='130'></option>");
  client.println("<option value='140'></option>");
  client.println("</datalist>");

  //submit button
  client.print("<div class='container'>");
  client.println("<button class = 'buttonStyle' onclick='sendSliderValues()'>SUBMIT</button>"); 
  client.println("</div>");

  //function to retrieve slider values
  client.println("<script>");
  //call the function to extract the values
  client.println("function sendSliderValues() {");
  //get elements by ID (cur value)
  client.println("var x_dist_value = document.getElementById('slider6').value;");
  client.println("var row_dist_value = document.getElementById('slider7').value;");
  client.println("var xhttp = new XMLHttpRequest();");
  client.println("xhttp.onreadystatechange = function() {");
  client.println("if (this.readyState == 4 && this.status == 200) {");
  //update the text with the new values
  client.println("document.getElementById('x_dist').innerText = x_dist_value;"); // update slider 6 value
  client.println("document.getElementById('row_dist').innerText = row_dist_value;"); // update slider 7 value

  client.println("}");
  client.println("};");
  // Send all five slider values to the server, this is the "url" separating values with "="
  client.println("xhttp.open('GET', '/autoSliders?slider6=' + x_dist_value + '&slider7=' + row_dist_value, true);");
  client.println("xhttp.send();");
  client.println("}");
  client.println("</script>");
  client.println("</body>");
  client.println("</html>");
}


void loop() {
  // compare the previous status to the current status
  if (status != WiFi.status()) {
    // it has changed update the variable
    status = WiFi.status();


    if (status == WL_AP_CONNECTED) {
      // a device has connected to the AP
      Serial.println("Device connected to AP");
    } else {
      // a device has disconnected from the AP, and we are back in listening mode
      Serial.println("Device disconnected from AP");
    }
  }
  
  WiFiClient client = server.available();   // listen for incoming clients


  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);    
      
        if (c == '\n') {                    // if the byte is a newline character

          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            
            //always start like this MIGHT HAVE TO MOVE WITHIN THE HTML PRINTS TO REFRESH?
            //right now if they're outside it still works, just have to manually refresh page
            //same if they are inside functions...hmm....
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            Serial.println("New client connected");
            

            switch (state) {
            case 0: // initial page, farmer plot parameters
              Serial.println("enter case 0");
              processInitInput(client);
              Serial.println(state);
              break;
            case 1: // automatic case page      
              Serial.println("enter case 1");
              processAutoInput(client);
              Serial.println(state);
              break;
            case 2: // other cases...
              Serial.println("cases done, enter case 2");
              break;
            }

            // end response
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
        // CASE 1 RETRIEVING DATA FROM 5 SLIDERS (INPUT CASE)
        if ((currentLine.startsWith("GET /inputSliders")) && (currentLine.endsWith("HTTP/1.1"))) { 
          Serial.println("input sliders received-----------------------------------------------");            
          state = 1;
          Serial.println(currentLine);
          if (currentLine.indexOf("GET /inputSliders?") != -1 ) {
            int queryStart = currentLine.indexOf('?');
            String queryString = currentLine.substring(queryStart + 1);
            // call the function that extracts all of the specific values in order from the url
            // these will be stored and translated to something else
            x_width_value = getValueFromQueryString(queryString, "slider1");
            y_length_value = getValueFromQueryString(queryString, "slider2");
            num_rows_value = getValueFromQueryString(queryString, "slider3");
            x_spacing_value = getValueFromQueryString(queryString, "slider4");
            mode_value = getValueFromQueryString(queryString, "slider5");

            Serial.println(x_width_value);
            Serial.print("exit retrieving input values");
          }
        }
        // CASE 2 RETRIEVING DATA FROM 2 SLIDERS (AUTOMATIC CASE)
        if ((currentLine.startsWith("GET /autoSliders")) && (currentLine.endsWith("HTTP/1.1"))) { 
          Serial.println("auto sliders received-----------------------------------------------");            
          state = 2;
          Serial.println(currentLine);
          if (currentLine.indexOf("GET /autoSliders?") != -1 ) {
            // separated by ?
            int queryStart = currentLine.indexOf('?');
            String queryString = currentLine.substring(queryStart + 1);

            x_dist_value = getValueFromQueryString(queryString, "slider6");
            row_dist_value = getValueFromQueryString(queryString, "slider7");
            
            Serial.println(x_dist_value);
            Serial.print("exit retrieving auto values");
          }
        }
        // future cases
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}


void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());


  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);


  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);


} 