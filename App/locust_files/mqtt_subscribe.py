# Import package
import paho.mqtt.client as mqtt
import ssl

# Define Variables
MQTT_PORT = 8883
MQTT_KEEPALIVE_INTERVAL = 45
MQTT_TOPIC = "helloTopic"
MQTT_MSG = "hello MQTT"

MQTT_HOST = "a2ht7rbdkt6040-ats.iot.ap-northeast-2.amazonaws.com"
CA_ROOT_CERT_FILE = "CA_CERT.pem"
THING_CERT_FILE = "IOT_CERT.crt"
THING_PRIVATE_KEY = "IOT_PRIVATE_KEY.key"

# Define on connect event function
# We shall subscribe to our Topic in this function
def on_connect(mosq, obj, flags, rc):
    mqttc.subscribe(MQTT_TOPIC, 0)

# Define on_message event function. 
# This function will be invoked every time,
# a new message arrives for the subscribed topic 
def on_message(mosq, obj, msg):
	print ("Topic: " + str(msg.topic))
	print ("QoS: " + str(msg.qos))
	print ("Payload: " + str(msg.payload))

def on_subscribe(mosq, obj, mid, granted_qos):
    print("Subscribed to Topic: " + 
	MQTT_MSG + " with QoS: " + str(granted_qos))

# Initiate MQTT Client
mqttc = mqtt.Client()

# Assign event callbacks
mqttc.on_message = on_message
mqttc.on_connect = on_connect
mqttc.on_subscribe = on_subscribe

# Configure TLS Set
mqttc.tls_set(CA_ROOT_CERT_FILE, certfile=THING_CERT_FILE, keyfile=THING_PRIVATE_KEY, cert_reqs=ssl.CERT_REQUIRED, tls_version=ssl.PROTOCOL_TLSv1_2, ciphers=None)


# Connect with MQTT Broker
mqttc.connect(MQTT_HOST, MQTT_PORT, MQTT_KEEPALIVE_INTERVAL)


# Continue monitoring the incoming messages for subscribed topic
mqttc.loop_forever()