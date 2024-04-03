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

// To A4988 driver
// Remember to connect RST pin to 3.3v HIGH
const int DIR_PIN = 2;  // Motor CW or CCW
const int STEP_PIN = 3;  // Freq of pulses sent to `STEP_PIN` defines motor speed
const int SLEEP_PIN = 4;
const int TEST_READY_BUTTON_PIN = 5;

// Limit Switches
const int LIM_SWITCH_BOT = 11;
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
  pinMode(LIM_SWITCH_TOP, INPUT);
  pinMode(LIM_SWITCH_BOT, INPUT);
  pinMode(MOISTURE, OUTPUT);
  pinMode(SLEEP_PIN, OUTPUT);
  pinMode(TEST_READY_BUTTON_PIN, INPUT);
  Serial.begin(9600);
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
void spinMotorCW() {
  // HIGH is 3.3 volts (confirmed with DMM)
  digitalWrite(DIR_PIN, HIGH);
}

// Move piston up
void spinMotorCCW() {
  digitalWrite(DIR_PIN, LOW);
}

void test_limit_switch() {
  Serial.print(digitalRead(LIM_SWITCH_TOP));
  Serial.print(" ");
  Serial.println(digitalRead(LIM_SWITCH_BOT));
  delay(25);
}

void loop() {
  // Put stepper in sleep mode by default
  sleepStepper();
  // RETRACTED    0,
  // EXTENDING    1,
  // EXTENDED     2,
  // MEASURING    3,
  // RETRACTING   4,
  // NONE         5,
  // NUM_STATES   6
  Serial.println(currentState);

  // State table
  switch (currentState) {

    // 0
    case RETRACTED:
    {      
      // Movement subsystem polling
      // TODO ready_to_read = digitalRead(...);
      // Temp pin for button
      int ready_to_read = digitalRead(TEST_READY_BUTTON_PIN);
      if (ready_to_read) {
        // TODO Send a signal to the comms team that the deployment system is ready
        currentState = EXTENDING;
      }
      break;
    }

    // 1
    case EXTENDING:
    {
      awakenStepper();  // Activate stepper
      spinMotorCW();
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
      int tot = 0;
      Serial.print("Reading... ");
      // The time it takes to read 10000 data points is not actually that long
      // 100000 points adds a slight delay to give illusion of it "processing"
      for (int i = 0; i < MAX_MOISTURE_DATA_PTS; i++) {
        tot += analogRead(MOISTURE);
      }
      tot /= MAX_MOISTURE_DATA_PTS;
      Serial.println(tot);
      // TODO Send data to the comms team
      currentState = RETRACTING;
      break;
    }

    // 4
    case RETRACTING:
    { 
      awakenStepper();  // Activate stepper
      spinMotorCCW();
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
