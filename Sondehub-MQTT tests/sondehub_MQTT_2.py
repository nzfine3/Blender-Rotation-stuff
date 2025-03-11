import paho.mqtt.client as mqtt
import time

# Define the callback function for when the client connects to the broker
def on_connect(client, userdata, flags, rc):
    print(f"Connected with result code {rc}")
    # Subscribe to a test topic
    client.subscribe("test/topic")
    print("Subscribed to 'test/topic'")

# Define the callback function for when a message is received
def on_message(client, userdata, msg):
    print(f"Received message: {msg.payload.decode()} on topic {msg.topic}")

# Create a new MQTT client using the updated constructor
client = mqtt.Client(clean_session=True)

# Assign the callbacks
client.on_connect = on_connect
client.on_message = on_message

# Connect to the HiveMQ public broker
print("Connecting to the broker...")
client.connect("broker.hivemq.com", 1883, 60)

# Start the MQTT loop (this runs in the background to handle incoming messages)
client.loop_start()

# Wait a moment to ensure the connection is established before publishing
time.sleep(1)

# Publish a test message
print("Publishing message to 'test/topic'...")
client.publish("test/topic", "Hello from MQTT!")

# Wait a little bit for the message to be received
time.sleep(5)

# Stop the loop
client.loop_stop()

# End of script
print("Test complete.")
