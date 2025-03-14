import bpy
import math
import time
from sondehub_MQTT_3.py import rotation_data

bpy.ops.object.text_add()

# Get the object # replace with object name
obj = bpy.data.objects["Cube"]
ob = bpy.context.object
# Flat list of rotation values in degrees (X, Y, Z, X, Y, Z, ...)
# list dosen't show up as verified becuase of .maxTokenizationLineLengh (too long to verify)


# Convert flat list into tuples of (X, Y, Z) and apply rotations with a delay
for i in range(0, len(ls), 3):
    x, y, z = ls[i:i+3]  # Extract sets of three values
    obj.rotation_euler = (math.radians(x), math.radians(y), math.radians(z))  # Convert to radians
    bpy.context.view_layer.update()  # Force Blender to recognize the change
    bpy.ops.wm.redraw_timer(type='DRAW_WIN_SWAP', iterations=1)  # Update viewport
    print(f"Object rotated to: ({x}, {y}, {z}) degrees")
    ob.data.body = i
    time.sleep(0.00001)  # Pause for 1 second to see the change