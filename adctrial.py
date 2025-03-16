import RPi.GPIO as io
import time
import adcdefs as adc
adc.adc_init()

while True:
	io.output(23,0), time.sleep(0.5), io.output(23,1), time.sleep(0.5)

