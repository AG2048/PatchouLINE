'''
0105b movement code
controls cart and pole, and interfaces with the sensor deployment / communication arduino nano board
'''

import board
import time
import digitalio
import pwmio
import time
import board
import adafruit_hcsr04

# A function to facilitate movement commands. The left/right duty number is the default calibrated duty cycle for the cart's movements. 
def move(left_fwd, right_fwd, left_rev, right_rev, left_ena, right_ena, stop=False, forward=True, left_duty=26000, right_duty=50000):
    left_ena.duty_cycle = left_duty
    right_ena.duty_cycle = right_duty
    # Boolean to stop if stopping
    left_fwd.value, left_rev.value = (stop or forward, stop or not forward)
    right_fwd.value, right_rev.value = (stop or forward, stop or not forward)

# bi-directional sensor
sonar_front = adafruit_hcsr04.HCSR04(trigger_pin=board.GP14, echo_pin=board.GP15)
sonar_back = adafruit_hcsr04.HCSR04(trigger_pin=board.GP17, echo_pin=board.GP16)

# data pins for stopping button on the left/right poles (to detect if pole reached origin)
left_stop = digitalio.DigitalInOut(board.GP18)
left_stop.direction = digitalio.Direction.INPUT
left_stop.pull = digitalio.Pull.UP
right_stop = digitalio.DigitalInOut(board.GP19)
right_stop.direction = digitalio.Direction.INPUT
right_stop.pull = digitalio.Pull.UP

# Poles motor driver board data pins:
pole_right_fwd = digitalio.DigitalInOut(board.GP12)
pole_right_rev = digitalio.DigitalInOut(board.GP11)
pole_right_fwd.direction = digitalio.Direction.OUTPUT
pole_right_rev.direction = digitalio.Direction.OUTPUT
pole_left_fwd = digitalio.DigitalInOut(board.GP10)
pole_left_rev = digitalio.DigitalInOut(board.GP9)
pole_left_fwd.direction = digitalio.Direction.OUTPUT
pole_left_rev.direction = digitalio.Direction.OUTPUT

# setup pole pwm
pole_left_ena = pwmio.PWMOut(board.GP8, duty_cycle = 65000)
pole_right_ena = pwmio.PWMOut(board.GP13, duty_cycle = 65000)

# defining the data pins to the cart motor driver board
right_fwd = digitalio.DigitalInOut(board.GP2)
right_rev = digitalio.DigitalInOut(board.GP3)
right_fwd.direction = digitalio.Direction.OUTPUT
right_rev.direction = digitalio.Direction.OUTPUT
left_fwd = digitalio.DigitalInOut(board.GP4)
left_rev = digitalio.DigitalInOut(board.GP5)
left_fwd.direction = digitalio.Direction.OUTPUT
left_rev.direction = digitalio.Direction.OUTPUT

# Communication pins with the comms/sensor deployment team
# indicate cart stopped, can deploy
sensor_go = digitalio.DigitalInOut(board.GP6)
sensor_done = digitalio.DigitalInOut(board.GP7)
sensor_go.direction = digitalio.Direction.OUTPUT
# indicate sensor retracted, can move
sensor_done.direction = digitalio.Direction.INPUT
sensor_go.value = False
# indicate position of cart (if it's at first position)
is_origin = digitalio.DigitalInOut(board.GP26)
is_origin.direction = digitalio.Direction.OUTPUT
is_origin.value = True # first deployment always origin

# Enable pins for the cart wheels
left_ena = pwmio.PWMOut(board.GP1, duty_cycle = 0)
right_ena = pwmio.PWMOut(board.GP0, duty_cycle = 0)

# wait until sensor ready
while True:
    if sensor_done.value == True:
        break;
    time.sleep(0.1)

# Reset to starting position (cart back to docker)
while True:
    print("resetting")
    # move cart until it reach pole
    move(left_fwd, right_fwd, left_rev, right_rev, left_ena, right_ena, stop=False, forward=False)
    try:
        print((sonar_back.distance,))
        if sonar_back.distance <= 10:
            move(left_fwd, right_fwd, left_rev, right_rev, left_ena, right_ena, stop=True)
            print("reached start")
            time.sleep(2)
            break
    except RuntimeError:
        print("Retrying!")
    time.sleep(0.1)

# Reset Pole until both hits reset button:
left_duty = 62500
right_duty = 33000
while True:
    move(pole_left_fwd, pole_right_fwd, pole_left_rev, pole_right_rev, pole_left_ena, pole_right_ena, forward=True, left_duty=left_duty,right_duty=right_duty)
    if left_stop.value == False:
        left_duty = 0
        print("left stop")
    if right_stop.value == False:
        right_duty = 0
        print("right stop")
    if left_duty == 0 and right_duty == 0:
        print('end')
        move(pole_left_fwd, pole_right_fwd, pole_left_rev, pole_right_rev, pole_left_ena, pole_right_ena, forward=True, left_duty=left_duty,right_duty=right_duty)
        break
    time.sleep(0.1)

# Move cart, deploy, move again (until reach end), deploy again, return, pole moves, car moves+deploy..., pole return to origin.
while True:
    # Move forward
    cart_start_time = time.time()
    while True:
        print("moving")
        move(left_fwd, right_fwd, left_rev, right_rev, left_ena, right_ena, stop=False, forward=True)
        # when time to stop and deploy:
        if 3 >= time.time() - cart_start_time >= 2:  # so only 1 pause per cycle
            move(left_fwd, right_fwd, left_rev, right_rev, left_ena, right_ena, stop=True, forward=True)
            print("time up, stop")
            # Indicate deploy ready
            # This code: this position is always the origin, so the "origin pin" is also high
            is_origin.value = True
            sensor_go.value = True
            # Wait a bit so ensure the sensor received the status pin data
            time.sleep(1)
            is_origin.value = False  # no longer origin
            sensor_go.value = False  # don't deploy now
            # wait until code done
            while True:
                if sensor_done.value == True:
                    break
                time.sleep(0.1)
        try:
            print((sonar_front.distance,))
            # move until hit end, then deploy sensor again.
            if sonar_front.distance <= 10:
                move(left_fwd, right_fwd, left_rev, right_rev, left_ena, right_ena, stop=True)
                sensor_go.value = True

                time.sleep(1)
                sensor_go.value = False  # done
                # wait until code done
                while True:
                    if sensor_done.value == True:
                        break
                    time.sleep(0.1)
                break
        except RuntimeError:
            print("Retrying!")
        time.sleep(0.1)

    # move backward until reached starting plate again.
    move(left_fwd, right_fwd, left_rev, right_rev, left_ena, right_ena, forward=False)
    while True:
        try:
            print((sonar_back.distance,))
            if sonar_back.distance <= 10:
                move(left_fwd, right_fwd, left_rev, right_rev, left_ena, right_ena, stop=True)
                print("Returned to back pole")
                time.sleep(2)
                break
        except RuntimeError:
            print("Retrying!")
        time.sleep(0.1)
    
    # Move poles. But the left pole should move for longer because it's carrying a lot of weight.
    move(pole_left_fwd, pole_right_fwd, pole_left_rev, pole_right_rev, pole_left_ena, pole_right_ena, stop=False, forward=False, left_duty=65500, right_duty=40000)
    time.sleep(3)
    move(pole_left_fwd, pole_right_fwd, pole_left_rev, pole_right_rev, pole_left_ena, pole_right_ena, stop=False, forward=False, left_duty=65500, right_duty=0)
    time.sleep(3)
    move(pole_left_fwd, pole_right_fwd, pole_left_rev, pole_right_rev, pole_left_ena, pole_right_ena, stop=True, forward=True)
    time.sleep(1)
    
    # This is the same code as the first movement, but "first position" is false now.
    cart_start_time = time.time()
    while True:
        print("moving")
        move(left_fwd, right_fwd, left_rev, right_rev, left_ena, right_ena, stop=False, forward=True)
        if 3 >= time.time() - cart_start_time >= 2:
            move(left_fwd, right_fwd, left_rev, right_rev, left_ena, right_ena, stop=True, forward=True)
            print("time up, stop")
            # Deploy code here (go value to true, wait 1 second)
            sensor_go.value = True
            time.sleep(1)
            sensor_go.value = False  # done
            # wait until code done
            while True:
                if sensor_done.value == True:
                    break
                time.sleep(0.1)
        try:
            print((sonar_front.distance,))
            if sonar_front.distance <= 10:
                move(left_fwd, right_fwd, left_rev, right_rev, left_ena, right_ena, stop=True)
                sensor_go.value = True

                time.sleep(1)
                sensor_go.value = False  # done
                # wait until code done
                while True:
                    if sensor_done.value == True:
                        break
                    time.sleep(0.1)
                break
        except RuntimeError:
            print("Retrying!")
        time.sleep(0.1)

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
        
    # return the pole back to origin.
    left_duty = 62500
    right_duty = 33000
    while True:
        move(pole_left_fwd, pole_right_fwd, pole_left_rev, pole_right_rev, pole_left_ena, pole_right_ena, forward=True, left_duty=left_duty,right_duty=right_duty)
        if left_stop.value == False:
            left_duty = 0
            print("left stop")
        if right_stop.value == False:
            right_duty = 0
            print("right stop")
        if left_duty == 0 and right_duty == 0:
            print('end')
            move(pole_left_fwd, pole_right_fwd, pole_left_rev, pole_right_rev, pole_left_ena, pole_right_ena, forward=True, left_duty=left_duty,right_duty=right_duty)
            break
        time.sleep(0.1)
