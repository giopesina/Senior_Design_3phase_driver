import tkinter as tk
from tkinter import ttk
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import matplotlib.gridspec as gridspec
import random


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
def set_speed_button(val):
    global set_speed, rpm
    rnd_rpm2 = False
    setrpm = float(val)
    rpm[-1] = setrpm
    slider_changed(val)
    
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

# Increment RPM by 10
def increment_rpm():
    global rpm
    rpm[-1] += 10
    if rpm[-1] > hirpm:
        rpm[-1] = hirpm
    slider_changed(None)

# Decrement RPM by 10
def decrement_rpm():
    global rpm
    rpm[-1] -= 10
    if rpm[-1] < lowrpm:
        rpm[-1] = lowrpm
    slider_changed(None)
    
lowrpm = 0
hirpm = 3000
# Function to update the plot in real-time
def update_graph():
    global temperature, rpm, heatloss, line1, line2, line3, rnd_rpm, lowrpm, hirpm, rnd_rpm2
    
    if rnd_rpm and (rnd_rpm2 != False):
        # Generate random temperature and RPM data
        rpm.append(random.uniform(lowrpm, hirpm))
        temperature.append(random.uniform(50, 120))
    else:
        # Use the last rpm and temperature value for constant mode
        rpm.append(rpm[-1])  # Append the last value (constant RPM)
        temperature.append(temperature[-1] + random.uniform(-3, 3))  # Small fluctuation

    # Calculate heat loss (avoid dividing by zero)
    if temperature[-1] != 0:
        heatloss.append(rpm[-1] / temperature[-1])
    else:
        heatloss.append(0)

    # Update line1 (Temperature graph)
    x_temperature = range(len(temperature))
    line1.set_xdata(x_temperature)
    line1.set_ydata(temperature)

    # Update line2 (RPM graph)
    x_rpm = range(len(rpm))
    line2.set_xdata(x_rpm)
    line2.set_ydata(rpm)

    # Update line3 (Heat Loss graph)
    line3.set_xdata(x_rpm)
    line3.set_ydata(heatloss)

    # Rescale axes
    ax1.relim()
    ax1.autoscale_view()
    
    ax2.relim()
    ax2.autoscale_view()
    
    ax3.relim()
    ax3.autoscale_view()
    value_label = ttk.Label(
        root,
        text=get_current_value()
    )
    # Redraw the canvas
    canvas.draw()

    # Schedule the next update (60 updates per second)
    root.after(16, update_graph)

# Setup Tkinter
root = tk.Tk()
root.title("Real-Time Temperature, RPM, and Heat Loss Graphs")

# Create a figure and axes (3 rows, 1 column for 3 subplots)
fig = plt.figure(figsize=(12,10))
gs = gridspec.GridSpec(3,3,figure=fig)

ax1 = fig.add_subplot(gs[0,:-1])
ax2 = fig.add_subplot(gs[2,:-1])
ax3 = fig.add_subplot(gs[1,1:])

# Initialize temperature, RPM, and heat loss data
temperature = [0]
rpm = [0]
heatloss = [0]

# Create initial line plots
line1, = ax1.plot(temperature, color='red', label="Temperature")
line2, = ax2.plot(rpm, color='blue', label="RPM")
line3, = ax3.plot(heatloss, color='purple', label="Heat Loss")

# Set titles and labels for each axis
ax1.set_title("Temperature (°C)")
ax1.set_ylabel("Temperature (°C)")

ax2.set_title("RPM (Revolutions per Minute)")
ax2.set_ylabel("RPM")

ax3.set_title("Heat Loss due to Speed")
ax3.set_ylabel("Heat Loss")

# Create a canvas to place the plot inside the Tkinter window
canvas = FigureCanvasTkAgg(fig, master=root)
canvas_widget = canvas.get_tk_widget()
canvas_widget.pack(fill=tk.BOTH, expand=1)

# Add a button to toggle between random and constant RPM/temperature
rpm_bttn = ttk.Button(root, text="Switch to Constant", command=toggle_cruise)
rpm_bttn.configure(width = 40)
rpm_bttn.pack(side='left', expand=True)
val = tk.DoubleVar()
# Add a slider to control RPM
rpm_slider = ttk.Scale(
    root,
    from_=lowrpm,
    to=hirpm,
    orient='vertical',  # horizontal
    command=set_slider_rpm,
    variable = val
)
rpm_slider.pack(side='right', expand=True)
value_label = ttk.Label(
    root,
    text=get_current_value()
)

# Add buttons to increment and decrement RPM by 10
increment_button = ttk.Button(root, text="Increment RPM by 10", command=increment_rpm)
increment_button.pack(side='left', expand=True)

decrement_button = ttk.Button(root, text="Decrement RPM by 10", command=decrement_rpm)
decrement_button.pack(side='left', expand=True)

value_label.pack(side='right')
# Add a quit button to the GUI
def quit():
    root.quit()
    root.destroy()

quit_button = ttk.Button(root, text="Quit", command=quit)
quit_button.pack()

# Start updating the graph
root.after(16, update_graph)

# Start the Tkinter event loop
root.mainloop()
