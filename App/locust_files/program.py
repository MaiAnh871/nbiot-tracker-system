from locust import User, TaskSet, events, task, between
import paho.mqtt.client as mqtt
import time

COUNTClient = 0
broker_address="broker.mqttdashboard.com"
REQUEST_TYPE = 'MQTT'
PUBLISH_TIMEOUT = 10000

def fire_locust_success(**kwargs):
    events.request_success.fire(**kwargs)

def increment():
    global COUNTClient
    COUNTClient = COUNTClient+1

def time_delta(t1, t2):
    return int((t2 - t1)*1000)

class Message(object):
    def __init__(self, type, qos, topic, payload, start_time, timeout, name):
        self.type = type,
        self.qos = qos,
        self.topic = topic
        self.payload = payload
        self.start_time = start_time
        self.timeout = timeout
        self.name = name

class PublishTask(TaskSet):
    def on_start(self):
        self.client.connect(host=broker_address, port=1883, keepalive=60)
        self.client.disconnect()

    @task(1)
    def task_pub(self):
        self.client.reconnect()
        self.client.loop_start()
        self.start_time = time.time()
        topic = "devices/readings/mydevice"
        payload = "Device - " + str(self.client._client_id)
        MQTTMessageInfo = self.client.publish(topic,payload,qos=0, retain=False)
        pub_mid = MQTTMessageInfo.mid
        print("Mid = " + str(pub_mid))
        self.client.pubmessage[pub_mid] = Message(
                    REQUEST_TYPE, 0, topic, payload, self.start_time, PUBLISH_TIMEOUT, str(self.client._client_id)
                    )
        MQTTMessageInfo.wait_for_publish()
        self.client.disconnect()
        self.client.loop_stop()
        time.sleep(5)

    wait_time = between(0.5, 10)

class MQTTLocust(User):
    tasks = {PublishTask}

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        increment()
        client_name = "Device - " + str(COUNTClient)
        self.client = mqtt.Client(client_name)
        self.client.on_connect = self.on_connect
        self.client.on_disconnect = self.on_disconnect
        self.client.on_publish = self.on_publish
        self.client.pubmessage  = {}

    def on_connect(client, userdata, flags, rc, props=None):
         fire_locust_success(
            request_type=REQUEST_TYPE,
            name='connect',
            response_time=0,
            response_length=0
            )

    def on_disconnect(client, userdata,rc,props=None):
        print("Disconnected result code "+str(rc))

    def on_publish(self, client, userdata, mid):
        end_time = time.time()
        message = client.pubmessage.pop(mid, None)
        total_time =  time_delta(message.start_time, end_time)
        fire_locust_success(
            request_type=REQUEST_TYPE,
            name=str(self.client._client_id),
            response_time=total_time,
            response_length=len(message.payload)
            )