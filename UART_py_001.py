import serial
import time

# Initialize the serial connection
ser = serial.Serial('/dev/serial0', baudrate=9600, timeout=1)

# Function to send data over UART
def usend(ans: str):
    ser.write(ans.encode())
    ser.flush()
    ser.reset_output_buffer()

# Function to read data from UART
def uread():
    reader = ser.read()
    reader = reader.hex()
    # ascii_reader = bytes.fromhex(reader).decode("utf-8")
    return reader

try:
    while True:
        sb = input("SEND: ")  # Take user input
        
        if sb == "0":
            read = ser.read()
            while read != b'1':
                usend("0")
                read = ser.read()
                print(read)
            print("Found 1")
        
        elif sb == "1":
            read = ser.read()
            while read != b'2':
                usend("1")
                read = ser.read()
                print(read)
            print("Found 2")

        elif sb == "2":
            read = ser.read()
            while read != b'0':
                usend("0")
                read = ser.read()
                print(read)
            print("Found 0")

        else:
            break

except KeyboardInterrupt:
    print("Closing")

finally:
    ser.close()
    print("Line closed")
