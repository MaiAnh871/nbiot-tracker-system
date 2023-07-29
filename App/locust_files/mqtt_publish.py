# Import package
import paho.mqtt.client as mqtt
import ssl
import time

# Define Variables
MQTT_PORT = 8883
MQTT_KEEPALIVE_INTERVAL = 45
MQTT_TOPIC = "iot/tracker"
MQTT_MSG = "{\"message\":{\"valid\":true,\"timestamp\":\"9:47:34T14-7-23\",\"device_id\":\"00-00-00-00-00-00\",\"latitude\":\"21.005743\",\"longitude\":\"105.839905\",\"speed\":\"0\",\"accel_x\":\"9264\",\"accel_y\":\"17582\",\"accel_z\":\"-8003\",\"tilt_alert\":false,\"wheelie_alert\":false,\"overspeed_alert\":false,\"connection_status\":{\"cell_id\":\"00000000\",\"rsrp\":\"0\"}}}"

MQTT_HOST = "a2ht7rbdkt6040-ats.iot.ap-northeast-2.amazonaws.com"
CA_ROOT_CERT_FILE = "CA_CERT.pem"
THING_CERT_FILE = "IOT_CERT.crt"
THING_PRIVATE_KEY = "IOT_PRIVATE_KEY.key"


# Define on_publish event function
def on_publish(client, userdata, mid):
	print ("Message Published...")


# Initiate MQTT Client
mqttc = mqtt.Client()

# Register publish callback function
mqttc.on_publish = on_publish

# Configure TLS Set
mqttc.tls_set(CA_ROOT_CERT_FILE, certfile=THING_CERT_FILE, keyfile=THING_PRIVATE_KEY, cert_reqs=ssl.CERT_REQUIRED, tls_version=ssl.PROTOCOL_TLSv1_2, ciphers=None)

# Connect with MQTT Broker
mqttc.connect(MQTT_HOST, MQTT_PORT, MQTT_KEEPALIVE_INTERVAL)		
mqttc.loop_start()

counter = 0
while True:
	mqttc.publish(MQTT_TOPIC,MQTT_MSG,qos=1)
	counter += 1
	time.sleep(1)

# Disconnect from MQTT_Broker
# mqttc.disconnect()