# Imports go at the top
from microbit import *
import time



class L298N:
    def __init__(self, pin_enable, pin_forward, pin_backward):
        self.pin_enable = pin_enable
        self.pin_forward = pin_forward
        self.pin_backward = pin_backward

    def set_value(self, num):
        self.pin_enable.write_analog(abs(num))
        if num > 0:
            self.pin_forward.write_digital(1)
            self.pin_backward.write_digital(0)
        else:
            self.pin_forward.write_digital(0)
            self.pin_backward.write_digital(1)
    


display.off()

FL = L298N(pin8, pin2, pin3)
BL = L298N(pin1, pin6, pin7)
FR = L298N(pin0, pin12, pin13)
BR = L298N(pin16, pin14, pin15)

FL.set_value(500)
FR.set_value(-550)
BL.set_value(500)
BR.set_value(-550)
time.sleep(2)
FL.set_value(0)
FR.set_value(0)
BL.set_value(0)
BR.set_value(0)