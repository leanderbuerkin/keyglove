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

MINIMUM_BIN_VALUE = 50
THRESHOLDS = np.array([-45, -15, 15, 45])
letter_bins = np.zeros((5, 5), dtype=int)

letters = np.array([['a', 'b', 'c', 'd', 'e'],
           ['f', 'g', 'h', 'i', 'j'],
           ['k', 'l', 'm', 'n', 'o'],
           ['p', 'q', 'r', 's', 't'],
           ['u', 'v', 'w', 'x', 'y']])

### CONSTANTS AND VARIABLES FOR PLOTS

fig, ax = plt.subplots()
ax.set_xlim(-0.5,4.5)
ax.set_ylim(-0.5,4.5)
#color_map = plt.get_cmap('viridis')
#color_norm = plt.Normalize(vmin=0, vmax=MINIMUM_BIN_VALUE)
#plot_letters(ax)

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

### INTERPRET DATA

def find_and_select_letter(data_sensor_1, data_sensor_2):
    global letter_bins
    x_bin = np.digitize(data_sensor_1, THRESHOLDS)
    y_bin = np.digitize(data_sensor_2, THRESHOLDS)
    letter_bins[y_bin][x_bin] += 1
    maximum = np.max(letter_bins)
    if maximum > MINIMUM_BIN_VALUE and 2*maximum > np.sum(letter_bins):
        argmax_tuple = np.argmax(letter_bins)
        letter_bins = np.zeros((5, 5), dtype=int)
        return letters[np.unravel_index(argmax_tuple, letter_bins.shape)]
    return None

### EXECUTION

def plot_letters(ax):
    for (i, j), val in np.ndenumerate(letters):
        ax.text(j, i, val, ha='center', va='center', color='black',
            bbox=dict(facecolor='white', edgecolor='none'))
    argmax_tuple = np.unravel_index(np.argmax(letter_bins), letter_bins.shape)
    ax.text(argmax_tuple[0], argmax_tuple[1],
            letters[argmax_tuple],
            ha='center', va='center', color='red',
            bbox=dict(facecolor='white', edgecolor='none'))

def main_function(frame):
    get_all_data_from_serial()
    while b'\n' in serial_buffer:
        data_sensor_1, data_sensor_2 = get_one_datapoint_from_buffer()
        if data_sensor_1 != None and data_sensor_2 != None:
            letter = find_and_select_letter(data_sensor_1, data_sensor_2)
            if letter:
                print(letter)

    plot_letters(ax)

    #ax.matshow(letter_bins, cmap=color_map, norm=color_norm)
    

ani = FuncAnimation(fig, main_function, frames=300, interval=100)

plt.show()
