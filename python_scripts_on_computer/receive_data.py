import time
import serial
import numpy as np
from pynput import keyboard
import pyautogui

# Sensor 1 has no metal ring

# -- GENERAL CONSTANTS AND VARIABLES --

START_TIME = time.time()
EXECUTION_TIME_IN_SECONDS = 60

# -- CONSTANTS AND VARIABLES FOR SERIAL COMMUNICATION --

SERIAL_WITH_ACCELEROMETERS = serial.Serial('/dev/ttyUSB0', 921600)
time.sleep(2)
serial_buffer = bytearray()

# -- CONSTANTS AND VARIABLES TO INTERPRET DATA --

THRESHOLDS = np.array([-21, -7, 7, 21])
# THRESHOLDS = np.array([-45, -15, 15, 45])
CHARACTERS = np.array([['a', 'b', 'c', 'd', 'e'],
                       ['f', 'g', 'h', 'i', 'j'],
                       ['k', 'l', 'm', 'n', 'o'],
                       ['p', 'q', 'r', 's', 't'],
                       ['u', 'v', 'w', 'x', 'y']])
SECONDS_BETWEEN_ESTIMATES = 0.05
print(str(SECONDS_BETWEEN_ESTIMATES) + " seconds between estimates.")
TIME_SINCE_LAST_ESTIMATE = START_TIME
detected_characters = []

# -- SERIAL --

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

# -- INTERPRET DATA --

def assign_data_to_character(data_sensor_1, data_sensor_2):
    global detected_characters
    bin_column = np.digitize(data_sensor_1, THRESHOLDS)
    bin_row = np.digitize(data_sensor_2, THRESHOLDS)
    detected_characters.append(CHARACTERS[bin_row][bin_column])

def estimate_character():
    global detected_characters
    most_detected_character = max(set(detected_characters),
                                  key=detected_characters.count)
    print(str(len(detected_characters)) + " detected characters")
    print(str((detected_characters.count(most_detected_character)*100)
              // len(detected_characters)) + "% probability for chosen character.")
    detected_characters = []
    return most_detected_character

# -- KEYBOARD --

estimated_character = 'a'

def on_press(key):
    if key == keyboard.Key.space:
        pyautogui.write(estimated_character)

listener = keyboard.Listener(on_press=on_press)
listener.start()

# -- EXECUTION --

while time.time() - START_TIME < EXECUTION_TIME_IN_SECONDS:
    if time.time() - TIME_SINCE_LAST_ESTIMATE > SECONDS_BETWEEN_ESTIMATES:
        get_all_data_from_serial()
        while b'\n' in serial_buffer:
            data_sensor_1, data_sensor_2 = get_one_datapoint_from_buffer()
            if data_sensor_1 is not None and data_sensor_2 is not None:
                assign_data_to_character(data_sensor_1, data_sensor_2)
        if len(detected_characters) < 10:
            print("Too little characters detected for estimate.")
        else:
            estimated_character = estimate_character()
            print(estimated_character)
        TIME_SINCE_LAST_ESTIMATE = time.time()
