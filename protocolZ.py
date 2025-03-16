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
y_data1 = []

def u_graph():
	elapsed_time = time.time() - start_time
	x_data.append(elapsed_time)
	y_data1.append(adc.adc0_in())
	
	ax.clear()
	
	ax.plot(x_data,y_data1, label = "\"Temp\" over time")
	
	ax.set_title("3-PHASE MOTOR DRIVER OUTPUT")
	ax.set_xlabel("time")
	ax.set_ylabel("adc0 value")
	ax.legend()
	
	canvas.draw()
	root.after(16, u_graph)
	
	
def state_run():
	io.output(23,0)
	while True:
		uart.send('2')
		if uart.read() == b'A':
			io.output(23,1)
			break
def state_standby():
	io.output(23,0)
	while True:
		uart.send('1')
		if uart.read() == b'A':
			io.output(23,1)
			break
def state_off():
	io.output(23,0)
	while True:
		uart.send('0')
		if uart.read() == b'A':
			io.output(23,1)
			break

def keypad(num):
	current_text = input.var.get()
	input_var.set(current_text + str(num))
	
def submit_input():
	entered_value = input_var.get()
	input_var.set("")


try:
	root = tk.Tk()
	root.title("Expendable Multi-Graph GUI")
	root.geometry("700x550")

	graph_frame = ttk.Frame(root)
	graph_frame.pack(side = tk.TOP, fill = tk.BOTH, expand = True)

	fig, ax = plt.subplots(figsize=(6,4))

	canvas = FigureCanvasTkAgg(fig, master=graph_frame)
	canvas_widget = canvas.get_tk_widget()
	canvas_widget.pack(fill = tk.BOTH, expand=True)
	
	buttons_frame = ttk.Frame(root)
	buttons_frame.pack(pady=10)
	
	start_button = ttk.Button(buttons_frame, text="Run", command=state_run, width=10)
	start_button.grid(row=0, column=0, padx=5)
	
	stop_button = ttk.Button(buttons_frame, text="Off", command=state_off, width=10)
	stop_button.grid(row=0, column=1, padx=5)
	
	
	#keypad for user input of speed ... maybe
	input_var = tk.StringVar()
	input_entry = ttk.Entry(root, textvariable=input_var, font=("Arial", 14), justify="center", width=10)
	input_entry.pack(pady=5)
	
	keypad = ttk.Frame(root)
	keypad.pack()
	
	buttons = []
	
	for i in range(3):
		for j in range(3):
			 num = i * 3 + j + 1
			 button = ttk.Button(keypad, text=str(num), command=lambda n=num: keypad_input(n), width=5)
			 button.grid(row=i, column=j, padx=5, pady=5)
			 buttons.append(button)
	
	zero_button = ttk.Button(keypad, text=str(num), command=lambda n=num: keypad_input(0), width=5)
	zero_button.grid(row=3, column=1, padx=5, pady=5)
			 
	submit_button = ttk.Button(root, text="Submit", command=submit_input)
	submit_button.pack(pady=10)
	
			 

	graph_selection = ttk.Combobox(root, values=["Graph 1"], state = "readonly")
	graph_selection.current(0)
	graph_selection.pack(pady=10)

	u_graph()

	root.mainloop()
	

except KeyboardInterrupt:
	print("Closing")

finally:
	uart.close()
	print("line closed")

