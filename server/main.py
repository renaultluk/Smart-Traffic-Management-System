import random
import time

import paho.mqtt.client as mqtt_client

from map_loader import loadMap

broker = 'broker.emqx.io'
port = 1883
map_topic = 'isdn4000g/smart-traffic/map'
path_topic = 'isdn4000g/smart-traffic/path'
command_topic = 'isdn4000g/smart-traffic/command'
client_id = f'python-mqtt-{random.randint(0, 1000)}'

node_map = []
edge_map = []
destinations = []

def connect_mqtt():
    def on_connect(client, userdata, flags, rc):
        if rc == 0:
            print('Connected to MQTT broker')
        else:
            print(f'Connection failed. Return code: {rc}')
    
    client = mqtt_client.Client(client_id)
    client.username_pw_set('emqx', 'emqx')
    client.on_connect = on_connect
    client.connect(broker, port)
    return client

def publish(client, topic, msg):
    result = client.publish(topic, msg)
    
    status = result[0]
    if status == mqtt_client.MQTT_ERR_SUCCESS:
        print(f'Message published: {msg}')
    else:
        print(f'Publish failed: {status}')

def subscribe(client, topic):
    def on_message(client, userdata, msg):
        print(f'Message received: {msg.payload}')
    
    client.subscribe(topic)
    client.on_message = on_message

def inputHandler(client):
    while True:
        print("Commands:")
        print("1. initialize vehicle: init <VEHICLE_ID> <START_NODE_INDEX>")
        print("2. set vehicle destination: set <VEHICLE_ID> <TARGET_NODE_INDEX>")
        print("3. toggle vehicle mode: toggle <VEHICLE_ID>")
        input_list = list(map(int, input("Enter command: ").split()))
        command = input_list[0]

        if command == "init":
            if len(input_list) != 3:
                print("Error: Invalid number of parameters. Please try again")
            else:
                vehicle_id = input_list[1]
                start_node_index = input_list[2]
                msg = f'i {vehicle_id} {start_node_index}'
                publish(client, command_topic, msg)

        elif command == "toggle":
            if len(input_list) != 2:
                print("Error: Invalid number of parameters. Please try again")
            else:
                vehicle_id = input_list[1]
                msg = f't {vehicle_id}'
                publish(client, command_topic, msg)

        elif command == "set":
            if len(input_list) != 3:
                print("Error: Invalid number of parameters. Please try again")
            else:
                vehicle_id = input_list[1]
                target_node_index = input_list[2]
                msg = f'd {vehicle_id} {target_node_index}'
                publish(client, command_topic, msg)

        else:
            print("Error: Invalid command, please try again")



if __name__ == '__main__':
    client = connect_mqtt()
    map_data = loadMap()
    publish(client, map_topic, map_data)
    subscribe(client, path_topic)
    client.loop_forever()