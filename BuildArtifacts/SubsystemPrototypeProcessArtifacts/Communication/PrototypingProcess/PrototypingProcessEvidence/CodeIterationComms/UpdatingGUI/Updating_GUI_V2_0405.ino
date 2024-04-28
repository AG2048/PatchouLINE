 #include <SPI.h>
 #include <WiFiNINA.h>
 

int moistureLevels[4] = {0, 1, 2, 3}; //say kevin appends here
int plot_index;
bool newData;
int i = 0;


 char ssid[] = "nano-rp-zipline";        // your network SSID (name)
 char pass[] = "$McZip24!";              // your network password (use for WPA, or use as key for WEP)
 int keyIndex = 0;                       // your network key index number (needed only for WEP)
 

 int status = WL_IDLE_STATUS;
 WiFiServer server(80);
 

void testprint() {
  for (int i=0;i<4;i++){
    Serial.print(moistureLevels[i]);
    Serial.print(" ");
  }
  Serial.println("--------------------------------------------------------------------------------------------");
}


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
         Serial.write(c);                    // print it out the serial monitor
         if (c == '\n') {                    // if the byte is a newline character
 

           // if the current line is blank, you got two newline characters in a row.
           // that's the end of the client HTTP request, so send a response:
           if (currentLine.length() == 0) {
             // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
             // and a content-type so the client knows what's coming, then a blank line:
             client.println("HTTP/1.1 200 OK");
             client.println("Content-type:text/html");
             client.println();

             Serial.println("New client connected");
             
              client.println("<!DOCTYPE html>");
              client.println("<html lang='en'>");
              client.println("<head>");
                  client.println("<meta charset='UTF-8'>");
                  client.println("<meta name='viewport' content='width=device-width, initial-scale=1.0'>");
                  client.println("<title>Heatmap with Gradient Legend</title>");
                  client.println("<style>");
                      client.println("html, body, #container {");
                          client.println("width: 100%;");
                          client.println("height: 100%;");
                          client.println("margin: 0;");
                          client.println("padding: 0;");
                      client.println("}");

                      client.println(".heatmap {");
                          client.println("display: grid;");
                          client.println("grid-template-columns: repeat(2, 1fr);");
                          client.println("grid-template-rows: repeat(2, 1fr);");
                          client.println("gap: 2px;");
                          client.println("width: 400px;");
                          client.println("height: 400px;");
                      client.println("}");

                      client.println(".plot {");
                          client.println("border: 1px solid #fff;");
                          client.println("position: relative;");
                      client.println("}");

                      client.println(".plot:hover::before {");
                          client.println("content: attr(data-heat);");
                          client.println("position: absolute;");
                          client.println("top: 50px;");
                          client.println("left: 50%;");
                          client.println("transform: translateX(-50%);");
                          client.println("background-color: rgba(255, 255, 255, 0.8);");
                          client.println("padding: 5px;");
                          client.println("border-radius: 5px;");
                          client.println("font-size: 12px;");
                      client.println("}");

                      client.println("#legend {");
                          client.println("margin-top: 20px;");
                          client.println("text-align: center;");
                          client.println("position: relative;");
                          client.println("width: 200px;");
                          client.println("height: 30px;");
                      client.println("}");

                      client.println(".gradient {");
                          client.println("position: absolute;");
                          client.println("width: 100%;");
                          client.println("height: 100%;");
                          client.println("background-image: linear-gradient(to right, #FFA500, #B0D8A4, #6495ED);"); //MIGHT CHANGE COLOURS
                      client.println("}");

                      client.println(".legend-labels {");
                          client.println("position: absolute;");
                          client.println("top: 35px;");
                          client.println("display: flex;");
                          client.println("justify-content: space-between;");
                          client.println("width: 100%;");
                      client.println("}");

                      client.println(".legend-label {");
                          client.println("flex: 1;");
                          client.println("text-align: center;");
                          client.println("font-size: 12px;");
                      client.println("}");

                      client.println(".legend-color {");
                          client.println("width: 20px;");
                          client.println("height: 20px;");
                          client.println("display: inline-block;");
                          client.println("margin-right: 5px;");
                          client.println("border: 1px solid #ccc;");
                          client.println("vertical-align: middle;");
                      client.println("}");
                  client.println("</style>");
              client.println("</head>");
              client.println("<body>");
                  if(newData) {
                      client.println("<h1><center>Moisture Levels (New data at plot index " + String(plot_index) + "</center></h1>");
                      newData = false;
                  }
                  else client.println("<h1><center>Moisture Levels</center></h1>");

                  //heatmap
                  client.println("<div id='container' class='heatmap'></div>");
                  //legend
                  client.println("<center>");
                  client.println("<h2>Legend</h2>");
                  client.println("<div id='legend'>");
                
                  client.println("<div class='gradient'></div>");
                  client.println("<div class='legend-labels'>");
                      client.println("<div class='legend-label'>Dry</div>");
                      client.println("<div class='legend-label'>Nice</div>");
                      client.println("<div class='legend-label'>Wet</div>");
                  client.println("</div>");
                
              client.println("</div>");
              client.println("</center>");

                  client.println("<script>");
                      client.println("document.addEventListener('DOMContentLoaded', function() {");
                          // Plot dimensions
                          client.println("const xCoord = 2;");
                          client.println("const yCoord = 2;");
                          
                          // Container for heatmap plots
                          client.println("const container = document.getElementById('container');");
                          
                          // Define color stops and corresponding moisture levels/ Dry Nice Wet
                          client.println("const colorStops = [");
                              client.println("{ color: '#FFA500', moistureLevel: 0 },");
                              client.println("{ color: '#B0D8A4', moistureLevel: 660 },");
                              client.println("{ color: '#6495ED', moistureLevel: 850 } ");
                          client.println("];");
                          
                          // Generate heatmap
                          String moistureLevelsToString = "[";
                          for(int j = 0; j < 3; j++){
                            moistureLevelsToString = moistureLevelsToString + moistureLevels[j] + ", ";
                          }
                          moistureLevelsToString = moistureLevelsToString + moistureLevels[3] + "]";
                          client.println("const moistureLevels = " + moistureLevelsToString + ";");
                          client.println("for (let i = 0; i < xCoord * yCoord; i++) {");
                              client.println("const plot = document.createElement('div');");
                              client.println("plot.classList.add('plot');");
                              client.println("plot.dataset.x = (i % xCoord) + 1;");
                              client.println("plot.dataset.y = Math.floor(i / yCoord) + 1;");
                              
                              client.println("const moistureLevel = moistureLevels[i];");
                              client.println("const color = interpolateColor(moistureLevel, colorStops);");
                              
                              client.println("plot.style.backgroundColor = color;");
                              //old
                              //client.println("plot.dataset.heat = 'Heat: ${moistureLevel}';");
                              client.println("plot.dataset.heat = 'Heat: ' + moistureLevel;");
                              
                              // Add event listener for click
                              client.println("plot.addEventListener('click', function() {");
                                  //client.println("alert('Heat Measurement: ${moistureLevel}\nMessage: ${getMessage(moistureLevel)}');");
                                  client.println("alert('Heat Measurement: ' + moistureLevel + '\\nMessage: ' + getMessage(moistureLevel));");
                              client.println("});");

                              client.println("container.appendChild(plot);");
                          client.println("}");
                      client.println("});");
                      
                      // Function to interpolate color based on moisture level
                      client.println("function interpolateColor(moistureLevel, colorStops) {");
                          client.println("for (let i = 1; i < colorStops.length; i++) {");
                              client.println("const prevStop = colorStops[i - 1];");
                              client.println("const currentStop = colorStops[i];");
                              
                              client.println("if (moistureLevel <= currentStop.moistureLevel) {");
                                  client.println("const prevColor = prevStop.color;");
                                  client.println("const currentColor = currentStop.color;");
                                  client.println("const prevMoisture = prevStop.moistureLevel;");
                                  client.println("const currentMoisture = currentStop.moistureLevel;");
                                  
                                  // Interpolate between previous and current colors
                                  client.println("const t = (moistureLevel - prevMoisture) / (currentMoisture - prevMoisture);");
                                  client.println("const interpolatedColor = interpolate(prevColor, currentColor, t);");
                                  client.println("return interpolatedColor;");
                              client.println("}");
                          client.println("}");
                          
                          // Default color (in case moisture level exceeds all stops) -> blue
                          client.println("return colorStops[colorStops.length - 1].color;");
                      client.println("}");
                      
                      // Function to interpolate between two colors
                      client.println("function interpolate(color1, color2, t) {");
                          client.println("const r1 = parseInt(color1.substr(1, 2), 16);");
                          client.println("const g1 = parseInt(color1.substr(3, 2), 16);");
                          client.println("const b1 = parseInt(color1.substr(5, 2), 16);");
                          client.println("const r2 = parseInt(color2.substr(1, 2), 16);");
                          client.println("const g2 = parseInt(color2.substr(3, 2), 16);");
                          client.println("const b2 = parseInt(color2.substr(5, 2), 16);");
                          
                          client.println("const r = Math.round(r1 + (r2 - r1) * t);");
                          client.println("const g = Math.round(g1 + (g2 - g1) * t);");
                          client.println("const b = Math.round(b1 + (b2 - b1) * t);");
                          
                          client.println("return '#' + componentToHex(r) + componentToHex(g) + componentToHex(b);");
                      client.println("}");
                      
                      client.println("function componentToHex(c) {");
                          client.println("const hex = c.toString(16);");
                          client.println("return hex.length == 1 ? '0' + hex : hex;");
                      client.println("}");

                      client.println("function getMessage(heat) {");
                          client.println("if (heat < 650) {");
                              client.println("return 'This plot is dry. The patchouli plants are in need of water!';");
                          client.println("} else if (heat <= 680) {");
                              client.println("return 'This plot has sufficient moisture. It does not need any more water for now!';");
                          client.println("} else {");
                              client.println("return 'This plot is overwatered.';");
                          client.println("}");
                      client.println("}");

                  client.println("</script>");
              client.println("</body>");
              client.println("</html>");

                
             // The HTTP response ends with another blank line:
             client.println();

              if (i<4){
                moistureLevels[i] = 200 + i;
                i++;
                testprint();
                newData = true;
              }
              if (i>=4) {
                moistureLevels[0] = moistureLevels[0] + 200;
                testprint();
                newData = true;
              }
             // break out of the while loop:
             break;
           } else {    // if you got a newline, then clear currentLine:
             currentLine = "";
           }
         } else if (c != '\r') {  // if you got anything else but a carriage return character,
           currentLine += c;      // add it to the end of the currentLine
         }
         // Check to see if the client request was /X
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