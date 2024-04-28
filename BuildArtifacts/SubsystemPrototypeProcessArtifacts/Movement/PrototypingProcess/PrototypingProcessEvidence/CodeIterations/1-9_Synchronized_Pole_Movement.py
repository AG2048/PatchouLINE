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

def move(left_fwd, right_fwd, left_rev, right_rev, left_ena, right_ena, stop=False, forward=True, left_duty=65000, right_duty=46000):
    left_ena.duty_cycle = left_duty
    right_ena.duty_cycle = right_duty
    left_fwd.value, left_rev.value = (stop or forward, stop or not forward)
    right_fwd.value, right_rev.value = (stop or forward, stop or not forward)

#sonar_front = adafruit_hcsr04.HCSR04(trigger_pin=board.GP14, echo_pin=board.GP15)
#sonar_back = adafruit_hcsr04.HCSR04(trigger_pin=board.GP17, echo_pin=board.GP16)



# set up direction pins as digital outputs
right_fwd = digitalio.DigitalInOut(board.GP19)
right_rev = digitalio.DigitalInOut(board.GP18)
right_fwd.direction = digitalio.Direction.OUTPUT
right_rev.direction = digitalio.Direction.OUTPUT

left_fwd = digitalio.DigitalInOut(board.GP17)
left_rev = digitalio.DigitalInOut(board.GP16)
left_fwd.direction = digitalio.Direction.OUTPUT
left_rev.direction = digitalio.Direction.OUTPUT

# set up motor driving signal as PWM output
left_ena = pwmio.PWMOut(board.GP15, duty_cycle = 65000)
right_ena = pwmio.PWMOut(board.GP20, duty_cycle = 65000)

# set time limits
start_time = time.time()
time_limit = 4

# rotate motor shaft in alternating directions with decreasing speed
while (time.time() - start_time) < time_limit:
    # rotate clockwise
    move(left_fwd, right_fwd, left_rev, right_rev, left_ena, right_ena, stop=False, forward=False, left_duty=62500, right_duty=38000)
    time.sleep(2)
    move(left_fwd, right_fwd, left_rev, right_rev, left_ena, right_ena, stop=True, forward=True)
    time.sleep(1)
    # rotate counterclockwise
    move(left_fwd, right_fwd, left_rev, right_rev, left_ena, right_ena, forward=True, left_duty=65000,right_duty=35000)
    time.sleep(2)