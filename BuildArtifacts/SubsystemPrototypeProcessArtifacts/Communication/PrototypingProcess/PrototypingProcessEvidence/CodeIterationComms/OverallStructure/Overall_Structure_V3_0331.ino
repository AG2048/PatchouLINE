/*
this code is just a test to determine how to switch pages within cases
and also extract information from xhttp requests.
it succesfully did that on basic button cases
next step: implement this with the user input and automatic cases already written
(those are the "first_input_sliders" files) combined overall structure
*/


#include <SPI.h>
#include <WiFiNINA.h>


char ssid[] = "nano-rp-zipline";  // your network SSID (name)
char pass[] = "$McZip24!";        // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                 // your network key index number (needed only for WEP)

int state = 0;

int status = WL_IDLE_STATUS;
WiFiServer server(80);


void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }


  Serial.println("Access Point Web Server");

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true)
      ;
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
    while (true)
      ;
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
          endIndex = queryString.length(); //i think i should have - 2 here because im adding \n
      }
      String valueString = queryString.substring(startIndex, endIndex); //take out value based on index
      return valueString.toInt(); //we know it's a value
  }
  // in case it is not found
  return 0;
}



//i think i should just copy and past the entire wifi setup within this function. like at least the while(client.available) part.

void button1(WiFiClient client) {

  Serial.println("enter button1");

  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();

  client.println("<!DOCTYPE html>");
  client.println("<html>");
  client.println("<head>");
  client.println("<meta charset='UTF-8'>");
  client.println("<meta name='viewport' content='width=device-width, initial-scale=1.0'>");
  client.println("</head>");
  client.println("<body>");

  client.print("<style>");
  client.print(".container {margin: 0 auto; text-align: center;}");  //change margin-top  margin-top: 100px;
  client.print(".slideContainer {width: 100%;}");
  client.print("button {color: white; width: 100px; height: 100px;");
  client.print("border-radius: 50%; margin: 20px; border: none; font-size: 20px; outline: none; transition: all 0.2s;}");
  client.print(".buttonStyle{background-color: grey;}");
  client.print("</style>");

  client.println("<h1>Screen 1</h1>");

  client.println("<div class='container'>");
  client.println("<button class = 'buttonStyle' onclick='buttonSubmit1()'>SUBMIT</button>");
  client.println("</div>");
  
  client.println("<script>");
  client.println("console.log('test');");
  client.println("function buttonSubmit1() {");
  client.println("var xhttp = new XMLHttpRequest();");
  client.println("xhttp.onreadystatechange = function() {");
  client.println("if (this.readyState == 4 && this.status == 200) {");
  client.println("}");
  client.println("};");
  client.println("xhttp.open('GET', '/button1pressed?test1=' + 2 + '&test2=' + 4, true);");
  client.println("xhttp.send();");  
  client.println("console.log('button1pressed was sent');");
  client.println("}");
  client.println("</script>");

  Serial.println("exit button1");

/*
  while (client.connected()) {
    if (client.available()) { //so this just doens't work it says no client available every single time.......and gets stuck
      // read an incoming byte from the server and print it to serial monitor:
      char c = client.read();
      Serial.write(c);
      Serial.println("help");
    }
  }
  */

/*  for(int i=0; i<10; i++){

    String request = client.readString(); //used to be \r
    Serial.println("-------------------------------");
    Serial.println(request);
    Serial.println("--------------------------------------------");
  }
  */

  //if (request.indexOf("GET /button1pressed") != -1) {
  //  Serial.println("entered request for button1");
  //  state = 1;
  //}
}



void button2(WiFiClient client) {

  Serial.println("enter button2");

  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();

  client.println("<!DOCTYPE html>");
  client.println("<html>");
  client.println("<head>");
  client.println("<meta charset='UTF-8'>");
  client.println("<meta name='viewport' content='width=device-width, initial-scale=1.0'>");
  client.println("</head>");
  client.println("<body>");

  client.print("<style>");
  client.print(".container {margin: 0 auto; text-align: center;}");  //change margin-top  margin-top: 100px;
  client.print(".slideContainer {width: 100%;}");
  client.print("button {color: white; width: 100px; height: 100px;");
  client.print("border-radius: 50%; margin: 20px; border: none; font-size: 20px; outline: none; transition: all 0.2s;}");
  client.print(".buttonStyle{background-color: grey;}");
  client.print("</style>");

  client.println("<h1>Screen 2</h1>");

  client.println("<div class='container'>");
  client.println("<button class = 'buttonStyle' onclick='buttonSubmit2()'>SUBMIT</button>");
  client.println("</div>");

  client.println("<script>");
  client.println("console.log('test2');");
  client.println("function buttonSubmit2() {");
  client.println("var xhttp = new XMLHttpRequest();");
  client.println("xhttp.onreadystatechange = function() {");
  client.println("if (this.readyState == 4 && this.status == 200) {");
  client.println("}");
  client.println("};");
  client.println("xhttp.open('GET', '/button2pressed', true);");
  client.println("xhttp.send();");
  client.println("console.log('button2pressed was sent');");
  client.println("}");
  client.println("</script>");

  Serial.println("exit button2");
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

  WiFiClient client = server.available();  // listen for incoming clients


  if (client) {                    // if you get a client,
    Serial.println("new client");  // print a message out the serial port
    String currentLine = "";       // make a String to hold incoming data from the client
    while (client.connected()) {   // loop while the client's connected
      if (client.available()) {    // if there's bytes to read from the client,
        char c = client.read();    // read a byte, then
        Serial.write(c);

        if (c == '\n') {  // if the byte is a newline character

          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {

            //always start like this
            Serial.println("New client connected");


            switch (state) {
              case 0:
                Serial.println("enter case 0");
                button1(client);
                Serial.println(state);
                break;
              case 1:
                Serial.println("enter case 1");
                button2(client);
                Serial.println(state);
                break;
              case 2:
                Serial.println("testing state 2");
                //because it displays no html, it goes to a black screen.
            }

            client.println();
            // break out of the while loop:
            break;
          } else {  // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

// processing the xhttp into variables within main code by readig querystrings
        if ((currentLine.startsWith("GET /button1pressed")) && (currentLine.endsWith("HTTP/1.1"))) { 
          Serial.println("BUTTON ONE RECEIVED_----------------------------");
          state = 1;
          Serial.println("enter process init input");
          Serial.println(currentLine);
          if (currentLine.indexOf("GET /button1pressed?") != -1) { //currentLine only contains GET /button1pressed not the rest bc too fast.
            // separated by ?
            // explanation: https://arduinogetstarted.com/tutorials/arduino-http-request#google_vignette
            int queryStart = currentLine.indexOf('?'); //between the pathname and query string
            String queryString = currentLine.substring(queryStart + 1);
            int TESTING = getValueFromQueryString(queryString, "test1"); //function to extract values from query string
            int TESTING2 = getValueFromQueryString(queryString, "test2");
            //show it works!
            Serial.println(TESTING);
            Serial.println(TESTING2);

          }
        }
        if (currentLine.endsWith("GET /button2pressed")) {
           Serial.println("BUTTON TWO RECEIVED_----------------------------"); //didn't add that yet but that's alright just a test
           state = 2;
        }
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