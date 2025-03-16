import RPi.GPIO as io
import time

def adc_init():
	io.setmode(io.BCM)
	io.setwarnings(False)
	io.setup(0, io.IN)
	io.setup(1, io.IN)
	io.setup(2, io.IN)
	io.setup(3, io.IN)
	io.setup(4, io.IN)
	io.setup(5, io.IN)
	io.setup(6, io.IN)
	io.setup(7, io.IN)
	io.setup(8, io.IN)
	io.setup(9, io.IN)
	io.setup(10, io.IN)
	io.setup(11, io.IN)
	io.setup(12, io.IN)
	io.setup(13, io.IN)
	io.setup(16, io.IN)
	io.setup(17, io.IN)
	io.setup(18, io.IN)
	io.setup(19, io.IN)
	io.setup(20, io.IN)
	io.setup(21, io.IN)
	io.setup(22, io.IN)
	io.setup(23, io.OUT)
	io.setup(24, io.IN)
	io.setup(25, io.IN)
	io.setup(26, io.IN)
	io.setup(27, io.IN)
	io.output(23,0)
	


def adc0_in():
	pin0 = io.input(2) * 1
	pin1 = io.input(3) * 2
	pin2 = io.input(4) * 4
	pin3 = io.input(17) * 8
	pin4 = io.input(27) * 16
	pin5 = io.input(22) * 32
	pin6 = io.input(10) * 64
	pin7 = io.input(9) * 128
	
	return pin0 + pin1 + pin2 + pin3 + pin4 + pin5 + pin6 + pin7

def adc1_in():
	pin0 = io.input(11) * 1
	pin1 = io.input(0) * 2
	pin2 = io.input(5) * 4
	pin3 = io.input(6) * 8
	pin4 = io.input(13) * 16
	pin5 = io.input(19) * 32
	pin6 = io.input(26) * 64
	pin7 = io.input(21) * 128
	
	return pin0 + pin1 + pin2 + pin3 + pin4 + pin5 + pin6 + pin7

def adc2_in():
	pin0 = io.input(20) * 1
	pin1 = io.input(16) * 2
	pin2 = io.input(12) * 4
	pin3 = io.input(1) * 8
	pin4 = io.input(7) * 16
	pin5 = io.input(8) * 32
	pin6 = io.input(25) * 64
	pin7 = io.input(24) * 128
	
	return pin0 + pin1 + pin2 + pin3 + pin4 + pin5 + pin6 + pin7

def adc_gather():
	return adc0_in(), adc1_in(), adc2_in()
	
