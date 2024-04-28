 #include <SPI.h>
 #include <WiFiNINA.h>
 

 char ssid[] = "nano-rp-zipline";        // your network SSID (name)
 char pass[] = "$McZip24!";    // your network password (use for WPA, or use as key for WEP)
 int keyIndex = 0;                // your network key index number (needed only for WEP)
 

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
             /*
    client.println("<!DOCTYPE html>");
    client.println("<html>");
    client.println("<head>");
    client.println("<title>Arduino Nano RP2040 Heatmap</title>");
    client.println("<script src='anychart-core.min.js'></script>");
    client.println("<script src='anychart-heatmap.min.js'></script>");
    client.println("<script src='anychart-base.min.js'></script>");
    client.println("<Content-Type: text/javascript>");
    client.println("</head>");
    client.println("<body>");
    client.println("<div id='container' style='width: 600px; height: 400px;'></div>");
    client.println("<script>");
    client.println("anychart.onDocumentReady(function() {");
    client.println("var data = [");
    client.println("  {x: 0, y: 0, value: 10},");
    client.println("  {x: 1, y: 0, value: 20},");
    client.println("  {x: 0, y: 1, value: 30},");
    client.println("  {x: 1, y: 1, value: 40}");
    client.println("];");
    client.println("var chart = anychart.heatMap(data);");
    client.println("chart.container('container');");
    client.println("chart.draw();");
    client.println("});");
    client.println("</script>");
    client.println("</body>");
    client.println("</html>");
  */
                ///*
                client.println("<!DOCTYPE html>");
                client.println("<html>");
                client.println("<head>");
                client.println("<meta charset='UTF-8'>");
                client.println("<meta name='viewport' content='width=device-width, initial-scale=1.0'>");
                client.println("<title>Heatmap in JavaScript</title>");
                //client.println("<script src='https://cdn.anychart.com/releases/8.11.0/js/anychart-core.min.js'></script>");
                //client.println("<script src='https://cdn.anychart.com/releases/8.11.0/js/anychart-heatmap.min.js'></script>");
                client.println("<script type='text/javascript' src='anychart-core.min.js'></script>");
                client.println("<script type='text/javascript' src='anychart-heatmap.min.js'></script>");
                //client.println("<script src='anychart-base.min.js'></script>");  


                client.println("<style type='text/css'>"); 
                client.println("html, body, #container {"); 
                  client.println("width: 100%; margin: 0; padding: 0;"); 
                client.println("}"); 
                client.println("</style>");
                client.println("</head>");

                
                client.println("<body>");
                client.println("<div id='container'></div>");
                client.println("<script>");
                client.println("anychart.onDocumentReady(function () {");
                      // create a heatmap
                  client.println("var chart = anychart.heatMap(getData());");
                      // set a custom color scale
                  client.println("var colorScale = anychart.scales.ordinalColor();");
                  client.println("colorScale.ranges([");
                  client.println("{ less: 649, color: '#FFA500' }, { from: 650, to: 680, color: '#B0D8A4' }, { greater: 680, color: '#6495ED' } ]);"); //dry nice wet
                  client.println("chart.colorScale(colorScale);");
                      // style the coloring in the hovered state
                  client.println("chart");
                    client.println(".hovered()");
                    client.println(".fill(function () {");
                    client.println("return anychart.color.darken(this.sourceColor, 0.25);");
                    client.println("});");
                      // hide the labels
                  client.println("chart.labels(false);");
                      // customize the axes
                  client.println("chart.xAxis().stroke(null);");
                  client.println("chart.yAxis().stroke(null);");
                  client.println("chart.yAxis().labels().padding([0, 10, 0, 0]);");
                  client.println("chart.xAxis().labels().padding([0, 0, 10, 0]);");
                      // set the tooltip
                  client.println("chart.tooltip().title().useHtml(true);");
                  client.println("chart");
                    client.println(".tooltip()");
                    client.println(".useHtml(true)");
                    client.println(".titleFormat(function () {");
                      client.println("return 'Accidents - ' + this.heat;");
                    client.println("})");
                    
                    //client.println(".format(function () {");
                      //client.println("return (");
                        //client.println("'<span style=\"color: #CECECE\">Day: </span>' + this.x + '<br/>' + '<span style=\"color: #CECECE\">Time: </span>' + this.y");
                      // name the heatmap
                  
                  client.println("chart");
                    client.println(".title()");
                    client.println(".enabled(true)");
                    client.println(".text('Soil Moisture Levels')");
                    client.println(".padding([0, 0, 20, 0]);");
                      // set the container for the heatmap
                  client.println("chart.container('container');");
                      // draw the heatmap
                  client.println("chart.draw();");
                client.println("});");
                Serial.println("right after draw statement"); //------------------------
                    // add the data
                client.println("function getData() {");
                        // these to uptate
                        // if has not been declared, then run the declared 
                    client.println("var x_coord = 6;");
                    client.println("var y_coord = 6;");
                    client.println("var moisture_levels = [1, 650, 650, 681, 234, 234,456, 3456, 654, 456, 656, 456, 456, 65, 765, 456 , 675, 65, 67, 657];");
                    client.println("last_value = moisture_levels.length;");
                    client.println("everything = [];");
                    client.println("new_coord = {x: 1, y: 2, heat: 3 };");
                    client.println("var datum_number = 0;");
                    client.println("declared = false;");
                        
                        // run this at first
                    client.println("if (declared == false){");
                    client.println("for (var i = 1; i <=x_coord; i++){");
                        client.println("for (var j = 1; j <= y_coord; j++){");
                            // need to do the modulo 
                            client.println("datum_number = (j-1)*x_coord + i;");
                            client.println("new_coord = {x: i, y: j, heat: moisture_levels[datum_number-1]};");
                            client.println("everything.push(new_coord);");
                        client.println("}");
                    client.println("}");
                    client.println("declared == true;");
                    client.println("}else{");
                        client.println("last_value = last_value + 1; ");
                        client.println("new_coord = {x: i, y: j, heat: moisture_levels[last_value]};");
                        client.println("everything.push(new_coord);");
                    client.println("}");
                        // then if it 
                  client.println("return everything;");
                client.println("}");
                
                client.println("</script>");
                client.println("</body>");

                client.println("<h1>testing</h1>");
                
                client.println("</html>");
                Serial.println("exit displaying GUI");
                //*/
                
             // The HTTP response ends with another blank line:
             client.println();
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