import serial
import time
import matplotlib.pyplot as plt
import numpy as np

number_of_data_requests = 1000

serial_with_accelerometers = serial.Serial('/dev/ttyUSB0', 921600)
time.sleep(2)    # to establish the connection via serial

def get_line(serial, timeout=1):
    start_time = time.time()
    buffer = bytearray()
    while True:
        if serial.in_waiting > 0:
            buffer += serial.read(serial.in_waiting)
            if b'\n' in buffer:
                line, buffer = buffer.split(b'\n', 1)
                return line.decode('utf-8', errors='replace').strip()
        if time.time() - start_time > timeout:
            return None

def get_accelerometer_data(serial):
    success = 1
    line_from_serial = get_line(serial)
    try:
        datapoint_sensor_1, datapoint_sensor_2 = line_from_serial.split(" ")
        data_sensor_1.append(int(datapoint_sensor_1))
        data_sensor_2.append(int(datapoint_sensor_2))
    except ValueError:
        success = 0
    return success

successfull_data_transmission = 0

start_of_execution = time.time()

def moving_average(data, window_size):
    return np.convolve(data, np.ones(window_size), 'valid') / window_size

thresholds = [235, 240, 245, 250]

def classify_signal(value):
    for i, threshold in enumerate(thresholds):
        if value < threshold:
            return i
    return 4  # If greater than all thresholds

dot_size = 50

data_sensor_1 = []
data_sensor_2 = []
plt.ion()
fig, ax = plt.subplots()
scatter = ax.scatter([], [], s=dot_size)
ax.set_xlim(-130, 130)
ax.set_ylim(-130, 130)

window_size = 10  # Adjust based on your signal characteristics

def avg(data_sensor):
    smoothed_value = moving_average(data_sensor, window_size)[-1]
    #### interval = classify_signal(smoothed_value)

    data_sensor.pop(0)  # Remove oldest value
    return smoothed_value

for datapoint_index in range(number_of_data_requests):
    successfull_data_transmission += get_accelerometer_data(serial_with_accelerometers)
    
    if len(data_sensor_1) >= window_size:
        scatter.set_offsets([(avg(data_sensor_1), avg(data_sensor_2))])
        fig.canvas.draw()
        fig.canvas.flush_events()

print(str(time.time() - start_of_execution) + " seconds")
print(successfull_data_transmission) # 823
print("out of " + str(number_of_data_requests))
