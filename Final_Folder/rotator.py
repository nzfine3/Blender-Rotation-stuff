import sondehub
from vpython import *

import time

import threading

global t

global p

latest_data = {
    'x': 0.0, 'y': 0.0, 'z': 0.0,  # Rotation
    'alt': 0.0, 'press': 0.0, 'temp': 0.0,  # Telemetry
    'lat': 0.0, 'lon': 0.0  # GPS
}

# Since you are probally going to only be using seral number, since it also checks if something is equal the callsign, SET IT TO no_callsign when you are not using it.
specific_callsign = "StratoScience"  # Replace with real callsign if available
specific_serial = "no_serial"    # Replace with real serial number


def on_message(message):
    try:
        callsign = message.get('callsign', "no_callsign")
        serial = message.get('serial', "no_serial")

        if callsign == specific_callsign or serial == specific_serial:
            latest_data['x'] = float(message.get('rot_x', 0.0))
            latest_data['y'] = float(message.get('rot_y', 0.0))
            latest_data['alt'] = message.get('alt', 0.0)
            latest_data['press'] = message.get('press', 0.0)
            latest_data['temp'] = message.get('temp', 0.0)
            latest_data['lat'] = message.get('lat', 0.0)
            latest_data['lon'] = message.get('lon', 0.0)

            print(f"Received data from {callsign if callsign != 'no_callsign' else serial}:\n")
            print(f"Rot: {latest_data['x']}, {latest_data['y']}, {latest_data['z']}")
            print(f"Press: {latest_data['press']} hPa")
            print(f"Alt: {latest_data['alt']} m")
            print(f"Lat: {latest_data['lat']}, Lon: {latest_data['lon']}")
            print(f"Temp: {latest_data['temp']} °C\n")
            print(f"Alt: {latest_data['alt']} m, Temp: {latest_data['temp']} °C, Rot: {latest_data['x']}, {latest_data['y']}, {latest_data['z']}, Lat: {latest_data['lat']}, Lon: {latest_data['lon']}")
    except Exception as e:
        print(f"Error processing message: {e}")


def visualization_loop():
    
    # Balloon part (semi-transparent)
    balloon = sphere(pos=vector(-0.5, 1.7, 0), radius=1.8, color=color.white, opacity=0.175)

    # Payload box (body)
    payload = box(pos=vector(-0.5, -1.5, 0), length=1, height=0.6, width=0.6, color=color.cyan)

    # Antenna (thin cylinder)
    antenna = cylinder(pos=vector(-0.5, 0, 0), axis=vector(0, -1.5, 0), radius=0.075, color=color.gray(0.5))

    # Group all parts into a compound object for rotation
    sonde = compound([balloon, payload, antenna])


    
    telemetry_label = label(
        pos=vector(4, 2, 0),  # Moved away from corner, closer to center
        text='Telemetry Data',
        xoffset=20, yoffset=20,
        height=20, border=10, font='sans',
        box=True, opacity=0.6, color=color.white
    )

    
    while True:
        rate(15)  # 30 updates per second

        # Fetch latest data
        x = latest_data['x']
        y = latest_data['y']
        z = latest_data['z'] if latest_data['z'] !=0 else 0.0001  # Avoid division by zero in rotation
        alt = latest_data['alt']
        press = latest_data['press']
        temp = latest_data['temp']
        lat = latest_data['lat']
        lon = latest_data['lon']

        
        sonde.axis = vector(1, 0, 0)
        sonde.up = vector(0, 1, 0)

        
        sonde.rotate(angle=radians(x), axis=vector(1, 0, 0))  # Roll
        sonde.rotate(angle=radians(y), axis=vector(0, 1, 0))  # Pitch
        sonde.rotate(angle=radians(z), axis=vector(0, 0, 1))  # Yaw

        
        telemetry_label.text = (
            f"Altitude: {alt} m\n"
            f"Pressure: {press} hPa\n"
            f"Temperature: {temp} °C\n"
            f"Lat: {lat:.5f}, Lon: {lon:.5f}\n"
            f"RotX: {x:.2f}°, RotY: {y:.2f}°"
        )


def start_sondehub_stream():
    test = sondehub.Stream(on_message=on_message, prefix='amateur', sondes=["StratoScience"])
    while True:
        pass # Keep the thread alive

                


sondehub_thread = threading.Thread(target=start_sondehub_stream, daemon=True)
sondehub_thread.start()

visualization_loop()  # Runs in main thread