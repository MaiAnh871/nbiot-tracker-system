import paho.mqtt.client as paho
import time
import ssl
CA_CERT = r"C:\Users\Admin\Desktop\CA\locust_files\CA_CERT.pem"
IOT_CERT = r"C:\Users\Admin\Desktop\CA\locust_files\IOT_CERT.crt"
IOT_PRIVATE_KEY = r"C:\Users\Admin\Desktop\CA\locust_files\IOT_PRIVATE_KEY.key"
def on_message(client, userdata, message):
    print("message received " ,str(message.payload.decode("utf-8")))

def on_connect(client, userdata, flags, rc):
    print("Connection returned result: "+paho.connack_string(rc))

broker_address="a2ht7rbdkt6040-ats.iot.us-west-2.amazonaws.com"
print("creating new instance")
print(broker_address)

client = paho.Client("P1")
client.on_connect = on_connect
client.on_message=on_message
client.tls_set(ca_certs=CA_CERT, certfile=IOT_CERT, keyfile=IOT_PRIVATE_KEY, cert_reqs=ssl.CERT_REQUIRED, tls_version=ssl.PROTOCOL_TLSv1_2, ciphers=None)
client.tls_insecure_set(True)
client.connect(host=broker_address, port=8883, keepalive=60)
client.loop_start() #start the loop

print("Subscribing to topic","tracker/data")
client.subscribe("tracker/data")
print("Publishing message to topic","tracker/data")
client.publish("tracker/data","22",qos=0, retain=False)
time.sleep(4) # wait
client.loop_stop() #stop the loop