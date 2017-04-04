#!/usr/bin/python
import json
import urllib2
import datetime
import pytz

def update():
    curHour = datetime.datetime.now(pytz.timezone('US/Central')).hour + 1
    curHour = curHour % 24 # modulate by 24 hours

    f = urllib2.urlopen('http://api.wunderground.com/api/c98d157e3c6a20bb/hourly/q/TX/Austin.json')
    json_string = f.read()
    f.close()

    parsed = json.loads(json_string)
    hourly = parsed['hourly_forecast']

    minTemp = 200
    maxTemp = 0
    curTemp = 0
    curHumid = 0
    flag = 0
    for i in hourly:
        hour = int(i['FCTTIME']['hour'])
        temp = int(i['temp']['english'])
        humidity = int(i['humidity'])

        if temp > maxTemp:
            maxTemp = temp
        if temp < minTemp:
            minTemp = temp
        if hour == curHour and flag == 0:
            curTemp = temp
            curHumid = humidity
            flag = 1

    if maxTemp < 100:
        maxTemp = " " + str(maxTemp)
    maxTemp = str(maxTemp)

    if minTemp < 100:
        minTemp = " " + str(minTemp)
    minTemp = str(minTemp)

    if curTemp < 100:
        curTemp = " " + str(curTemp)
    curTemp = str(curTemp)

    if curHumid > 99:
        curHumid = 99
    curHumid = str(curHumid)

    data = "Temp "+curTemp+" Hum "+curHumid+"%High "+maxTemp+" Low "+minTemp
    return data

def main():
    print(update())

if __name__ == "__main__":
    main()
