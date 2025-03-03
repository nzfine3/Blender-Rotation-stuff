import bpy
import math
import time

# Get the object
obj = bpy.data.objects["Cube"]

# Flat list of rotation values in degrees (X, Y, Z, X, Y, Z, ...)
rotation_values = [-180.0,-179.63963963963964,-179.27927927927928,-178.91891891891893,-178.55855855855857,-178.1981981981982,-177.83783783783785,-177.47747747747748,-177.11711711711713,-176.75675675675677,-176.3963963963964,-176.03603603603604,-175.67567567567569,-175.31531531531533,-174.95495495495496,-174.5945945945946,-174.23423423423424,-173.87387387387387,-173.51351351351352,-173.15315315315316,-172.79279279279279,-172.43243243243244,-172.07207207207208,-171.7117117117117,-171.35135135135135,-170.990990990991,-170.63063063063064,-170.27027027027027,-169.90990990990992,-169.54954954954955,-169.18918918918919,-168.82882882882883,-168.46846846846847,-168.1081081081081,-167.74774774774775,-167.38738738738739,-167.02702702702703,-166.66666666666666,-166.3063063063063,-165.94594594594595,-165.58558558558558,-165.22522522522523,-164.86486486486487,-164.5045045045045,-164.14414414414415,-163.78378378378378,-163.42342342342343,-163.06306306306307,-162.7027027027027,-162.34234234234235,-161.98198198198198,-161.62162162162163,-161.26126126126127,-160.9009009009009,-160.54054054054055,-160.18018018018018,-159.81981981981983,-159.45945945945946,-159.0990990990991,-158.73873873873874,-158.37837837837838,-158.01801801801802,-157.65765765765766,-157.2972972972973,-156.93693693693693,-156.57657657657658,-156.21621621621622,-155.85585585585586,-155.4954954954955,-155.13513513513514,-154.77477477477477,-154.41441441441442,-154.05405405405405,-153.6936936936937,-153.33333333333334,-152.97297297297298,-152.6126126126126,-152.25225225225224,-151.89189189189188,-151.53153153153153,-151.17117117117118,-150.8108108108108,-150.45045045045044,-150.09009009009008,-149.72972972972973,-149.36936936936937,-149.00900900900902,-148.64864864864866,-148.28828828828829,-147.92792792792793,-147.56756756756758,-147.2072072072072,-146.84684684684684,-146.48648648648649,-146.12612612612613,-145.76576576576577,-145.4054054054054,-145.04504504504505,-144.68468468468468,-144.32432432432433,-143.96396396396397,-143.6036036036036,-143.24324324324325,-142.88288288288288,-142.52252252252253,-142.16216216216217,-141.8018018018018,-141.44144144144145,-141.08108108108108,-140.72072072072073,-140.36036036036037,-140.0,-139.63963963963964,-139.27927927927928,-138.91891891891893,-138.55855855855857,-138.1981981981982,-137.83783783783785,-137.47747747747748,-137.11711711711713,-136.75675675675677,-136.3963963963964,-136.03603603603604,-135.67567567567569,-135.31531531531533,-134.95495495495496,-134.5945945945946,-134.23423423423424,-133.87387387387387,-133.51351351351352,-133.15315315315316,-132.79279279279279,-132.43243243243244,-132.07207207207208,-131.7117117117117,-131.35135135135135,-130.990990990991,-130.63063063063064,-130.27027027027027,-129.90990990990992,-129.54954954954955,-129.18918918918919,-128.82882882882883,-128.46846846846847,-128.1081081081081,-127.74774774774775,-127.38738738738739,-127.02702702702703,-126.66666666666666,-126.3063063063063,-125.94594594594595,-125.58558558558558,-125.22522522522523,-124.86486486486487,-124.5045045045045,-124.14414414414415,-123.78378378378378,-123.42342342342343,-123.06306306306307,-122.7027027027027,-122.34234234234235,-121.98198198198198,-121.62162162162163,-121.26126126126127,-120.9009009009009,-120.54054054054055,-120.18018018018018,-119.81981981981983,-119.45945945945946,-119.0990990990991,-118.73873873873874,-118.37837837837838,-118.01801801801802,-117.65765765765766,-117.2972972972973,-116.93693693693693,-116.57657657657658,-116.21621621621622,-115.85585585585586,-115.4954954954955,-115.13513513513514,-114.77477477477477,-114.41441441441442,-114.05405405405405,-113.6936936936937,-113.33333333333334,-112.97297297297298,-112.6126126126126,-112.25225225225224,-111.89189189189188,-111.53153153153153,-111.17117117117118,-110.8108108108108,-110.45045045045044,-110.09009009009008,-109.72972972972973,-109.36936936936937,-109.00900900900902,-108.64864864864866,-108.28828828828829,-107.92792792792793,-107.56756756756758,-107.2072072072072,-106.84684684684684,-106.48648648648649,-106.12612612612613,-105.76576576576577,-105.4054054054054,-105.04504504504505,-104.68468468468468,-104.32432432432433,-103.96396396396397,-103.6036036036036,-103.24324324324325,-102.88288288288288,-102.52252252252253,-102.16216216216217,-101.8018018018018,-101.44144144144145,-101.08108108108108,-100.72072072072073,-100.36036036036037,-100.0,-99.63963963963964,-99.27927927927928,-98.918918]

# Convert flat list into tuples of (X, Y, Z) and apply rotations with a delay
for i in range(0, len(rotation_values), 3):
    x, y, z = rotation_values[i:i+3]  # Extract sets of three values
    obj.rotation_euler = (math.radians(x), math.radians(y), math.radians(z))  # Convert to radians
    bpy.context.view_layer.update()  # Force Blender to recognize the change
    bpy.ops.wm.redraw_timer(type='DRAW_WIN_SWAP', iterations=1)  # Update viewport
    print(f"Object rotated to: ({x}, {y}, {z}) degrees")
    time.sleep(0.05)  # Pause for 1 second to see the change