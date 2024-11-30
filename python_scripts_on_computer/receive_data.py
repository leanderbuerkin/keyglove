import time
import serial
import matplotlib.pyplot as plt

import numpy as np

# Sensor 1 has no metal ring

### CONSTANTS AND VARIABLES FOR SERIAL COMMUNICATION

SERIAL_WITH_ACCELEROMETERS = serial.Serial('/dev/ttyUSB0', 921600)
serial_buffer = bytearray()

### CONSTANTS AND VARIABLES TO INTERPRET DATA

MINIMUM_BIN_VALUE = 50
THRESHOLDS = np.array([-45, -15, 15, 45])
character_bins = np.zeros((5, 5), dtype=int)

characters = np.array([['a', 'b', 'c', 'd', 'e'],
                    ['f', 'g', 'h', 'i', 'j'],
           ['k', 'l', 'm', 'n', 'o'],
           ['p', 'q', 'r', 's', 't'],
           ['u', 'v', 'w', 'x', 'y']])

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
        print(line_from_serial)
        datapoint_sensor_1, datapoint_sensor_2 = line_from_serial.split(" ")
        return int(datapoint_sensor_1), int(datapoint_sensor_2)
    except ValueError:
        return None, None

### INTERPRET DATA

def assign_data_to_character(data_sensor_1, data_sensor_2):
    global character_bins
    x_bin = np.digitize(data_sensor_1, THRESHOLDS)
    y_bin = np.digitize(data_sensor_2, THRESHOLDS)
    character_bins[y_bin][x_bin] += 1

def show_character():
    print(characters[np.unravel_index(np.argmax(character_bins), character_bins.shape)])


### EXECUTION

successfull_transmissions = 0
failed_transmissions = 0
START_TIME = time.time()
while time.time() - START_TIME < 10:
    get_all_data_from_serial()
    while b'\n' in serial_buffer:
        data_sensor_1, data_sensor_2 = get_one_datapoint_from_buffer()
        if data_sensor_1 is not None and data_sensor_2 is not None:
            successfull_transmissions += 1
            assign_data_to_character(data_sensor_1, data_sensor_2)
            show_character()
        else:
            failed_transmissions += 1

print("Failed Transmissions: " + str(failed_transmissions))
print("Successfull Transmissions: " + str(successfull_transmissions))
