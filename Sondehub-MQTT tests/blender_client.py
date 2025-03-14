import bpy
import socket
import json
import math

# Function to receive data from the server (the sondehub_listener.py script)
def receive_data():
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.connect(('localhost', 12345))  # Connect to the server at localhost:12345
    data = client_socket.recv(1024)  # Receive data
    client_socket.close()
    return json.loads(data.decode('utf-8'))

# Function to update Blender object based on received data
def update_object_rotation():
    data = receive_data()  # Get the data from the server
    if data:
        x, y, z = data["rotationX"], data["rotationY"], data["rotationZ"]
        # Convert to radians for Blender's rotation system
        obj = bpy.data.objects["Cube"]
        obj.rotation_euler = (math.radians(x), math.radians(y), math.radians(z))
        bpy.context.view_layer.update()  # Update the view

# Run the update function
update_object_rotation()