import serial
import time
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

import numpy as np

# Sensor 1 has no metal ring

### CONSTANTS AND VARIABLES FOR SERIAL COMMUNICATION

SERIAL_WITH_ACCELEROMETERS = serial.Serial('/dev/ttyUSB0', 921600)
serial_buffer = bytearray()

### CONSTANTS AND VARIABLES TO INTERPRET DATA

THRESHOLDS = np.array([-45, -15, 15, 45])
letter_bins = np.zeros((5, 5), dtype=int)

letters = np.array([['a', 'b', 'c', 'd', 'e'],
           ['f', 'g', 'h', 'i', 'j'],
           ['k', 'l', 'm', 'n', 'o'],
           ['p', 'q', 'r', 's', 't'],
           ['u', 'v', 'w', 'x', 'y']])

### CONSTANTS AND VARIABLES FOR PLOTS

#DOT_SIZE = 50
#plt.ion()
#fig, ax = plt.subplots()
#scatter = ax.scatter([], [], s=DOT_SIZE)
#BIGGEST_POSSIBLE_VALUE = 130
#ax.set_xlim(-BIGGEST_POSSIBLE_VALUE, BIGGEST_POSSIBLE_VALUE)
#ax.set_ylim(-BIGGEST_POSSIBLE_VALUE, BIGGEST_POSSIBLE_VALUE)

fig, ax = plt.subplots()
cmap = plt.get_cmap('viridis')
norm = plt.Normalize(vmin=0, vmax=32)


# Annotate each cell with corresponding character
for (i, j), val in np.ndenumerate(letters):
    ax.text(j, i, val, ha='center', va='center', color='white')

ax.set_title('Character Matrix with Color Updates')
ax.set_xticks(np.arange(len(letters[0])))
ax.set_yticks(np.arange(len(letters)))

### SERIAL

def get_all_data_from_serial():
    global serial_buffer
    while SERIAL_WITH_ACCELEROMETERS.in_waiting > 0:
        serial_buffer += SERIAL_WITH_ACCELEROMETERS.read(SERIAL_WITH_ACCELEROMETERS.in_waiting)

def get_one_datapoint_from_buffer():
    global serial_buffer
    line, serial_buffer = serial_buffer.split(b'\n', 1)
    line_from_serial = line.decode('utf-8', errors='replace').strip()
    try:
        datapoint_sensor_1, datapoint_sensor_2 = line_from_serial.split(" ")
        return int(datapoint_sensor_1), int(datapoint_sensor_2)
    except ValueError:
        return None, None

### PLOT

#def plot_datapoints(x_coordinate, y_coordinate):
#    scatter.set_offsets([x_coordinate, y_coordinate])
#    fig.canvas.draw()
#    fig.canvas.flush_events()

### INTERPRET DATA

def find_and_select_letter(data_sensor_1, data_sensor_2):
    global letter_bins
    x_bin = np.digitize(data_sensor_1, THRESHOLDS)
    y_bin = np.digitize(data_sensor_2, THRESHOLDS)
    letter_bins[y_bin][x_bin] += 1
    maximum = np.max(letter_bins)
    if maximum > 30 and 2*maximum > np.sum(letter_bins):
        argmax_tuple = np.argmax(letter_bins)
        letter_bins = np.zeros((5, 5), dtype=int)
        return letters[np.unravel_index(argmax_tuple, letters.shape)]
    return None

### EXECUTION

def animate(frame):
    get_all_data_from_serial()
    while b'\n' in serial_buffer:
        data_sensor_1, data_sensor_2 = get_one_datapoint_from_buffer()
        if data_sensor_1 != None and data_sensor_2 != None:
            letter = find_and_select_letter(data_sensor_1, data_sensor_2)
            if letter:
                print(letter)

    # ax.clear()
    cax = ax.matshow(letter_bins, cmap=cmap, norm=norm)  # Display updated values
    
    
ani = FuncAnimation(fig, animate, frames=100, interval=1000)  # Update every second

plt.show()

# print(str(time.time() - START_OF_EXECUTION) + " seconds")
# print(successfull_data_transmissions) # 800 out of 1000
# print("out of " + str(NUMBER_OF_DATA_REQUEST))
