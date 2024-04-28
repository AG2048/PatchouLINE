#include <SPI.h>
#include <WiFiNINA.h>

int state = 0;

int x_width;
int y_length;
int num_rows;
int x_spacing;
char mode;

int x_dist;
int row_dist;

//gonna need a set size, let's say 10
int x_coord_list; //will do [10] after
int y_coord_list; 

bool sensor_ready = true; //will be false
bool submit = true; //will be false

// need to display some sort of GUI html at EVERY STATE

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial) {
     ; // wait for serial port to connect. Needed for native USB port only
   }

}

void loop() {
  
  Serial.println("Execute Main Loop");
  delay(4000);

  switch (state) {
  
  // get user inputs, check if valid each time
  case 0:
    //MAYBE A BENEFIT OF A SLIDER IS THERE IS A SET INTERVAL THEY CAN SELECT FROM
    Serial.println("Enter Case 0");

    // display input GUI

    if (submit == true) {
      //STORE EVERYTHING
      Serial.println("Enter submit true");

      /* //this prints a slider to the page:
      client.print("<div class='slidecontainer'>");
      client.print("<input type='range' min='1' max='100' step = '10' value='50' class='slider' id='myRange'>");
      client.print("</div>");
      */

      // x width
      // y length
      // # rows
      // x spacing between plants
      // mode input

      while ((mode != 'M') && (mode != 'A')){
      //ask again and get input
        Serial.println("not M or A"); //these are just testing for now
        delay(5000);
        mode = Serial.read();
        Serial.print(mode);
      }

      if (mode == 'M') {
        state = 1;
      } if (mode == 'A') {
        state = 2;
      }
      submit = false; //set back just in case
      break;
    }
    break;
  
  // manual mode
  case 1:
    // x dist between samples desired (skipping)
    // row dist between samples desired (skipping)

    Serial.println("Enter Case 1");
    delay(4000);
    x_coord_list = 0;
    y_coord_list = 0;

    state = 3;
    break;

  // automatic mode
  case 2:
    //x coord list
    //y coord list
    Serial.println("Enter Case 2");
    delay(4000);
    x_dist = 0;
    row_dist = 0;
    state = 3;
    break;

  // send information to teams
  case 3:
    //functions out of GPIO ports
    // x width
    // y length
    // # rows
    // x spacing between plants
    // mode input
    // x dist
    // row dist
    // x coord list
    // y coord list
    Serial.println("Enter Case 3");
    state = 4;
    delay(4000);

    break;


  // WAIT state
  case 4:
    // get sensor ready from the sensor team
    // might not be bool, adjust later
    Serial.println("Enter Case 4");
    delay(4000);
    if (sensor_ready == true) {
      state = 5;
    }
    break; //should go out of loop and come back here

  // receive data
  case 5:
    // get data from the pins
    // specifically coordinate and NPKM
    // place in main storage (Function done)
    // individual storage (need to figure out...)
    Serial.println("Enter Case 5");
    delay(2000);
    state = 4;
    break;
  
  // WHAT IS DEFAULT?
  default:
    // statements
    break;
  }
  
}













