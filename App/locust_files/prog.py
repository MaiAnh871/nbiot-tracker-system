import paho.mqtt.client as paho
import time

def on_message(client, userdata, message):
    print("message received " ,str(message.payload.decode("utf-8")))

def on_connect(client, userdata, flags, rc):
    print("Connection returned result: "+paho.connack_string(rc))

broker_address="broker.mqttdashboard.com"
print("creating new instance")

client = paho.Client("P1")
client.on_connect = on_connect
client.on_message=on_message
client.connect(host=broker_address, port=1883, keepalive=60)
client.loop_start() #start the loop

print("Subscribing to topic","maintopic/subtopic/tempread")
client.subscribe("maintopic/subtopic/tempread")
print("Publishing message to topic","maintopic/subtopic/tempread")
client.publish("maintopic/subtopic/tempread","22",qos=0, retain=False)
time.sleep(4) # wait
client.loop_stop() #stop the loop