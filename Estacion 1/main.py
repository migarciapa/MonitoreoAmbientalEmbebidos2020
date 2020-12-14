from machine import Pin, ADC, Timer
import machine
import time
import dht
from mq135 import MQ135
from BaseMQ import BaseMQ 
import BME280

def sub_cb(topic, msg):
  print((topic, msg))

def connect_and_subscribe():
  global client_id, mqtt_server, topic_sub
  client = MQTTClient(client_id, mqtt_server, 1883, 'admin', 'monitorEmbebidos2020')
  client.set_callback(sub_cb)
  client.connect()
  client.subscribe(topic_sub)
  print('Connected to %s MQTT broker, subscribed to %s topic' % (mqtt_server, topic_sub))
  return client

def restart_and_reconnect():
  print('Failed to connect to MQTT broker. Reconnecting...')
  time.sleep(10)
  machine.reset()

try:
  client = connect_and_subscribe()
except OSError as e:
  restart_and_reconnect()

buzz = 0

while True:
  buz = Pin(4,Pin.OUT)
  buz.value(buzz)
  sda_pin = Pin(21)
  scl_pin = Pin(22)
  i2c = machine.I2C(sda=sda_pin, scl=scl_pin)
  #[56, 57, 118]
  bme = BME280.BME280(i2c=i2c)
  pres = bme.pressure
  mqs135 = MQ135(Pin(34))
  sensor = dht.DHT11(Pin(16))
  mq7 = BaseMQ(Pin(35))
  sensor1 = Pin(17)
  rain = Pin(18)
  luz = ADC(Pin(32))
  luz.atten(ADC.ATTN_11DB)
  luzmeasure = luz.read()
  mq7measure = mq7.readCarbonMonoxide()
  sensor.measure()
  tem = sensor.temperature()
  hum = sensor.humidity()
  rzero = mqs135.get_rzero()
  corrected_rzero = mqs135.get_corrected_rzero(tem, hum)
  resistance = mqs135.get_resistance()
  ppm = mqs135.get_ppm()
  corrected_ppm = mqs135.get_corrected_ppm(tem, hum)
  no = sensor1.value()
  ra = rain.value()

  ADDR_L = 0x38 # 7bit address of the VEML6070 (write CMD, read LSB)
  ADDR_H = 0x39 # 7bit address of the VEML6070 (read MSB)
  levels = [(0,'low'),
          (2241,'moderate'),
          (4483,'high'),
          (5977,'very high'),
          (8217,'extreme')]
  i2c2 = I2C(1, scl=Pin(22), sda=Pin(21) )
  SD = 0    # shutdown (disabled)
  IT = 0b11 # integration time = 4T
  cmd = (IT << 2) | SD
  # write command to VEML6070 
  i2c2.writeto( ADDR_L, bytes([cmd]) )
  refresh_time = 500
  time.sleep_ms( refresh_time )
  # read the MSB (high byte)
  msb = i2c2.readfrom( ADDR_H, 1)[0]
  # read the LSB (low byte)
  lsb = i2c2.readfrom( ADDR_L, 1)[0]
  value = (msb <<8) | lsb # 16-bit value (unsigned)
  uv_index = ''
  for level in levels:
      if value < level[0]:
          break
      uv_index = level[1]
  if value > 5977:
	buzz = 1
  else:
	buzz = 0
  
  print('Temperature = %.2f' % tem)
  print('Humidity = %.2f' % hum)
  print('Noise = %.2f' % no)
  print('PPM = %.2f' % ppm)
  print('Corrected PPM = %.2f' % corrected_ppm)
  print('Monoxido de carbono = %.2f' % mq7measure)
  print('LLuvia = %.2f' % ra)
  print('Luz = %.2f' % luzmeasure)
  print('Presion = ' , pres)
  print( 'VEML6070: {} ({})'.format(value,uv_index) )
  tim = Timer(-1)
  tim.init(period=5000, mode=Timer.ONE_SHOT, callback=lambda t:print(1))
  time.sleep(1)
  try:
    client.check_msg()
    if (time.time() - last_message) > message_interval:
        msg = b'#%d' % tem
	msg1 = b'#%d' % hum
        msg2 = b'#%d' % no
        msg3 = b'#%d' % corrected_ppm
        msg4 = b'#%d' % mq7measure
        msg5 = b'#%d' % ra
        msg6 = b'#%d' % luzmeasure
        msg7 = pres
        msg8 = b'#%d' % value
	msg9 = b'#%d' % buzz
        client.publish(topic_pub, msg)
	client.publish(topic_pub1, msg1)
        client.publish(topic_pub2, msg2)
        client.publish(topic_pub3, msg3)
        client.publish(topic_pub4, msg4)
        client.publish(topic_pub5, msg5)
        client.publish(topic_pub6, msg6)
        client.publish(topic_pub7, msg7)
        client.publish(topic_pub8, msg8)
	client.publish(topic_pub9, msg9)
        last_message = time.time()
        counter += 1
  except OSError as e:
    restart_and_reconnect()
