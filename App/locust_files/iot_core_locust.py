# Import package
from locust import User, TaskSet, events, task, between
import paho.mqtt.client as mqtt
import time
import ssl
import socket

socket.getaddrinfo('localhost', 8089)

# Define Variables
MQTT_PORT = 8883
MQTT_KEEPALIVE_INTERVAL = 60
MQTT_TOPIC = "tracker/data"
MQTT_MSG = "{\"message\":{\"valid\":true,\"timestamp\":\"9:47:34T14-7-23\",\"device_id\":\"00-00-00-00-00-00\",\"latitude\":\"21.005743\",\"longitude\":\"105.839905\",\"speed\":\"0\",\"accel_x\":\"9264\",\"accel_y\":\"17582\",\"accel_z\":\"-8003\",\"tilt_alert\":false,\"wheelie_alert\":false,\"overspeed_alert\":false,\"connection_status\":{\"cell_id\":\"00000000\",\"rsrp\":\"0\"}}}"
MQTT_HOST = "a2ht7rbdkt6040-ats.iot.ap-northeast-2.amazonaws.com"
CA_ROOT_CERT_FILE = "CA_CERT.pem"
THING_CERT_FILE = "IOT_CERT.crt"
THING_PRIVATE_KEY = "IOT_PRIVATE_KEY.key"
REQUEST_TYPE = 'MQTT'
PUBLISH_TIMEOUT = 10000
CLIENT_COUNT = 0

def fire_locust_success(**kwargs):
    events.request_success.fire(**kwargs)

def fire_locust_failure(**kwargs):
    events.request_failure.fire(**kwargs)

def increment():
    global CLIENT_COUNT
    CLIENT_COUNT = CLIENT_COUNT + 1

def time_delta(t1, t2):
    return int((t2 - t1) * 1000)

class Message(object):
    def __init__(self, type, qos, topic, payload, start_time, timeout, name):
        self.type = type
        self.qos = qos
        self.topic = topic
        self.payload = payload
        self.start_time = start_time
        self.timeout = timeout
        self.name = name
    
class PublishTask(TaskSet):
    def on_start(self):
        self.client.connect(MQTT_HOST, MQTT_PORT, MQTT_KEEPALIVE_INTERVAL)
        self.client.disconnect()

    @task(1)
    def task_pub(self):
        self.client.reconnect()
        self.client.loop_start()
        self.start_time = time.time()
        topic = MQTT_TOPIC
        payload = MQTT_MSG
        mqtt_msg_info = self.client.publish(
            topic, 
            payload, 
            qos = 0, 
            retain = False
            )
        pub_mid = mqtt_msg_info.mid
        print("Mid = " + str(pub_mid))
        self.client.pubmessage[pub_mid] = Message(
            REQUEST_TYPE, 
            0, 
            topic, 
            payload, 
            self.start_time, 
            PUBLISH_TIMEOUT, 
            str(self.client._client_id)
            )
        mqtt_msg_info.wait_for_publish()
        self.client.disconnect()
        self.client.loop_stop()
        time.sleep(5)

    waitTime = between(0.5, 10)

class MQTTLocust(User):
    tasks = {PublishTask}

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        increment()
        clientName= "Device - " + str(CLIENT_COUNT)
        self.client = mqtt.Client(clientName)
        self.client.on_connect = self.on_connect
        self.client.on_disconnect = self.on_disconnect
        self.client.on_publish = self.on_publish
        self.client.pubmessage = {}

        self.client.tls_set(
            CA_ROOT_CERT_FILE, 
            certfile = THING_CERT_FILE, 
            keyfile = THING_PRIVATE_KEY, 
            cert_reqs = ssl.CERT_REQUIRED, 
            tls_version = ssl.PROTOCOL_TLSv1_2, 
            ciphers = None
            )

    def on_connect(client, userdata, flags, rc, props=None):
        fire_locust_success(
            request_type = REQUEST_TYPE,
            name = 'connect',
            response_time = 0,
            response_length = 0
            )
    
    def on_disconnect(client, userdata, rc, props=None):
        print("Disconnect result code " + str(rc))

    def on_publish(self, client, userdata, mid):
        end_time = time.time()
        message = client.pubmessage.pop(mid, None)
        total_time = time_delta(message.start_time, end_time)
        fire_locust_success(
            request_type = REQUEST_TYPE,
            name = str(self.client._client_id),
            response_time = total_time,
            response_length = len(message.payload)
            )