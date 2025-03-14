import sondehub

# list to store captured data
captured_data = []
rotation_data = []

# specific callsign or serial number to capture data from
specific_callsign = "XXXYYYZZZ"  # callsign of sonde (if has one)
specific_serial = "24059746"    # serial number of sonde

# function that runs when each message if recived
def on_message(message):
    try:
        # Extract fields with default values if they are missing
        callsign = message.get('callsign', "no_callsign")
        serial = message.get('serial', "no_serial")
        lat = message.get('lat', "no_latitude")
        lon = message.get('lon', "no_longitude")
        heading = message.get('heading', "no_heading")
        alt = message.get('alt', "no_altitude")
        press = message.get('press', "no_pressure")
        temp = message.get('temp', "no_temperature" )
        rotationX = message.get('rotationX', "no_rotationX")
        rotationY = message.get('rotationY', "no_rotationY")
        rotationZ = message.get('rotationZ', "no_rotationZ")
        
        # Only add data if it matches the specific callsign or serial number
        if callsign == specific_callsign or serial == specific_serial:
            captured_data.extend([callsign, serial, lat, lon, heading, alt, press, temp, rotationX, rotationY, rotationZ])
            rotation_data.extend([rotationX, rotationY, rotationZ])

            # Optional: print the last added values to verify
            print([callsign, serial, lat, lon, heading, alt, press, temp, rotationX, rotationY, rotationZ])
    except Exception as e:
        print(f"Error processing message: {e}")

# initalize the stream
test = sondehub.Stream(on_message=on_message)


while True:
    pass