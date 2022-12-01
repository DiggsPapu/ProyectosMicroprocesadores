#--------------------------------------------------------------
# proyecto.py
#--------------------------------------------------------------
# Universidad del Valle de Guatemala
# Programación de microprocesadores
#
#--------------------------------------------------------------
import BMP180_lib
import time
import csv
import json
from gpiozero import LineSensor
from datetime import datetime

detector = LineSensor(16)

def csv_insert_row(time, temperature, pressure, altitude, linea):
    with open('datos.csv', mode='a', encoding='utf-8') as csv_file: # abrir archivo
        csv_writer = csv.writer(csv_file,delimiter=',', lineterminator='\n')
        # mode=a para hacer un 'append' al csv
        csv_writer.writerow([time, temperature, pressure, altitude, linea])

csv_insert_row("Current Time","Temperature","Pressure","Altitude","Linea")
for x in range(500):
    now = datetime.now()
    current_time = now.strftime("%H:%M:%S")

    #Lectura del sensor BMP180
    temp, press, alt = BMP180_lib.readBmp180()

    # Lectura de sensor HW-511
    if detector.value == 1:
        frase = "Fuera de Linea"
    else:
        frase = "En Linea"

    #Se imprimen los datos recibidos
    print("Current Time: "+current_time+" Temperature: " + str(temp) + " °C " + " Pressure: " + str(press) + " Altitude: " + str(alt) + " Linea: " + frase)

    #Se guardan en un diccionario los datos
    csv_insert_row(current_time, temp, press, alt, frase)
    time.sleep(1)