#created by Giovanny Pesina

import tkinter as tk
from tkinter import ttk
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import matplotlib.gridspec as gridspec
import random
import numpy as np

# Initialize toggle states
rnd_rpm = True
rnd_temp = True
rnd_rpm2 = True

# Function to toggle between random and constant RPM/temperature
def toggle_cruise():
    global rnd_rpm, rnd_rpm2
    rnd_rpm2 = True
    rnd_rpm = not rnd_rpm
    if rnd_rpm:
        rpm_bttn.config(text="Switch to Constant")
    else:
        rpm_bttn.config(text="Switch to Random")

def set_slider_rpm(val):
    global rnd_rpm2, rpm
    rnd_rpm2 = False
    setrpm = float(val)
    rpm[-1] = setrpm  # Directly update the RPM value based on slider position
    slider_changed(val)

def get_current_value():
    return 'RPM: {: .2f}'.format(val.get())

def slider_changed(event):
    value_label.configure(text=get_current_value())

# Increment and decrement RPM functions
def increment_rpm():
    global rpm
    rpm[-1] += 10
    if rpm[-1] > hirpm:
        rpm[-1] = hirpm

def decrement_rpm():
    global rpm
    rpm[-1] -= 10
    if rpm[-1] < lowrpm:
        rpm[-1] = lowrpm

def increment_rpm_by_1():
    global rpm
    rpm[-1] += 1
    if rpm[-1] > hirpm:
        rpm[-1] = hirpm

def decrement_rpm_by_1():
    global rpm
    rpm[-1] -= 1
    if rpm[-1] < lowrpm:
        rpm[-1] = lowrpm

def increment_rpm_by_100():
    global rpm
    rpm[-1] += 100
    if rpm[-1] > hirpm:
        rpm[-1] = hirpm

def decrement_rpm_by_100():
    global rpm
    rpm[-1] -= 100
    if rpm[-1] < lowrpm:
        rpm[-1] = lowrpm

lowrpm = 0
hirpm = 3000

# Function to update the plot in real-time
def update_graph():
    global temperature, rpm, heatloss, line1, line2, line3, rnd_rpm, rnd_rpm2, time, voltage, line4

    if rnd_rpm and rnd_rpm2:
        # Generate random temperature and RPM data
        rpm.append(random.uniform(lowrpm, hirpm))
        temperature.append(random.uniform(50, 120))
    else:
        # Use the last rpm and temperature value for constant mode
        rpm.append(rpm[-1])
        temperature.append(temperature[-1] + random.uniform(-3, 3))

    # Calculate heat loss
    if temperature[-1] != 0:
        heatloss.append(rpm[-1] / temperature[-1])
    else:
        heatloss.append(0)

    # Generate sine wave data for voltage
    time = np.linspace(len(rpm) * 0.1, len(rpm) * 0.1 + 2 * np.pi, 256)
    phase_shift_120 = 2 * np.pi / 3
    phase_shift_240 = 4 * np.pi / 3
    
    voltage = 5 * np.sin(time + len(rpm) * 0.1) 
    voltage_shift1 = 5 * np.sin(time+ phase_shift_120 + len(rpm) * 0.1)
    voltage_shift2 = 5 * np.sin(time+ phase_shift_240 + len(rpm) * 0.1) 
    
    max_points = 50
    start_index = max(0, len(temperature) - max_points)

    # Update line plots
    line1.set_xdata(range(start_index, start_index + len(temperature[start_index:])))
    line1.set_ydata(temperature[start_index:])

    line2.set_xdata(range(start_index, start_index + len(rpm[start_index:])))
    line2.set_ydata(rpm[start_index:])

    line3.set_xdata(range(start_index, start_index + len(heatloss[start_index:])))
    line3.set_ydata(heatloss[start_index:])

    line4.set_xdata(time)
    line4.set_ydata(voltage)
    
    line5.set_xdata(time)
    line5.set_ydata(voltage_shift1)
    
    line6.set_xdata(time)
    line6.set_ydata(voltage_shift2)
    
    # Rescale axes
    ax1.relim()
    ax1.autoscale_view()
    ax2.relim()
    ax2.autoscale_view()
    ax3.relim()
    ax3.autoscale_view()
    ax4.relim()
    ax4.autoscale_view()

    # Update the RPM display
    value_label.configure(text=f'RPM: {rpm[-1]:.2f}')

    # Redraw the canvas
    canvas.draw()

    # Schedule the next update
    root.after(16, update_graph)

# Setup Tkinter
root = tk.Tk()
root.config(bg='grey')
root.title("Real-Time Graphs: Temperature, RPM, Heat Loss, and Voltage")

# Create a figure and axes
fig = plt.figure(figsize=(12, 10))
gs = gridspec.GridSpec(4, 13, figure=fig, hspace=0.5, wspace=0.3)

ax1 = fig.add_subplot(gs[0, 0:-7:])
ax2 = fig.add_subplot(gs[0, 7:])
ax3 = fig.add_subplot(gs[1, :])
ax4 = fig.add_subplot(gs[3, :]) #sin
ax1.set_facecolor('darkgrey')
ax2.set_facecolor('darkgrey')
ax3.set_facecolor('darkgrey')
ax4.set_facecolor('darkgrey')


# Initialize data
temperature = [0]
rpm = [0]
heatloss = [0]
time = np.linspace(0, 2 * np.pi, 256)
voltage = 5 * np.sin(time)

# Create initial line plots
line1, = ax1.plot(temperature, color='red', label="Temperature")
line2, = ax2.plot(rpm, color='blue', label="RPM")
line3, = ax3.plot(heatloss, color='purple', label="Heat Loss")
line4, = ax4.plot(time, voltage, color='green', label="Voltage Input (Sine Wave)")
line5, = ax4.plot(time, voltage , color='orange')
line6, = ax4.plot(time, voltage, color='purple')

# Set titles and labels
ax1.set_title("Temperature (°C)")
ax1.set_ylabel("Temperature (°C)")

ax2.set_title("RPM (Revolutions per Minute)")
ax2.set_ylabel("RPM")

ax3.set_title("Heat Loss due to Speed")
ax3.set_ylabel("Heat Loss")

ax4.set_title("Voltage Input (Sine Wave)")
ax4.set_ylabel("Voltage (V)")
ax4.set_xlabel("Time (s)")

# Create canvas
canvas = FigureCanvasTkAgg(fig, master=root)
canvas_widget = canvas.get_tk_widget()
canvas_widget.configure(bg='black')
canvas_widget.grid(row=0, column=0, columnspan=3)

# Add control buttons and slider below the canvas
rpm_bttn = ttk.Button(root, text="Switch to Constant", command=toggle_cruise)
rpm_bttn.grid(row=1, column=0, padx=5, pady=5)

val = tk.DoubleVar()
rpm_slider = ttk.Scale(
    root,
    from_=lowrpm,
    to=hirpm,
    orient='vertical',
    command=set_slider_rpm,
    variable=val
)
rpm_slider.grid(row=1, column=1, rowspan=3, sticky='ns', padx=5)

value_label = ttk.Label(root, text="RPM: 0.00")
value_label.grid(row=1, column=2, padx=5, pady=5)

# Add control buttons for RPM adjustment
increment_button = ttk.Button(root, text="Increment RPM by 10", command=increment_rpm)
increment_button.grid(row=3, column=0)

decrement_button = ttk.Button(root, text="Decrement RPM by 10", command=decrement_rpm)
decrement_button.grid(row=3, column=2)

increment_button_1 = ttk.Button(root, text="Increment RPM by 1", command=increment_rpm_by_1)
increment_button_1.grid(row=2, column=0)

decrement_button_1 = ttk.Button(root, text="Decrement RPM by 1", command=decrement_rpm_by_1)
decrement_button_1.grid(row=2, column=2)

increment_button_100 = ttk.Button(root, text="Increment RPM by 100", command=increment_rpm_by_100)
increment_button_100.grid(row=4, column=0)

decrement_button_100 = ttk.Button(root, text="Decrement RPM by 100", command=decrement_rpm_by_100)
decrement_button_100.grid(row=4, column=2)

# Quit button
def quit():
    root.quit()
    root.destroy()

quit_button = ttk.Button(root, text="Quit", command=quit)
quit_button.grid(row=5, column=1, pady=10)

# Start updating the graph
root.after(16, update_graph)

# Start the Tkinter event loop
root.mainloop()
