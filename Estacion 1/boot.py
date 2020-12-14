# This file is executed on every boot (including wake-boot from deepsleep)
#import esp
#esp.osdebug(None)

import network
from machine import Pin, I2C
from time import sleep
# import ssd1306
# import framebuf

led = Pin(2, Pin.OUT)
led.on()
sleep(0.2)
led.off()

ap = network.WLAN(network.AP_IF)
ap.active(True)
ap.config(essid='Embebidos2020.auto')
ap.config(authmode=3, password='12345678')

import webrepl
webrepl.start()

from umqttsimple import MQTTClient
import ubinascii
import micropython
import machine
import esp
esp.osdebug(None)
import gc
gc.collect()

ssid = 'Saavedra 2.4'
password = '25042130'
mqtt_server = 'monitorse.duckdns.org'
#EXAMPLE IP ADDRESS
#mqtt_server = '192.168.1.144'
client_id = ubinascii.hexlify(machine.unique_id())
topic_sub = b'Rest1/Test'
topic_pub = b'Rest1/temp'
topic_pub1 = b'Rest1/hum'
topic_pub2 = b'Rest1/ruido'
topic_pub3 = b'Rest1/ppm'
topic_pub4 = b'Rest1/mon'
topic_pub5 = b'Rest1/lluvia'
topic_pub6 = b'Rest1/luz'
topic_pub7 = b'Rest1/pres'
topic_pub8 = b'Rest1/uv'
topic_pub9 = b'Rest1/Buzzer'

last_message = 0
message_interval = 1
counter = 0


station = network.WLAN(network.STA_IF)

station.active(True)
station.connect(ssid, password)

while station.isconnected() == False:
  pass

print('Connection successful')
print(station.ifconfig())

led.on()
