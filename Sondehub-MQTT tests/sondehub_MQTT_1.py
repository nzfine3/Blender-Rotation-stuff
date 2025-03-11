import paho.mqtt.client as mqtt
import json

# Flat list to store [x1, y1, z1, x2, y2, z2, ...]
rotation_data = []

# Output file CHANGE TO ACTUAL FILE
output_file = "output_data.json"

#CHANGE WHEN NEEDED
TARGET_CALLSIGN = "SP2ROC-39"

# Callback when connected to MQTT broker
def on_connect(client, userdata, flags, rc):
    print(f"Connected to MQTT broker with result code {rc}")
    client.subscribe("sondehub/+/telemetry")  # Subscribe to all telemetry

# Callback when a message is received
def on_message(client, userdata, msg):
    global rotation_data
    try:
        payload = json.loads(msg.payload.decode())
        
        callsign = payload.get("callsign")
        if callsign != TARGET_CALLSIGN:
            return
        # Assign values directly to X, Y, Z CHANGE WHEN NEEDED
        Z = payload.get("heading")  # Yaw (Z-axis)
        Y = payload.get("pitch", 0)  # Pitch (Y-axis), default to 0 if missing
        X = payload.get("roll", 0)   # Roll (X-axis), default to 0 if missing

        # Check if Z (heading) is available to proceed
        if Z is not None:
            # Append in X, Y, Z order
            rotation_data.extend([X, Y, Z])
            print(f"[{callsign}] Captured: X={X}, Y={Y}, Z={Z}")

            # Save list to file (overwrite each time to keep latest data)
            with open(output_file, "w") as f:
                json.dump(rotation_data, f, indent=4)

    except Exception as e:
        print(f"Error handling message: {e}")

# Setup MQTT client
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

# Connect to Sondehub MQTT broker
client.connect("mqtt.sondehub.org", 1883, 60)

# Start listening
client.loop_forever()