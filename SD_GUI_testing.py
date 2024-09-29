
"""
Created on Thu Sep 26 22:08:41 2024

@author: megag
"""

import tkinter as tk
from tkinter import ttk
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import matplotlib.ticker as plticker
import random
import math
import time


# init toggle states
rnd_rpm = True
rnd_temp = True

# func to toggle between random and constant 
def toggle_cruise():
    global rnd_rpm, rnd_temp
    rnd_rpm = not rnd_rpm
    rnd_temp = not rnd_temp
    if rnd_rpm:
        rpm_bttn.config(text="Switch to Constant")  # Switch to "Random"
    else:
        rpm_bttn.config(text="Switch to Random")  # Switch to "Constant"

# Function to update the plot in real-time
def update_graph():
    global temperature, rpm, line1, line2, rnd_rpm

    if rnd_rpm:
        # gen rnd temp and rpm data
        rpm.append(random.uniform(0, 3000))
        temperature.append(random.uniform(50, 120))
    else:
        # use the last rpm and temperature value for constant mode
        rpm.append(rpm[-1])  # Append the last value (constant RPM)
        temperature.append(temperature[-1] + random.uniform(-3, 3))  # Add small fluctuation for realism

    # Update line1 (Temperature graph)
    x_temperature = range(len(temperature))
    line1.set_xdata(x_temperature)
    line1.set_ydata(temperature)

    # Update line2 (RPM graph)
    x_rpm = range(len(rpm))
    line2.set_xdata(x_rpm)
    line2.set_ydata(rpm)

    # rescale
    ax1.relim()
    ax1.autoscale_view()
    
    ax2.relim()
    ax2.autoscale_view()

    # redraw
    canvas.draw()

    # schedule the next update
    root.after(16, update_graph)  # Update 60 times every second



# Setup Tkinter
root = tk.Tk()
root.title("Real-Time Temperature and RPM Graphs")

# Create a plot graph figure and axes (2 subplots one for temp and one for RPM)
fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 8))  # 2 rows, 1 column of subplots
plt.subplots_adjust(hspace = 0.5)
# innit temp and RPM data
temperature = [0]  # init temperature data
rpm = [0]  # init RPM data

# Create initial line plots
line1, = ax1.plot(temperature, color='red')
line2, = ax2.plot(rpm, color='blue')

# Set titles for each graph
ax1.set_title("Temperature (°C)")
ax2.set_title("RPM (Revolutions per Minute)")

# Set labels for axes
ax1.set_xlabel("Time (seconds)")
ax1.set_ylabel("Temperature (°C)")
ax2.set_xlabel("Time (seconds)")
ax2.set_ylabel("RPM")

loc = plticker.IndexLocator()
ax2.xaxis.set_major_locator(loc)

# Create a canvas to place the plot figures inside the Tkinter window
canvas = FigureCanvasTkAgg(fig, master=root)
canvas_widget = canvas.get_tk_widget()
canvas_widget.pack(fill=tk.BOTH, expand=1)

# add a button to toggle between random and constant RPM/temperature
rpm_bttn = ttk.Button(root, text="Switch to constant", command=toggle_cruise)
rpm_bttn.pack()

# add a quit button to the GUI
def quit():
    root.quit()
    root.destroy()

quit_button = ttk.Button(root, text="Quit", command=quit)
quit_button.pack()

# Start updating the graph
root.after(16, update_graph)

# Start the Tkinter event loop
root.mainloop()
