'''
ESC204 2024S Lab 4, Task D
Task: Use PWM to modulate the speed of a DC motor.
'''
import board
import time
import digitalio
import pwmio
import time
import board
import adafruit_hcsr04

def move(left_fwd, right_fwd, left_rev, right_rev, forward=True, left_duty=50000, right_duty=25000):
    left_fwd.value, left_rev.value = (forward, not forward)
    right_fwd.value, right_rev.value = (forward, not forward)

sonar_front = adafruit_hcsr04.HCSR04(trigger_pin=board.GP14, echo_pin=board.GP15)
sonar_back = adafruit_hcsr04.HCSR04(trigger_pin=board.GP17, echo_pin=board.GP16)



# set up direction pins as digital outputs
left_fwd = digitalio.DigitalInOut(board.GP2)
left_rev = digitalio.DigitalInOut(board.GP3)
left_fwd.direction = digitalio.Direction.OUTPUT
left_rev.direction = digitalio.Direction.OUTPUT

right_fwd = digitalio.DigitalInOut(board.GP4)
right_rev = digitalio.DigitalInOut(board.GP5)
right_fwd.direction = digitalio.Direction.OUTPUT
right_rev.direction = digitalio.Direction.OUTPUT

# set up motor driving signal as PWM output
left_ena = pwmio.PWMOut(board.GP0, duty_cycle = 50000)
right_ena = pwmio.PWMOut(board.GP1, duty_cycle = 25000)

# set time limits
start_time = time.time()
time_limit = 40

# rotate motor shaft in alternating directions with decreasing speed
while (time.time() - start_time) < time_limit:
    # rotate clockwise
    move(left_fwd, right_fwd, left_rev, right_rev, forward=True, left_duty=50000, right_duty=25000)
    while True:
        try:
            print((sonar_front.distance,))
            if sonar_front.distance <= 20:
                time.sleep(2)
                break
        except RuntimeError:
            print("Retrying!")
        time.sleep(0.1)

    # rotate counterclockwise
    move(left_fwd, right_fwd, left_rev, right_rev, forward=False, left_duty=50000, right_duty=25000)
    while True:
        try:
            print((sonar_back.distance,))
            if sonar_back.distance <= 20:
                time.sleep(2)
                break
        except RuntimeError:
            print("Retrying!")
        time.sleep(0.1)