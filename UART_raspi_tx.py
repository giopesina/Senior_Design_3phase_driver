import serial
import time

line = serial.Serial('/dev/serial0', baudrate=9600, timeout=1)
print("line open\n")

def i3i(ans: str):
    if ans == "0":
        line.write(b'0')
        line.flush()
        line.reset_output_buffer()
    
    elif ans == "1":
        line.write(b'1')
        line.flush()
        line.reset_output_buffer()
    
    elif ans == "2":
        line.write(b'2')
        line.flush()
        line.reset_output_buffer()
    
    else:
        line.write(b'A')
        line.flush()
        line.reset_output_buffer()

try:
    while True:
        i3i(input())

except KeyboardInterrupt:
    print("close line")

finally:
    line.close()
    print("line closed")
