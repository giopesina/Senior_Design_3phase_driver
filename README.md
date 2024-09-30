# 3-Phase Driver GUI for EV

## Overview
This project involves the development of a graphical user interface (GUI) designed to provide real-time feedback for a 3-phase driver system in an electric vehicle (EV). The system monitors key performance indicators, displaying them in an intuitive and user-friendly manner. 

The core functionalities include real-time temperature monitoring, speed tracking, efficiency calculations, and an additional feature (TBD) for future expansion. The system is designed to optimize performance and ensure safety while providing engineers with actionable data.

## Features
- **Real-Time Temperature Monitoring**: Continuously tracks the temperature of the system components (e.g., motor, driver, and MOSFETs) to prevent overheating and ensure optimal performance.
- **Speed Feedback**: Displays real-time speed readings from the EV, allowing precise tracking of acceleration and velocity changes.
- **Efficiency Calculation**: Provides live updates on the efficiency of the power conversion and usage, helping to monitor system performance and energy consumption.
- **TBD Menu Option**: A customizable feature that will be implemented in the future to enhance the system’s capabilities.

## GUI Details
The GUI is designed to be both functional and easy to navigate, allowing users to monitor all key performance indicators in real-time. The interface provides clear, concise data visualizations, including charts and gauges, to help interpret the real-time feedback effectively.

### Key Visual Components:
1. **Temperature Gauges**: Graphical display showing the temperature of critical components, with alerts if temperatures exceed safe levels.
2. **Speed Indicator**: Displays current speed with a responsive dial or digital readout.
3. **Efficiency Monitor**: Shows system efficiency in percentage and graphs trends over time.
4. **TBD Option**: Reserved for future functionality based on user needs.

## Technologies Used
- **Programming Language**: Python (for backend and data handling)
- **GUI Framework**: Tkinter/PyQt (for frontend interface)
- **Real-Time Data Handling**: Integration with sensor systems for continuous data feed from the EV
- **Microcontroller**: PIC 16F1517 (controls the 3-phase driver system)
- **Communication Protocol**: I²C/SPI (for sensor communication)

# List of what to do
- **Create a skeleton of the GUI
- **Connect the fake data generator to the GUI
- **Connect the GUI to the data come from the Motor