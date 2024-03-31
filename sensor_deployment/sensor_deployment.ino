/*
Code to control sensor deployment subsystem of the prototype.

Controls a NEMA 17 stepper motor with an A4988 driver module. Uses as little external libraries as possible.
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

void setup() {
}

void loop() {
}
