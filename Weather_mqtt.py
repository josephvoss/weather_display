#!/usr/bin/python
import pytz
import datetime
import socket
import pickle
import struct
import time
import sys
import paho.mqtt.client as mqtt

import Weather_get

def on_connect(client, userdata,  rc):
    print("Connected with result code "+str(rc))
    client.subscribe("weather_display/voltage")

# Should be battery data reporting from sensor
def on_message(client, userdata, msg):
    weather_data = Weather_get.update()
    print(msg.topic+" "+str(msg.payload))

    timestamp = int(time.time())
    metrics = []
    curr_temp = weather_data.split()[1]
    print("Uploading to Graphite. Temp: " + str(curr_temp) + ". Voltage: " +
        str(msg.payload)) 
    metrics.append(('weather_sensor.current_temp', (timestamp,
        int(curr_temp))))
    metrics.append(('weather_sensor.battery_voltage', (timestamp,
        float(msg.payload))))
    
    payload = pickle.dumps(metrics, protocol=2)
    header = struct.pack("!L", len(payload))
    message = header + payload

    sock = socket.socket()
    sock.connect(('192.168.1.111', 2004)) #connect to carbon port 
    sock.sendall(message)
    sock.close()

    client.publish("weather_display/weather",payload=weather_data,retain=False)
    
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect("192.168.1.111", 1883, 60)
client.loop_forever()
