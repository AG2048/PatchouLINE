/*
Code to control sensor deployment subsystem of the prototype.

Controls a NEMA 17 stepper motor with an A4988 driver module. Uses as little external libraries as possible.

Resources:
- https://lastminuteengineers.com/a4988-stepper-motor-driver-arduino-tutorial/
- https://www.youtube.com/watch?v=7spK_BkMJys
- https://www.pololu.com/file/0J450/a4988_DMOS_microstepping_driver_with_translator.pdf
- https://www.reddit.com/r/arduino/comments/124iace/why_nema17_stepper_motor_with_drv8225_vibrates/
*/

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

// Some global variables
int moistureReading = 0;
bool deploymentFinish = false;
bool newData = false;
int moistureLevels[4] = {0, 0, 0, 0};

// To A4988 driver
// Remember to connect RST pin to 3.3v HIGH
const int DIR_PIN = 2;  // Motor CW or CCW
const int STEP_PIN = 3;  // Freq of pulses sent to `STEP_PIN` defines motor speed
const int SLEEP_PIN = 4;

// To other subsystems
const int READY_BUTTON_PIN = 5;  // When deployment starts
const int FIRST_TIME_PIN = 6;  // When the cart just leaves the track
const int DEPLOYMENT_FINISH_PIN = 7;  // One cycle of deployment
int plot_index = 0;

// Limit Switches
const int LIM_SWITCH_BOT = 10;
const int LIM_SWITCH_TOP = 12;

// Moisture sensor
const int MOISTURE = A0;  // This is an analog pin

// Other global variables
const int STEPS_PER_REVOLUTION = 200;  // Set to match NEMA 17 specs + config
const int MAX_MOISTURE_DATA_PTS = 250000;
// const bool ready_to_read = false;  // At right place and right time to deploy sensor

void setup() {
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(SLEEP_PIN, OUTPUT);

  pinMode(READY_BUTTON_PIN, INPUT);
  pinMode(FIRST_TIME_PIN, INPUT);
  pinMode(DEPLOYMENT_FINISH_PIN, OUTPUT);

  pinMode(LIM_SWITCH_TOP, INPUT);
  pinMode(LIM_SWITCH_BOT, INPUT);

  pinMode(MOISTURE, OUTPUT);

  Serial.begin(9600);

  // Read top dep switch
  // Is it pushed in?
  // If not
  // Set current state = retracting
  // Set digitalwrite(READY, LOW)
  // Always do this, works out
  currentState = RETRACTING;
  digitalWrite(DEPLOYMENT_FINISH_PIN, LOW);
}

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

// void loop() {
//   testLimitSwith();
// }

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
      // While the bottom is not hit
      while (!digitalRead(LIM_SWITCH_BOT)) {
        stepperStep(1);
      }
      // After hitting the bottom
      currentState = EXTENDED;
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
      // While the top is not hit
      while (!digitalRead(LIM_SWITCH_TOP)) {
        stepperStep(1);
      }
      // After hitting the top
      currentState = RETRACTED;
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

void loop() {
  deploySensorProcess();
  //testLimitSwith();
}
