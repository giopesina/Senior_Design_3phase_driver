# -*- coding: utf-8 -*-
"""
Created on Thu Sep 26 21:14:17 2024

@author: Zarek
"""

import numpy as np
import matplotlib.pyplot as plt
import time
import signal
import sys

temperature_data = []  # List to store temperature readings over time
speed_data = []        # List to store speed readings over time
time_data = []         # List to store the elapsed time corresponding to each reading

# Function to handle program exit when the user presses Ctrl+C
def signal_handler(sig, frame):
    print("\nProgram terminated. Plotting results...")
    # Call the function to plot results after termination
    plot_results()
    # Exit the program
    sys.exit(0)

# Register the signal handler for Ctrl+C, so it triggers signal_handler() when interrupted
signal.signal(signal.SIGINT, signal_handler)

# Function to generate random temperature and speed values (our fake motor readings)
def generate_fake_data():
    # Generate a random temperature between 50°C and 120°C
    temperature = np.random.uniform(50, 120)
    # Generate a random speed between 0 and 3000 RPM
    speed = np.random.uniform(0, 3000)
    return temperature, speed

# Function to plot the temperature and speed data over time after the program stops
def plot_results():
    # Create a new figure with 2 subplots (one for temperature, one for speed)
    plt.figure(figsize=(10, 5))
    
    # First subplot: Temperature over time
    plt.subplot(2, 1, 1)  # 2 rows, 1 column, 1st subplot
    plt.plot(time_data, temperature_data, label="Temperature (°C)", color="red")
    plt.xlabel("Time (s)")
    plt.ylabel("Temperature (°C)")
    plt.title("Motor Temperature Over Time")
    plt.legend()  # Show a legend for the graph
    
    # Second subplot: Speed over time
    plt.subplot(2, 1, 2)  # 2 rows, 1 column, 2nd subplot
    plt.plot(time_data, speed_data, label="Speed (RPM)", color="blue")
    plt.xlabel("Time (s)")
    plt.ylabel("Speed (RPM)")
    plt.title("Motor Speed Over Time")
    plt.legend()  # Show a legend for the graph
    
    # Adjust layout to avoid overlap of titles and labels
    plt.tight_layout()
    # Display the plots
    plt.show()

# Main function that will continuously run and generate data 60 times per second
def main():
    # Record the time when the program starts, for calculating elapsed time later
    start_time = time.time()
    
    while True:  # Infinite loop (until manually stopped with Ctrl+C)
        # Generate random temperature and speed values using the fake data generator
        temperature, speed = generate_fake_data()
        
        # Calculate the elapsed time since the program started
        elapsed_time = time.time() - start_time
        
        # Append the generated data and elapsed time to our lists for later plotting
        temperature_data.append(temperature)
        speed_data.append(speed)
        time_data.append(elapsed_time)
        
        # Print the current temperature and speed to the console
        print(f"Temperature: {temperature:.2f}°C, Speed: {speed:.2f} RPM")
        
        # Pause for a short time (1/60 seconds) to achieve 60 updates per second
        # 1 second / 60 = approximately 0.01667 seconds
        time.sleep(1 / 60)

# Check if this script is being run directly (not imported as a module)
if __name__ == "__main__":
    # Call the main function to start generating data
    main()