
# proyecto.py
#--------------------------------------------------------------
# Universidad del Valle de Guatemala
# Programación de microprocesadores
#
#--------------------------------------------------------------

import csv
import time
import RPi.GPIO as GPIO
GPIO.setmode(GPIO.BCM)
GPIO.setup(23, GPIO.OUT) 
GPIO.setup(24, GPIO.OUT)

with open('calc.csv') as Draft:
    reader = csv.reader(Draft)
    next(reader)
    for row in reader: 
        temperatura = float(row[0])
        altitud = row[1]
        presion = row[2]
        time_sol = row[3]
        time_sombra = row[4]
        
    if(temperatura >= 25.0):
        GPIO.output(24, 1)
    else:
        GPIO.output(23, 1)
    time.sleep(5)
    GPIO.output(23, 0)
    GPIO.output(24, 0)
    print(temperatura)