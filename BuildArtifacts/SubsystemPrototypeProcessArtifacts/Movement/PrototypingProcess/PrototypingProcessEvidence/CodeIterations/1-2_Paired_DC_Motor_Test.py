'''
ESC204 2024S Lab 4, Task D
Task: Use PWM to modulate the speed of a DC motor.
'''
import board
import time
import digitalio
import pwmio

# set up direction pins as digital outputs
in1 = digitalio.DigitalInOut(board.GP8)
in2 = digitalio.DigitalInOut(board.GP9)
in1.direction = digitalio.Direction.OUTPUT
in2.direction = digitalio.Direction.OUTPUT

in3 = digitalio.DigitalInOut(board.GP10)
in4 = digitalio.DigitalInOut(board.GP11)
in3.direction = digitalio.Direction.OUTPUT
in4.direction = digitalio.Direction.OUTPUT

# set up motor driving signal as PWM output
ena = pwmio.PWMOut(board.GP16, duty_cycle = 0)
ena2 = pwmio.PWMOut(board.GP17, duty_cycle = 0)

# set time limits
start_time = time.time()
time_limit = 20

# set starting (fastest) motor duty cycles
CW_duty = 50000
CCW_duty = 50000
duty_step = 5000
max_int = 65535

# rotate motor shaft in alternating directions with decreasing speed
while (time.time() - start_time) < time_limit:
    # rotate clockwise
    in1.value, in2.value = (False, True)
    in3.value, in4.value = (False, True)
    ena.duty_cycle = CW_duty
    ena2.duty_cycle = CW_duty
    print("Rotating CW at %f duty cycle"%(100*CW_duty/max_int))
    CW_duty = CW_duty - duty_step
    time.sleep(2)

    # rotate counterclockwise
    in1.value, in2.value = (True, False)
    in3.value, in4.value = (True, False)
    ena.duty_cycle = CCW_duty
    ena2.duty_cycle = CCW_duty
    print("Rotating CCW at %f duty cycle"%(100*CCW_duty/max_int))
    CCW_duty = CCW_duty - duty_step
    time.sleep(2)