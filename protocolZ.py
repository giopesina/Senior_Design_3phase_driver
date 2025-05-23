import uartdefs as uart
import adcdefs as adc
import RPi.GPIO as io
import time
import matplotlib.pyplot as plt
import tkinter as tk
from tkinter import ttk
import numpy as np
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

adc.adc_init()
start_time = time.time()
x_data = []
y_data = []
y_data2 = []
y_data3 = []

selected = "temp"
last_time_set = 255  # default starting value

def clear_input():
    input_var.set("")

def gradual_shutdown():
    global last_time_set
    print(f"Initiating gradual shutdown from {last_time_set}...")

    for value in reversed(range(0, last_time_set + 1, 10)):
        uart.send(bytes([0x54]))
        time.sleep(0.05)
        uart.send(bytes([value]))
        if uart.read() != bytes([0x41]):
            print(f"Failed to ack value {value}")
        time.sleep(0.1)

    uart.send(bytes([0x53]))  # 'S'
    time.sleep(0.05)
    uart.send(bytes([0x30]))  # '0'
    if uart.read() == bytes([0x41]):
        print("System turned off.")

    print("Exiting GUI.")
    root.quit()


def avg_chunk_data(x, y, c_size):
    avg_x = []
    avg_y = []

    for i in range(0, len(y), c_size):
        c_x = x[i:i+c_size]
        c_y = y[i:i+c_size]

        if len(c_y) == 0:
            continue

        av_x = sum(c_x) / len(c_x)
        av_y = sum(c_y) / len(c_y)

        avg_x.append(av_x)
        avg_y.append(av_y)

    return avg_x, avg_y

def u_graph():
    elapsed_time = time.time() - start_time
    x_data.append(elapsed_time)
    y_data.append(adc.adc0_in())
    y_data2.append(adc.adc1_in())
    y_data3.append(adc.adc2_in())

    for a in ax:
        a.clear()

    ax[0].scatter(x_data, y_data, s=5, alpha=0.3, label="\"Temp\" over time")
    avg_x, avg_y = avg_chunk_data(x_data, y_data, 5)
    ax[0].plot(avg_x, avg_y, linewidth=2)
    ax[0].set_ylabel("Temp")
    ax[0].legend()

    ax[1].scatter(x_data, y_data2, s=5, alpha=0.3, label="\"Voltage\" over time")
    avg_x2, avg_y2 = avg_chunk_data(x_data, y_data2, 5)
    ax[1].plot(avg_x2, avg_y2, linewidth=2)
    ax[1].set_ylabel("Voltage")
    ax[1].legend()

    ax[2].scatter(x_data, y_data3, s=5, alpha=0.3, label="\"Current\" over time")
    avg_x3, avg_y3 = avg_chunk_data(x_data, y_data3, 5)
    ax[2].plot(avg_x3, avg_y3, linewidth=2)
    ax[2].set_ylabel("Current")
    ax[2].set_xlabel("time")
    ax[2].legend()

    for a in ax:
        if elapsed_time > 50:
            a.set_xlim(elapsed_time - 50, elapsed_time)
        else:
            a.set_xlim(0, 50)

    canvas.draw()
    root.after(16, u_graph)

def submit_input():
    global last_time_set
    try:
        ev = int(input_var.get())
    except ValueError:
        print("Invalid input")
        return

    ev = max(0, min(255, ev))
    print(f"Sending raw timeSet value: {ev} (0x{ev:02X})")

    while True:
        uart.send(bytes([0x54]))  # 'T'
        time.sleep(0.05)
        uart.send(bytes([ev]))
        if uart.read() == bytes([0x41]):
            last_time_set = ev
            print("TimeSet acknowledged.")
            break


def state_run():
    io.output(23, 0)
    while True:
        uart.send(bytes([0x53]))  # 'S'
        time.sleep(0.05)
        uart.send(bytes([0x32]))  # '2'
        if uart.read() == bytes([0x41]):
            break

def state_standby():
    io.output(23, 0)
    while True:
        uart.send(bytes([0x53]))  # 'S'
        time.sleep(0.05)
        uart.send(bytes([0x31]))  # '1'
        if uart.read() == bytes([0x41]):
            break

def state_off():
    io.output(23, 0)
    while True:
        uart.send(bytes([0x53]))  # 'S'
        time.sleep(0.05)
        uart.send(bytes([0x30]))  # '0'
        if uart.read() == bytes([0x41]):
            break

def keypad_in(num):
    current_text = input_var.get()
    input_var.set(current_text + str(num))

try:
    root = tk.Tk()
    root.title("3-Phase Motor Driver Dashboard")
    root.geometry("700x550")

    graph_frame = ttk.Frame(root)
    graph_frame.pack(side=tk.TOP, fill=tk.BOTH, expand=True)

    graph_frame_secondary = ttk.Frame(root)

    fig, ax = plt.subplots(3, 1, figsize=(6, 4))
    canvas = FigureCanvasTkAgg(fig, master=graph_frame)
    canvas_widget = canvas.get_tk_widget()
    canvas_widget.pack(fill=tk.BOTH, expand=True)

    buttons_frame = ttk.Frame(root)
    buttons_frame.pack(pady=10)

    start_button = ttk.Button(buttons_frame, text="Run", command=state_run, width=10)
    start_button.grid(row=0, column=0, padx=5)

    stop_button = ttk.Button(buttons_frame, text="Off", command=state_off, width=10)
    stop_button.grid(row=0, column=1, padx=5)

    # Keypad Input
    input_var = tk.StringVar()
    input_entry = ttk.Entry(root, textvariable=input_var, font=("Arial", 14), justify="center", width=10)
    input_entry.pack(pady=5)

    keypad = ttk.Frame(root)
    keypad.pack()

    buttons = []
    for i in range(3):
        for j in range(3):
            num = i * 3 + j + 1
            button = ttk.Button(keypad, text=str(num), command=lambda n=num: keypad_in(n), width=5)
            button.grid(row=i, column=j, padx=5, pady=5)
            buttons.append(button)

    zero_button = ttk.Button(keypad, text="0", command=lambda: keypad_in(0), width=5)
    zero_button.grid(row=3, column=1, padx=5, pady=5)

    submit_frame = ttk.Frame(root)
    submit_frame.pack(pady=10)

    submit_button = ttk.Button(submit_frame, text="Submit", command=submit_input, width=10)
    submit_button.grid(row=0, column=0, padx=5)

    clear_button = ttk.Button(submit_frame, text="Clear", command=clear_input, width=10)
    clear_button.grid(row=0, column=1, padx=5)

    off_button = ttk.Button(submit_frame, text="Shutdown", command=gradual_shutdown, width=10)
    off_button.grid(row=0, column=2, padx=5)


    u_graph()
    root.mainloop()

except KeyboardInterrupt:
    print("Closing")

finally:
    uart.close()
    print("UART connection closed.")
