/*
FINAL INTEGRATION
last update: april 6th, 2024
current version: V4
working on: coordinate update for the alert
*/

#include <SPI.h>
#include <WiFiNINA.h>

//COMMS GLOBAL VARIABLES==========================================================================================================================
char ssid[] = "nano-rp-zipline";  // network name (SSID)
char pass[] = "$McZip24!";        // network password (use for WPA for us, also can use as key for WEP)
int keyIndex = 0;                 // network key index number (only for WEP)

//communication with the printGUI
int moistureLevels[4] = { 0, 0, 0, 0 };
int plot_index = 0;
bool newData = false;

int status = WL_IDLE_STATUS;
WiFiServer server(80);

int state = 0;
int big_state = 0;

// slider values for inputs
int x_width_value;
int y_length_value;
int num_rows_value;
int x_spacing_value;
int mode_value;
// automatic more sliders. set to 0 bc they should be 0 in manual mode
int x_dist_value = 0;    // frequency
int row_dist_value = 0;  //frequency
// ignore manual mode for now

//received data (NOT SURE IF STILL DOING THIS BUT NEED IT FOR STRUCT AND STORAGE RN)
int x_receive;
int y_receive;
int m_receive;
int n_receive;
int p_receive;
int k_receive;

int storage[2][2][4];  //will be a maximum size defined by us. this is for prototyping
//actual dimensions will be storage[83][83][4]

int x;
int y;
int m;
int n;
int p;
int k;

//SENSOR GLOBAL VARIABLES=========================================================================================================================
typedef enum {
  RETRACTED,
  EXTENDING,
  EXTENDED,
  MEASURING,
  RETRACTING,
  NONE,
  NUM_STATES
} sensorState;

// Keep track of both current and previous may be useful
sensorState currentState = RETRACTED;
sensorState previousState = NONE;

int moistureReading = 0;
bool deploymentFinish = false;

// To A4988 driver
// Remember to connect RST pin to 3.3v HIGH
const int DIR_PIN = 2;   // Motor CW or CCW
const int STEP_PIN = 3;  // Freq of pulses sent to `STEP_PIN` defines motor speed
const int SLEEP_PIN = 4;

// To other subsystems
const int READY_BUTTON_PIN = 5;       // When deployment starts
const int FIRST_TIME_PIN = 6;         // When the cart just leaves the track
const int DEPLOYMENT_FINISH_PIN = 7;  // One cycle of deployment

// Limit Switches
const int LIM_SWITCH_BOT = 10;
const int LIM_SWITCH_TOP = 12;

// Moisture sensor
const int MOISTURE = A0;  // This is an analog pin

// Other global variables
const int STEPS_PER_REVOLUTION = 200;  // Set to match NEMA 17 specs + config
const int MAX_MOISTURE_DATA_PTS = 10000;
// const bool ready_to_read = false;  // At right place and right time to deploy sensor


//COMMS FUNCTIONS=================================================================================================================================

//define struct with all values related to a reading
struct nutrients {
  int x;
  int y;
  int m;
  int n;
  int p;
  int k;
};

//store new input in struct
nutrients store() {
  //simulate being received from sensor team
  //will no longer have random parameters, will be values, skip this chunk

  x = random(3);  //this is distance in meters, i made it 2 because max is /0.5 = 4
  //x = x_received;
  y = random(2);  //this y is ALREADY the row number so no adjustments
  //y = y_received;
  m = random(1000);
  //m = m_received;
  n = random(1000);
  //n = n_received;
  p = random(1000);
  //p = p_received;
  k = random(1000);
  //k = k_received;

  //THESE WILL BE CHANGED TO THE ACTUAL GLOBAL VARIABLES I AM STORING THINGS IN

  //fixing values for proper use, divide by 100 bc x_spacing input in cm
  //int x_new = x / (x_spacing_value/100); //actual code
  int x_new = x;  //we dont want it to depend on user input for the prototype
  //adjusment formulas for all nutrient to be understandable
  m = adaptMoisture(m);

  nutrients input = { x_new, y, m, n, p, k };
  return input;
}

//calibrate moisture to percent moisture [NOT DONE]
int adaptMoisture(int moisture) {
  //some sort of function to adapt moisture to usable values
  int new_moisture = moisture + 1;
  return new_moisture;
}

//put new values in storage array
void place_coord(nutrients input) {
  int y = input.y;
  int x = input.x;
  storage[y][x][0] = input.m;
  storage[y][x][1] = input.n;
  storage[y][x][2] = input.p;
  storage[y][x][3] = input.k;
  return;
}

//extract values from query string
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
    if (endIndex == -1) {  //-1 means the end
      endIndex = queryString.length();
    }
    String valueString = queryString.substring(startIndex, endIndex);  //take out value based on index
    return valueString.toInt();                                        //we know it's a value
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
  client.print(".container {margin: 0 auto; text-align: center;}");  //change margin-top  margin-top: 100px;
  client.print(".slideContainer {width: 100%;}");
  client.print("button {color: white; width: 100px; height: 100px;");
  client.print("border-radius: 50%; margin: 20px; border: none; font-size: 20px; outline: none; transition: all 0.2s;}");
  client.print(".buttonStyle{background-color: cadetblue;}");
  client.print("</style>");

  //client.print("<style>");
  //client.print(".container {margin: 0 auto; text-align: center; margin-top: 100px;}");
  //client.print("button {color: white; width: 100px; height: 100px;");

  //client.print("border-radius: 50%; margin: 20px; border: none; font-size: 20px; outline: none; transition: all 0.2s;}");

  //client.print(".off{background-color: grey;}");
  //client.print("button:hover{cursor: pointer; opacity: 0.7;}"); //dont have this
  //client.print("</style>");

  client.println("<h1 style='color: #5F9EA0'>Patchouli Farm Parameters</h1>");

  //assuming min 0.6 apart so max 84*0.6 = 140 rows
  client.print("<div class='slideContainer'>");
  client.println("<h2>Plot Width (X): <span id='x_width'>42</span></h2>");                                //THIS IS THE PART SEEN ON THE SCREEN
  client.println("0<input type='range' min='0' max='84' value='42' id='slider1' list='XYmarkers' />84");  // slider for x width

  client.println("<h2>Plot Length (Y): <span id='y_length'>42</span></h2>");
  client.println("0<input type='range' min='0' max='84' value='42' id='slider2' list='XYmarkers' />84");  // slider for y length

  client.println("<h2>Number of Rows: <span id='num_rows'>70</span></h2>");
  client.println("0<input type='range' min='0' max='140' value='70' id='slider3' list='NUMmarkers' />140");  // slider for number of rows

  client.println("<h2>X Spacing [cm]: <span id='x_spacing'>100</span></h2>");
  client.println("60<input type='range' min='60' max='120' value='100' id='slider4' list='NUMmarkers' />120");  // slider for x distance

  client.println("<h2>Mode (Automatic or Manual): <span id='modeAM'>N</span></h2>");
  client.println("A<input type='range' min='0' max='1' value='0' id='slider5'>M");  // slider for mode

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
  client.println("document.getElementById('x_width').innerText = x_width_value;");      // update slider 1 value
  client.println("document.getElementById('y_length').innerText = y_length_value;");    // update slider 2 value
  client.println("document.getElementById('num_rows').innerText = num_rows_value;");    // update slider 3 value
  client.println("document.getElementById('x_spacing').innerText = x_spacing_value;");  // update slider 4 value

  client.println("if (mode_value == 0) {");
  client.println("document.getElementById('modeAM').innerText = 'A';");  // update slider 5 value
  client.println("} else {");                                            //else if (mode_value == 1)
  client.println("document.getElementById('modeAM').innerText = 'M';");  // update slider 5 value
  client.println("}");
  client.println("document.getElementById('refreshMessage').innerHTML = 'Please refresh the screen!';");
  client.println("}");
  client.println("};");
  // Send all five slider values to the server, this is the "url" separating values with "="
  client.println("xhttp.open('GET', '/inputSliders?slider1=' + x_width_value + '&slider2=' + y_length_value + '&slider3=' + num_rows_value + '&slider4=' + x_spacing_value + '&slider5=' + mode_value, true);");
  client.println("xhttp.send();");
  client.println("}");
  client.println("</script>");
  client.println("<h1 id='refreshMessage'></h1>");
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
  client.print(".container {margin: 0 auto; text-align: center;}");  //change margin-top  margin-top: 100px;
  client.print(".slideContainer {width: 100%;}");
  client.print("button {color: white; width: 100px; height: 100px;");
  client.print("border-radius: 50%; margin: 20px; border: none; font-size: 20px; outline: none; transition: all 0.2s;}");
  client.print(".buttonStyle{background-color: cadetblue;}");
  client.print(".sliderStyle{background-color: cadetblue;}");
  client.print("</style>");

  client.println("<h1 style='font-family: Tahoma'>Automatic Mode Parameters</h1>");

  //assuming min 0.6 apart so max 84*0.6 = 140 rows
  client.print("<div class='slideContainer'>");
  client.println("<h2>X Distance Between Plants [m]: <span id='x_dist'>42</span></h2>");                                       //THIS IS THE PART SEEN ON THE SCREEN
  client.println("0<input type='range' class='.sliderStyle' min='0' max='84' value='42' id='slider6' list='XYmarkers' />84");  // slider for x width

  client.println("<h2>Row Frequency: <span style='font-family: Tahoma' id='row_dist'>70</span></h2>");
  client.println("0<input type='range' min='0' max='140' value='1' id='slider7' list='NUMmarkers' />140");  // slider for y length

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
  client.println("document.getElementById('x_dist').innerText = x_dist_value;");      // update slider 6 value
  client.println("document.getElementById('row_dist').innerText = row_dist_value;");  // update slider 7 value
  client.println("document.getElementById('refreshMessage').innerHTML = 'Please refresh the screen!';");
  client.println("}");
  client.println("};");
  // Send all five slider values to the server, this is the "url" separating values with "="
  client.println("xhttp.open('GET', '/autoSliders?slider6=' + x_dist_value + '&slider7=' + row_dist_value, true);");
  client.println("xhttp.send();");
  client.println("}");
  client.println("</script>");
  client.println("<h1 id='refreshMessage'></h1>");
  client.println("</body>");
  client.println("</html>");
}



void printGUI(WiFiClient client) {
  Serial.println("enter printing gui");

  if (client) {                    // if you get a client,
    Serial.println("new client");  // print a message out the serial port
    String currentLine = "";       // make a String to hold incoming data from the client
    while (client.connected()) {   // loop while the client's connected
      if (client.available()) {    // if there's bytes to read from the client,
        char c = client.read();    // read a byte, then
        Serial.write(c);           // print it out the serial monitor
        if (c == '\n') {           // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // html
            client.println("<!DOCTYPE html>");
            client.println("<html lang='en'>");
            client.println("");
            client.println("<head>");
            client.println("    <meta charset='UTF-8'>");
            client.println("    <meta name='viewport' content='width=device-width, initial-scale=1.0'>");
            client.println("    <title>Heatmap with Gradient Legend</title>");
            client.println("    <style>");
            client.println("        html,");
            client.println("        body,");
            client.println("        #container {");
            client.println("            width: 100%;");
            client.println("            height: 100%;");
            client.println("            margin: 0;");
            client.println("            padding: 0;");
            client.println("            align: 'center';");
            client.println("            position: relative;");
            client.println("            border: 7px solid #FFFFFF;");
            client.println("            margin-top: 10px;");
            client.println("            margin-bottom: 10px;");
            client.println("");
            client.println("        }");
            client.println("");
            client.println("        .heatmap {");
            client.println("            display: grid;");
            client.println("            grid-template-columns: auto repeat(2, 1fr);");
            client.println("            grid-template-rows: auto repeat(2, 1fr);");
            client.println("            gap: 2px;");
            client.println("            width: 400px;");
            client.println("            height: 400px;");
            client.println("            position: relative;");
            client.println("            text-align: center;");
            client.println("        }");
            client.println("");
            client.println("        .plot {");
            client.println("            border: 1px solid #fff;");
            client.println("            position: relative;");
            client.println("        }");
            client.println("");
            client.println("        .plot:hover::before {");
            client.println("            content: attr(data-heat);");
            client.println("            position: absolute;");
            client.println("            top: 50px;");
            client.println("            left: 50%;");
            client.println("            transform: translateX(-50%);");
            client.println("            background-color: rgba(255, 255, 255, 0.8);");
            client.println("            padding: 5px;");
            client.println("            border-radius: 5px;");
            client.println("            font-size: 11px;");
            client.println("        }");
            client.println("");
            client.println("        .row-label,");
            client.println("        .column-label {");
            client.println("            text-align: center;");
            client.println("            font-size: 14px;");
            client.println("            line-height: 14px;");
            client.println("");
            client.println("        }");
            client.println("");
            client.println("        .row-label {");
            client.println("            grid-column: 1;");
            client.println("            grid-row: span 1;");
            client.println("            padding: 150px 0;");
            client.println("            text-align: center;");
            client.println("        }");
            client.println("");
            client.println("        .column-label {");
            client.println("            grid-row: 1;");
            client.println("            grid-column: span 2;");
            client.println("            vertical-align: middle;");
            client.println("            justify-self: center;");
            client.println("            /* Center the row labels */");
            client.println("        }");
            client.println("");
            client.println("");
            client.println("        #legend {");
            client.println("            margin-top: 20px;");
            client.println("            text-align: center;");
            client.println("            position: relative;");
            client.println("            width: 400px;");
            client.println("            height: 30px;");
            client.println("        }");
            client.println("");
            client.println("        .gradient {");
            client.println("            position: absolute;");
            client.println("            width: 100%;");
            client.println("            height: 100%;");
            client.println("            background-image: linear-gradient(to right, #FFA500, #B0D8A4, #6495ED);");
            client.println("        }");
            client.println("");
            client.println("        .legend-labels {");
            client.println("            position: absolute;");
            client.println("            top: 35px;");
            client.println("            display: flex;");
            client.println("            justify-content: space-between;");
            client.println("            width: 100%;");
            client.println("        }");
            client.println("");
            client.println("        .legend-label {");
            client.println("            flex: 1;");
            client.println("            text-align: center;");
            client.println("            font-size: 12px;");
            client.println("        }");
            client.println("");
            client.println("        .legend-color {");
            client.println("            width: 20px;");
            client.println("            height: 20px;");
            client.println("            display: inline-block;");
            client.println("            margin-right: 5px;");
            client.println("            border: 1px solid #ccc;");
            client.println("            vertical-align: middle;");
            client.println("        }");
            client.println("    </style>");
            client.println("</head>");
            client.println("");
            client.println("<body>");
            //added this to describe the plot location after refresh so farmers know what has changed
            if (newData) {
              client.println("<h1><center>Patchouli Soil Moisture Levels -- New data at plot location (" + String(plot_index/2 +1) + "," + String(plot_index%2 +1) + ")</center></h1>");
              newData = false;
            } else client.println("<h1><center>Patchouli Soil Moisture Levels</center></h1>");

            client.println("    <!--Title-->");
            client.println("    <center>");
            //client.println("        <h1> Patchouli Soil Moisture Levels</h1>");
            client.println("        <h2><i>Tingkat Kelembaban Tanah Nilam</i></h2>");
            client.println("    </center>");
            client.println("");
            client.println("");
            client.println("    <body> The following map depicts the soil moisture levels of all patchouli plots. Each plot is provided with an associated colour which presents the moisture level of the designated plot. The numbers along the rows and columns distinguish each plot. Please see the legend below to determine the soil moisture level of the plot and the associated recommendation.</body>");
            client.println("    <br>");
            client.println("    <br>");
            client.println("    <body><i>Berikut ini adalah peta yang menggambarkan tingkat kelembaban tanah dari semua petak tanaman nilam. Setiap petak diberikan warna terkait yang menunjukkan tingkat kelembaban tanah dari petak yang ditentukan. Angka di sepanjang baris dan kolom membedakan setiap petak. Silakan lihat legenda di bawah ini untuk menentukan tingkat kelembaban tanah dari petak dan rekomendasi yang terkait.</i></body>");
            client.println("    <br>");
            client.println("    <br>");
            client.println("    <body> Click on a coloured square to receive an update of the patchouli plot's status!</body>");
            client.println("    <br>");
            client.println("    <br>");
            client.println("    <body><i>Klik pada kotak berwarna untuk mendapatkan pembaruan status petak tanaman nilam Anda</i></body>");
            client.println("    <!-- Heatmap -->");
            client.println("    <div id='container' class='heatmap'></div>");
            client.println("    <!-- Legend -->");
            client.println("    <br>");
            client.println("    <center>");
            client.println("        <h2>Legend</h2>");
            client.println("        <h3><i>Legenda</i></h3>");
            client.println("        <div id='legend'>");
            client.println("            <div class='gradient'></div>");
            client.println("            <div class='legend-labels'>");
            client.println("                <div class='legend-label'>Too Little Moisture <br> [Less than 76%] <br><br> <i> Terlalu Sedikit Kelembapan <br> [Kurang dari 76%] </i></div>");
            client.println("                <div class='legend-label'>Proper Moisture <br>[Between 76% and 80%] <br><br> <i>Kelembapan yang Tepat <br>[Antara 76% dan 80%]</i> </div>");
            client.println("                <div class='legend-label'>Too Much Moisture <br>[Greater than 80%] <br><br> <i>Terlalu Banyak Kelembapan <br>[Lebih dari 80%]</i> </div>");
            client.println("            </div>");
            client.println("            <br>");
            client.println("            <br>");
            client.println("            </div>");
            client.println("        </div>");
            client.println("        ");
            client.println("    </center>");
            client.println("    <br>");
            client.println("    <br>");
            client.println("    <br>");
            client.println("    <br>");
            client.println("     <br>");
            client.println("");
            client.println("    <h3>Recommendation</h3>");
            client.println("    <h4><i>Rekomendasi</i></h4>");
            client.println("");
            client.println("    <body>");
            client.println("        Dry soil regions with a moisture level below 76% (orange) are in need of additional watering for optimal patchouli survival. <strong>Please water this patchouli plot!</strong>");
            client.println("        <br>");
            client.println("        <br>");
            client.println("        <i>Daerah tanah kering dengan tingkat kelembaban di bawah 76% (oranye) memerlukan penyiraman tambahan untuk kelangsungan hidup nilam yang optimal.");
            client.println("        <strong>Silakan siram petak nilam ini!</strong></i>");
            client.println("");
            client.println("        <br>");
            client.println("        <br> Overwatered soil regions with a moisture level above 80% (blue) are at risk of overwatering the patchouli plant. <strong>Don't water this patchouli plot and limit any moisture added!</strong>");
            client.println("         <br>");
            client.println("        <br>");
            client.println("       <i>Daerah tanah yang terlalu disiram dengan tingkat kelembaban di atas 80% (biru) berisiko menyiram tanaman nilam berlebihan. <strong>Jangan menyiram petak nilam ini dan batasi penambahan kelembaban apa pun!</strong></i>");
            client.println("");
            client.println("");
            client.println("        <br>");
            client.println("        <br> Well-maintained soil regions with a moisture level between 76% and 80% have sufficient moisture levels for optimal patchouli growth. <strong>This plot doesn't need any more water!</strong>");
            client.println("        ");
            client.println("         <br>");
            client.println("        <br><i>");
            client.println("       Daerah tanah yang terawat dengan baik dengan tingkat kelembaban antara 76% dan 80% memiliki tingkat kelembaban yang cukup untuk pertumbuhan nilam yang optimal. <strong>Petak ini tidak memerlukan air lagi!</strong></i>");
            client.println("");
            client.println("");
            client.println("    </body>");
            client.println("");
            client.println("");
            client.println("    <br>");
            client.println("    <br>");
            client.println("    </center>");
            client.println("");
            client.println("    <script>");
            client.println("        document.addEventListener('DOMContentLoaded', function() {");
            client.println("            // Plot dimensions");
            client.println("            const xCoord = 2;");
            client.println("            const yCoord = 2;");
            client.println("");
            client.println("            // Container for heatmap plots");
            client.println("            const container = document.getElementById('container');");
            client.println("");
            client.println("            // Sensor readings");
            client.println("");
            client.println("            // Define color stops and corresponding moisture levels");
            client.println("            const colorStops = [{");
            client.println("                    color: '#FFA500',");
            client.println("                    moistureLevel: 0");
            client.println("                }, // Dry");
            client.println("                {");
            client.println("                    color: '#B0D8A4',");
            client.println("                    moistureLevel: 650");
            client.println("                }, // Nice");
            client.println("                {");
            client.println("                    color: '#6495ED',");
            client.println("                    moistureLevel: 1000");
            client.println("                } // Wet");
            client.println("            ];");

            // Generate heatmap
            String moistureLevelsToString = "[";
            for (int j = 0; j < 3; j++) {
              moistureLevelsToString = moistureLevelsToString + moistureLevels[j] + ", ";
            }
            moistureLevelsToString = moistureLevelsToString + moistureLevels[3] + "]";

            client.println("const moistureLevels = " + moistureLevelsToString + ";");
            client.println("            // Generate heatmap");
            client.println("            for (let i = 0; i < xCoord * yCoord; i++) {");
            client.println("                if (i % xCoord == 0) {");
            client.println("                    const rowLabel = document.createElement('div');");
            client.println("                    rowLabel.classList.add('row-label');");
            client.println("                    rowLabel.textContent = i / xCoord + 1;");
            client.println("                    container.appendChild(rowLabel);");
            client.println("                    // container.style.textAlign = 'center';");
            client.println("                    //container.style.verticalAlign = middle;");
            client.println("                }");
            client.println("                const plot = document.createElement('div');");
            client.println("                plot.classList.add('plot');");
            client.println("                plot.dataset.x = (i % xCoord) + 1;");
            client.println("                plot.dataset.y = Math.floor(i / yCoord) + 1;");
            client.println("");
            client.println("                // Determine color based on moisture level");
            client.println("                const moistureLevel = moistureLevels[i];");
            client.println("              ");
            client.println("                let color;");
            client.println("                if (moistureLevel == 0) {");
            client.println("                    color = '#FFFFFF'; // Set white color for no reading");
            client.println("                } else {");
            client.println("                    color = interpolateColor(moistureLevel, colorStops);");
            client.println("                }");
            client.println("");
            client.println("                plot.style.backgroundColor = color;");
            client.println("                plot.dataset.heat = 'Soil Moisture Level: ' +  Math.round(moistureLevel/850*100) + '% Tingkat Kelembaban Tanah: ' +  Math.round(moistureLevel/850*100) + '%' ;");
            client.println("");
// Add event listener for click"
            client.println("                plot.addEventListener('click', function() {");
            client.println("                    alert('Soil Moisture Level / Tingkat Kelembaban Tanah: ' + Math.round(moistureLevel/850*100) + '%' + getMessage(moistureLevel));");
            client.println("                });");
            client.println("");
            client.println("                container.appendChild(plot);");
            client.println("            }");
            client.println("            for (let i = 1; i <= 2; i++) {");
            client.println("                const columnLabel = document.createElement('div');");
            client.println("                columnLabel.classList.add('column-label');");
            client.println("                columnLabel.textContent = i;");
            client.println("                container.appendChild(columnLabel);");
            client.println("                //container.style.verticalAlign = middle;");
            client.println("            }");
            client.println("");
            client.println("        });");
            client.println("");
            client.println("        // Function to interpolate color based on moisture level");
            client.println("        function interpolateColor(moistureLevel, colorStops) {");
            client.println("            for (let i = 1; i < colorStops.length; i++) {");
            client.println("                const prevStop = colorStops[i - 1];");
            client.println("                const currentStop = colorStops[i];");
            client.println("");
            client.println("                if (moistureLevel == null) {");
            client.println("                    interpolatedColor = '#FFFFFF';");
            client.println("                    return interpolatedColor;");
            client.println("");
            client.println("                } else if (moistureLevel == 0) {");
            client.println("                    interpolatedColor = '#FFFFFF';");
            client.println("                    return interpolatedColor;");
            client.println("");
            client.println("                } else if (moistureLevel <= currentStop.moistureLevel) {");
            client.println("                    const prevColor = prevStop.color;");
            client.println("                    const currentColor = currentStop.color;");
            client.println("                    const prevMoisture = prevStop.moistureLevel;");
            client.println("                    const currentMoisture = currentStop.moistureLevel;");
            client.println("");
            client.println("                    // Interpolate between previous and current colors");
            client.println("                    const t = (moistureLevel - prevMoisture) / (currentMoisture - prevMoisture);");
            client.println("                    const interpolatedColor = interpolate(prevColor, currentColor, t);");
            client.println("                    return interpolatedColor;");
            client.println("                }");
            client.println("            }");
            client.println("            // Default color (in case moisture level exceeds all stops) -> blue");
            client.println("            return colorStops[colorStops.length - 1].color;");
            client.println("        }");
            client.println("");
            client.println("        // Function to interpolate between two colors for gradient");
            client.println("        function interpolate(color1, color2, t) {");
            client.println("            const r1 = parseInt(color1.substr(1, 2), 16);");
            client.println("            const g1 = parseInt(color1.substr(3, 2), 16);");
            client.println("            const b1 = parseInt(color1.substr(5, 2), 16);");
            client.println("            const r2 = parseInt(color2.substr(1, 2), 16);");
            client.println("            const g2 = parseInt(color2.substr(3, 2), 16);");
            client.println("            const b2 = parseInt(color2.substr(5, 2), 16);");
            client.println("            const r = Math.round(r1 + (r2 - r1) * t);");
            client.println("            const g = Math.round(g1 + (g2 - g1) * t);");
            client.println("            const b = Math.round(b1 + (b2 - b1) * t);");
            client.println("            return '#' + componentToHex(r) + componentToHex(g) + componentToHex(b);");
            client.println("        }");
            client.println("");
            client.println("        function componentToHex(c) {");
            client.println("            const hex = c.toString(16);");
            client.println("            return hex.length == 1 ? '0' + hex : hex;");
            client.println("        }");
            client.println("");
            client.println("        function getMessage(heat) {");
            client.println("            if (heat < 650) {");
            //client.println()
            client.println("                return 'This plot is dry. The patchouli plants are in need of water!' +' '  + 'Plot ini kering. Tanaman nilam membutuhkan air!' ;");
            client.println("            } else if (heat <= 680) {");
            client.println("                return 'This plot has sufficient moisture. It does not need any more water for now!' + 'Plot ini memiliki kelembapan yang cukup. Ia tidak membutuhkan air lagi untuk saat ini!';");
            client.println("            } else {");
            client.println("                return 'This plot is overwatered.' + 'Plot ini tergenang air.';");
            client.println("            }");
            client.println("        }");
            client.println("    </script>");
            client.println("</body>");
            client.println("");
            client.println("</html>");
            client.println();
            

            // break out of the while loop:
            break;
          } else {  // if you got a newline, then clear currentLine:
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
  Serial.println("exit printing GUI");
}

// only for displaying the storage array
void testprint() {
  for (int i = 0; i < 4; i++) {
    Serial.print(moistureLevels[i]);
    Serial.print(" ");
  }
  Serial.println("--------------------------------------------------------------------------------------------");
}


//SENSOR FUNCTIONS================================================================================================================================

void sleepStepper() {
  // Logic low puts device into sleep mode
  digitalWrite(SLEEP_PIN, LOW);
}

void awakenStepper() {
  // Logic high enables normal operation
  digitalWrite(SLEEP_PIN, HIGH);
  // In order to allow the charge pump to stabilize, provide a delay of 1 ms before issuing a Step comman
  // SLP pin should be high by default
  delay(1);
}

// Send one pulse to the stepper motor which moves the motor by a tiny bit
void stepperStep(int msDelay) {
  digitalWrite(STEP_PIN, HIGH);
  delay(msDelay);
  digitalWrite(STEP_PIN, LOW);
  delay(msDelay);
}

// Move piston down
void setMotorSpinCW() {
  // HIGH is 3.3 volts (confirmed with DMM)
  digitalWrite(DIR_PIN, HIGH);
}

// Move piston up
void setMotorSpinCCW() {
  digitalWrite(DIR_PIN, LOW);
}

void testLimitSwith() {
  Serial.print(digitalRead(LIM_SWITCH_TOP));
  Serial.print(" ");
  Serial.println(digitalRead(LIM_SWITCH_BOT));
  delay(25);
}

// Deployment
void deploySensorProcess() {
  // Re-init the global variables
  moistureReading = 0;

  // Put stepper in sleep mode by default
  sleepStepper();
  // RETRACTED    0,
  // EXTENDING    1,
  // EXTENDED     2,
  // MEASURING    3,
  // RETRACTING   4,
  // NONE         5,
  // NUM_STATES   6
  Serial.print("State: ");
  Serial.println(currentState);

  // State table
  switch (currentState) {
    case RETRACTED:
      {
        // Movement subsystem polling
        // TODO ready_to_read = digitalRead(...);
        // Temp pin for button
        int ready_to_read = digitalRead(READY_BUTTON_PIN);
        int first_time = digitalRead(FIRST_TIME_PIN);
        digitalWrite(DEPLOYMENT_FINISH_PIN, HIGH);
        if (ready_to_read) {
          digitalWrite(DEPLOYMENT_FINISH_PIN, LOW);
          Serial.print("ready_to_read: ");
          Serial.println(ready_to_read);
          Serial.print("first_time: ");
          Serial.println(first_time);
          if (first_time)
            plot_index = 0;
          else
            plot_index = (plot_index + 1) % 4;
          // TODO Send a signal to the comms team that the deployment system is ready
          currentState = EXTENDING;
        }
        Serial.print("Plot_index: ");
        Serial.println(plot_index);
        break;
      }

    // 1
    case EXTENDING:
      {
        awakenStepper();  // Activate stepper
        setMotorSpinCW();
        // Should be fine because independent of other subsystem
        // After hitting the bottom
        if (digitalRead(LIM_SWITCH_BOT)) {
          currentState = EXTENDED;
          break;
        }
        // While the bottom is not hit
        for (int ii = 0; ii < 500; ii++) {
          stepperStep(1);
        }
        break;
      }

    // 2
    case EXTENDED:
      {
        currentState = MEASURING;
        break;
      }

    // 3
    case MEASURING:
      {
        // Turns the "listener" on
        Serial.print("Reading... ");
        // The time it takes to read 10000 data points is not actually that long
        // 100000 points adds a slight delay to give illusion of it "processing"
        for (int i = 0; i < MAX_MOISTURE_DATA_PTS; i++) {
          moistureReading += analogRead(MOISTURE);
        }
        moistureReading /= MAX_MOISTURE_DATA_PTS;
        Serial.println(moistureReading);
        moistureLevels[plot_index] = moistureReading;
        newData = true;
        // TODO Send data to the comms team
        currentState = RETRACTING;
        break;
      }

    // 4
    case RETRACTING:
      {
        awakenStepper();  // Activate stepper
        setMotorSpinCCW();
        // After hitting the top
        if (digitalRead(LIM_SWITCH_TOP)) {
          currentState = RETRACTED;
          break;
        }
        // While the top is not hit
        for (int ii = 0; ii < 500; ii++) {
          stepperStep(1);
        }
        break;
      }

    default:
      {
        currentState = RETRACTED;
        break;
      }
  }

  // Put stepper in sleep mode after performing necessary movements
  sleepStepper();
}


//SETUP LOOP =====================================================================================================================================
void setup() {
  //COMMS SETUP LOOP ------------------------------------------------------------------------------------------------------
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  // while (!Serial) {
  //   ; // wait for serial port to connect. Needed for native USB port only
  // }


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

  // // wait 10 seconds for connection:
  // delay(10000);

  // start the web server on port 80
  server.begin();

  // you're connected now, so print out the status
  printWiFiStatus();

  //SENSOR SETUP LOOP -----------------------------------------------------------------------------------------------------
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(SLEEP_PIN, OUTPUT);

  pinMode(READY_BUTTON_PIN, INPUT);
  pinMode(FIRST_TIME_PIN, INPUT);
  pinMode(DEPLOYMENT_FINISH_PIN, OUTPUT);

  pinMode(LIM_SWITCH_TOP, INPUT);
  pinMode(LIM_SWITCH_BOT, INPUT);

  pinMode(MOISTURE, OUTPUT);

  // Read top dep switch
  // Is it pushed in?
  // If not
  // Set current state = retracting
  // Set digitalwrite(READY, LOW)
  // Always do this, works out
  currentState = RETRACTING;
  digitalWrite(DEPLOYMENT_FINISH_PIN, LOW);
}


//EXECUTED MAIN LOOP =============================================================================================================================
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

  switch (big_state) {
    case 0:

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

                client.println("HTTP/1.1 200 OK");
                client.println("Content-type:text/html");
                client.println();

                Serial.println("New client connected");


                switch (state) {
                  case 0:  // initial page, farmer plot parameters
                    Serial.println("enter case 0");
                    processInitInput(client);
                    Serial.println(state);
                    break;
                  case 1:  // automatic case page
                    Serial.println("enter case 1");
                    processAutoInput(client);
                    Serial.println(state);
                    break;
                  case 2:                                        // sending data to the movement team through pins
                    Serial.println("cases done, enter case 2");  //it basically stops here
                    client.println("<!DOCTYPE html>");
                    client.println("<html>");
                    client.println("<head>");
                    client.println("<meta charset='UTF-8'>");
                    client.println("<meta name='viewport' content='width=device-width, initial-scale=1.0'>");
                    client.println("</head>");
                    client.println("<body>");
                    client.println("<h1 style='color: #5F9EA0'>Please wait for data to be sent to the movement team! :)</h1>");

                    //THIS IS SO FAST IT DOESN'T EVEN GET SHOWN

                    //send through pins to the raspberry pi?
                    //serial communication?
                    //hardcoding just for the sake of the prototype?

                    big_state = 1;
                }
                // end response
                client.println();
                // break out of the while loop:
                break;
              } else {  // if you got a newline, then clear currentLine:
                currentLine = "";
              }
            } else if (c != '\r') {  // if you got anything else but a carriage return character,
              currentLine += c;      // add it to the end of the currentLine
            }
            // CASE 0 RETRIEVING DATA FROM 5 SLIDERS (INPUT CASE)
            if ((currentLine.startsWith("GET /inputSliders")) && (currentLine.endsWith("HTTP/1.1"))) {
              Serial.println("input sliders received-----------------------------------------------");
              state = 1;  //we don't have manual mode right now so we just send everything to the state 1 which is automatic
              Serial.println(currentLine);
              if (currentLine.indexOf("GET /inputSliders?") != -1) {
                int queryStart = currentLine.indexOf('?');
                String queryString = currentLine.substring(queryStart + 1);
                // call the function that extracts all of the specific values in order from the url
                // these will be stored and translated to something else
                x_width_value = getValueFromQueryString(queryString, "slider1");
                y_length_value = getValueFromQueryString(queryString, "slider2");
                num_rows_value = getValueFromQueryString(queryString, "slider3");
                x_spacing_value = getValueFromQueryString(queryString, "slider4");
                mode_value = getValueFromQueryString(queryString, "slider5");
                Serial.print("exit retrieving input values");
              }
            }
            // CASE 1 RETRIEVING DATA FROM 2 SLIDERS (AUTOMATIC CASE)
            if ((currentLine.startsWith("GET /autoSliders")) && (currentLine.endsWith("HTTP/1.1"))) {
              Serial.println("auto sliders received-----------------------------------------------");
              state = 2;
              Serial.println(currentLine);
              if (currentLine.indexOf("GET /autoSliders?") != -1) {
                // separated by ?
                int queryStart = currentLine.indexOf('?');
                String queryString = currentLine.substring(queryStart + 1);

                x_dist_value = getValueFromQueryString(queryString, "slider6");
                row_dist_value = getValueFromQueryString(queryString, "slider7");
                Serial.print("exit retrieving auto values");
              }
            }
          }
        }
        // close the connection:
        client.stop();
        Serial.println("client disconnected");
      }
      break;


    case 1:

      Serial.println("enter sensor retrieval and display code");

      deploySensorProcess();

      //run the GUI
      printGUI(client);

      testprint();

      //no longer need to utilize storing.
      //nutrients new_input = store();
      //place_coord(new_input);

      break;
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