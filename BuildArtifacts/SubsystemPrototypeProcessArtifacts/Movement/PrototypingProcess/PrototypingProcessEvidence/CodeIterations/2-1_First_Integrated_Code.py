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

def move(left_fwd, right_fwd, left_rev, right_rev, left_ena, right_ena, stop=False, forward=True, left_duty=20000, right_duty=50000):
    left_ena.duty_cycle = left_duty
    right_ena.duty_cycle = right_duty
    left_fwd.value, left_rev.value = (stop or forward, stop or not forward)
    right_fwd.value, right_rev.value = (stop or forward, stop or not forward)

sonar_front = adafruit_hcsr04.HCSR04(trigger_pin=board.GP14, echo_pin=board.GP15)
sonar_back = adafruit_hcsr04.HCSR04(trigger_pin=board.GP17, echo_pin=board.GP16)


# Poles:
pole_right_fwd = digitalio.DigitalInOut(board.GP12)
pole_right_rev = digitalio.DigitalInOut(board.GP11)
pole_right_fwd.direction = digitalio.Direction.OUTPUT
pole_right_rev.direction = digitalio.Direction.OUTPUT

pole_left_fwd = digitalio.DigitalInOut(board.GP10)
pole_left_rev = digitalio.DigitalInOut(board.GP9)
pole_left_fwd.direction = digitalio.Direction.OUTPUT
pole_left_rev.direction = digitalio.Direction.OUTPUT

# set up motor driving signal as PWM output
pole_left_ena = pwmio.PWMOut(board.GP8, duty_cycle = 65000)
pole_right_ena = pwmio.PWMOut(board.GP13, duty_cycle = 65000)


# set up direction pins as digital outputs
right_fwd = digitalio.DigitalInOut(board.GP2)
right_rev = digitalio.DigitalInOut(board.GP3)
right_fwd.direction = digitalio.Direction.OUTPUT
right_rev.direction = digitalio.Direction.OUTPUT

left_fwd = digitalio.DigitalInOut(board.GP4)
left_rev = digitalio.DigitalInOut(board.GP5)
left_fwd.direction = digitalio.Direction.OUTPUT
left_rev.direction = digitalio.Direction.OUTPUT

# set up motor driving signal as PWM output
left_ena = pwmio.PWMOut(board.GP1, duty_cycle = 65000)
right_ena = pwmio.PWMOut(board.GP0, duty_cycle = 30000)

# set time limits
start_time = time.time()
time_limit = 40

# rotate motor shaft in alternating directions with decreasing speed
while (time.time() - start_time) < time_limit:
    # rotate clockwise
    move(left_fwd, right_fwd, left_rev, right_rev, left_ena, right_ena, stop=False, forward=True)
    while True:
        try:
            print((sonar_front.distance,))
            if sonar_front.distance <= 5:
                move(left_fwd, right_fwd, left_rev, right_rev, left_ena, right_ena, stop=True)
                time.sleep(2)
                break
        except RuntimeError:
            print("Retrying!")
        time.sleep(0.1)

    # rotate counterclockwise
    move(left_fwd, right_fwd, left_rev, right_rev, left_ena, right_ena, forward=False)
    while True:
        try:
            print((sonar_back.distance,))
            if sonar_back.distance <= 10:
                move(left_fwd, right_fwd, left_rev, right_rev, left_ena, right_ena, stop=True)
                time.sleep(2)
                break
        except RuntimeError:
            print("Retrying!")
        time.sleep(0.1)

    move(pole_left_fwd, pole_right_fwd, pole_left_rev, pole_right_rev, pole_left_ena, pole_right_ena, stop=False, forward=False, left_duty=62500, right_duty=35000)
    time.sleep(2)
    move(pole_left_fwd, pole_right_fwd, pole_left_rev, pole_right_rev, pole_left_ena, pole_right_ena, stop=True, forward=True)
    time.sleep(1)
    move(left_fwd, right_fwd, left_rev, right_rev, left_ena, right_ena, stop=False, forward=True)
    while True:
        try:
            print((sonar_front.distance,))
            if sonar_front.distance <= 5:
                move(left_fwd, right_fwd, left_rev, right_rev, left_ena, right_ena, stop=True)
                time.sleep(2)
                break
        except RuntimeError:
            print("Retrying!")
        time.sleep(0.1)

    # rotate counterclockwise
    move(left_fwd, right_fwd, left_rev, right_rev, left_ena, right_ena, forward=False)
    while True:
        try:
            print((sonar_back.distance,))
            if sonar_back.distance <= 10:
                move(left_fwd, right_fwd, left_rev, right_rev, left_ena, right_ena, stop=True)
                time.sleep(2)
                break
        except RuntimeError:
            print("Retrying!")
        time.sleep(0.1)
    # rotate counterclockwise
    move(pole_left_fwd, pole_right_fwd, pole_left_rev, pole_right_rev, pole_left_ena, pole_right_ena, forward=True, left_duty=62500,right_duty=33000)
    time.sleep(2)
    move(pole_left_fwd, pole_right_fwd, pole_left_rev, pole_right_rev, pole_left_ena, pole_right_ena, stop=True, forward=True)
    time.sleep(1)
