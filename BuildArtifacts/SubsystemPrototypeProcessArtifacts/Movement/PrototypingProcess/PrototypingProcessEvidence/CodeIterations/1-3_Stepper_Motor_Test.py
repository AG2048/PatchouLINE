# a4988.py
#
# Raspberry Pi Pico - stepper motor driver support
#
# This module provides a class for controlling an Allegro 4988 stepper motor
# driver.  This device can drive one bipolar stepper motor up to 2A per coil
# using microstepping current control.

# A typical usage requires two digital outputs.  The defaults assumes a Pololu
# A4988 stepper driver has been wired up to the Pico as follows:
#
#   Pico pin 21, GPIO16   -> DIR
#   Pico pin 22, GPIO17   -> STEP
#   any Pico GND          -> GND

# A4988 carrier board: https://www.pololu.com/product/1182

# This implementation bit-bangs the step line and so is limited to about 1000
# steps/sec as a result of CircuitPython execution speed.  This solution is is
# only suitable for low-speed stepper motion.

# Likely a better long-term solution will be to use the RP2040 programmable IO
# peripheral (PIO) to cycle the step output.

################################################################
# CircuitPython module documentation:
# time       https://circuitpython.readthedocs.io/en/latest/shared-bindings/time/index.html
# board      https://circuitpython.readthedocs.io/en/latest/shared-bindings/board/index.html
# digitalio  https://circuitpython.readthedocs.io/en/latest/shared-bindings/digitalio/index.html
#
# Driver lifecycle documentation:
# https://circuitpython.readthedocs.io/en/latest/docs/design_guide.html#lifetime-and-contextmanagers
#
################################################################################
# load standard Python modules
import time

# load the CircuitPython hardware definition module for pin definitions
import board

# load the CircuitPython GPIO support
import digitalio

#--------------------------------------------------------------------------------
class A4988:
    def __init__(self, DIR=board.GP16, STEP=board.GP17):
        """This class represents an A4988 stepper motor driver.  It uses two output pins
        for direction and step control signals."""

        self._dir  = digitalio.DigitalInOut(DIR)
        self._step = digitalio.DigitalInOut(STEP)

        self._dir.direction  = digitalio.Direction.OUTPUT
        self._step.direction = digitalio.Direction.OUTPUT

        self._dir.value = False
        self._step.value = False

    def step(self, forward=True):
        """Emit one step pulse, with an optional direction flag."""
        self._dir.value = forward

        # Create a short pulse on the step pin.  Note that CircuitPython is slow
        # enough that normal execution delay is sufficient without actually
        # sleeping.
        self._step.value = True
        # time.sleep(1e-6)
        self._step.value = False

    def move_sync(self, steps, speed=200.0):
        """Move the stepper motor the signed number of steps forward or backward at the
        speed specified in steps per second.  N.B. this function will not return
        until the move is done, so it is not compatible with asynchronous event
        loops.
        """

        self._dir.value = (steps >= 0)
        time_per_step = 1.0 / speed
        for count in range(abs(steps)):
            self._step.value = True
            # time.sleep(1e-6)
            self._step.value = False
            time.sleep(time_per_step)

    def deinit(self):
        """Manage resource release as part of object lifecycle."""
        self._dir.deinit()
        self._step.deinit()
        self._dir  = None
        self._step = None

    def __enter__(self):
        return self

    def __exit__(self):
        # Automatically deinitializes the hardware when exiting a context.
        self.deinit()

#--------------------------------------------------------------------------------
# Stepper motor demonstration.

stepper = A4988()
print("Starting stepper motor test.")

speed = 400

while True:
    print(f"Speed: {speed} steps/sec.")
    stepper.move_sync(800, speed)
    time.sleep(1.0)
    
