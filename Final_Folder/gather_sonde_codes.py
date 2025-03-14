import sondehub

def on_message(message):
    print(message.get('serial', "no_serial"))
    print(message.get('callsign', "no_callsign"))

test = sondehub.Stream(on_message=on_message)
while 1:
    pass