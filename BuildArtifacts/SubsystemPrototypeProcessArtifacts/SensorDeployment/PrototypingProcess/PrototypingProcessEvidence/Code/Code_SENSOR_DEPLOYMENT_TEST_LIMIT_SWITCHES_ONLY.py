# SPDX-FileCopyrightText: 2021 Kattni Rembor for Adafruit Industries
#
# SPDX-License-Identifier: MIT

"""Example for Pico. Blinks the built-in LED."""
import time
import board
import digitalio

led = digitalio.DigitalInOut(board.LED)
led.direction = digitalio.Direction.OUTPUT

limit_switch_1 = digitalio.DigitalInOut(board.GP15)
limit_switch_1.direction = digitalio.Direction.INPUT

limit_switch_2 = digitalio.DigitalInOut(board.GP16)
limit_switch_2.direction = digitalio.Direction.INPUT

while True:
    print(limit_switch_1.value, limit_switch_2.value)
    time.sleep(0.1)
