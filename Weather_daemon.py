#!/bin/python
import pytz
import datetime
import socket
import pickle
import struct
import time
import sys
import paho.mqtt.client as mqtt

from Daemon import Daemon
import Weather_get

class Weather_Display_Daemon(Daemon):
    sock = socket.socket()
    client = mqtt.Client()

    def on_connect(client, userdata, flags, rc):
        print("Connected with result code "+str(rc))
        self.client.subscribe("weather_display")

    # Should be battery data reporting from sensor
    def on_message(client, userdata, msg):
        weather_data = weather_get.update()
        print(msg.topic+" "+str(msg.payload))

        timestamp = int(time.time())
        metrics = []
        data = j.print_str()
        curr_temp = weather_data.split()[1]
        metrics.append(('weather_sensor.current_temp', (timestamp,
            int(curr_temp))))
        metrics.append(('weather_sensor.battery_voltage', (timestamp,
            float(msg.payload))))
        
        payload = pickle.dumps(metrics, protocol=2)
        header = struct.pack("!L", len(payload))
        message = header + payload
        self.sock.sendall(message)

        self.client.publish(topic,payload=weather_get.update(),retain=False)

    def run(self):
        self.sock.connect(('192.168.1.111', 2004)) #connect to carbon port 
        self.client.on_connect = self.on_connect
        self.client.on_message = self.on_message
        self.client.connect("192.168.1.111", 1883, 60)

        self.client.loop_forever()
 
if __name__ == "__main__":
    daemon = Weather_Display_Daemon('/tmp/weather-display-daemon.pid')
    if len(sys.argv) == 2:
        if 'start' == sys.argv[1]:
            daemon.start()
            print "working"
        elif 'stop' == sys.argv[1]:
            daemon.stop()
            daemon.sock.close()
            daemon.client.disconnect()

        elif 'restart' == sys.argv[1]:
            daemon.restart()
        else:
            print "Unknown command"
            sys.exit(2)
        sys.exit(0)
    else:
        print "usage: %s start|stop|restart" % sys.argv[0]
        sys.exit(2)
	

