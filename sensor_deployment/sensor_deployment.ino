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
  DATA_SENDING,
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

// Limit Switches
const int LIM_SWITCH_BOT = 6;
const int LIM_SWITCH_TOP = 7;

// Moisture sensor
const int MOISTURE = 13;  // This is an analog pin

// Other global variables
const int STEPS_PER_REVOLUTION = 200;  // Set to match NEMA 17 specs + config
const int MAX_MOISTURE_DATA_PTS = 1000;
const bool READY_TO_READ = false;  // At right place and right time to deploy sensor

void setup() {
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(LIM_SWITCH_TOP, OUTPUT);
  pinMode(LIM_SWITCH_BOT, OUTPUT);
  pinMode(MOISTURE, OUTPUT);
  pinMode(SLEEP_PIN, OUTPUT);
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
void stepperStep(microsecondDelay) {
	digitalWrite(STEP_PIN, HIGH);
	delayMicroseconds(microsecondDelay);
	digitalWrite(STEP_PIN, LOW);
	delayMicroseconds(microsecondDelay);
}

// Move piston down
void spinMotorCW() {
	digitalWrite(DIR_PIN, HIGH);
}

// Move piston up
void spinMotorCCW() {
  digitalWrite(DIR_PIN, LOW);
}

void loop() {
  // Put stepper in sleep mode by default
  sleepStepper();

  // State table
  switch (currentState) {

    case RETRACTED:
      // Movement subsystem polling
      // TODO READY_TO_READ = digitalRead(...);
      if (READY_TO_READ) {
        // TODO Send a signal to the comms team that the deployment system is ready
        currentState = EXTENDING;
      }
      break;

    case EXTENDING:
      awakenStepper();  // Activate stepper
      spinMotorCW();
      // Should be fine because independent of other subsystem
      // While the bottom is not hit
      while (!digitalRead(LIM_SWITCH_BOT)) {
        stepperStep(1000);
      }
      // After hitting the bottom
      currentState = EXTENDED;
      break;

    case EXTENDED:
      currentState = MEASURING;
      break;

    case MEASURING:
      // Turns the "listener" on
      int sum = 0;
      for (int i = 0; i < MAX_MOISTURE_DATA_PTS; i++) {
        sum += analogRead(MOISTURE);
      }
      sum /= MAX_MOISTURE_DATA_PTS;
      // TODO Send data to the comms team
      currentState = RETRACTING;
      break;

    case RETRACTING:
      awakenStepper();  // Activate stepper
      spinMotorCCW();
      // While the top is not hit
      while (!digitalRead(LIM_SWITCH_TOP)) {
        stepperStep(1000);
      }
      // After hitting the top
      currentState = RETRACTED;
      break;

    default:
      currentState = RETRACTED;
      break;
  }

  // Put stepper in sleep mode after performing necessary movements
  sleepStepper();
}
