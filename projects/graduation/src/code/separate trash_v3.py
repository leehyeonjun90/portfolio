from ultralytics import YOLO
from picamera2 import Picamera2
import numpy as np
import cvzone
import cv2
import serial
import threading
import math
import sys
import os
import RPi.GPIO as GPIO
import time


ser = serial.Serial("/dev/ttyUSB0", baudrate=9600, timeout=1.0)

# Initialize the camera
# ls /dev/tty*
picam2 = Picamera2()
#picam2.preview_configuration.main.size = (320, 240)
picam2.preview_configuration.main.size = (640, 480)
picam2.preview_configuration.main.format = "RGB888"
picam2.preview_configuration.align()
picam2.configure("preview")
picam2.start()

Receive_DaTa=""
mode_s=0
count_c=0

model = YOLO('Separate_Trash_best.pt')

classnames  = []
with open('Separate_Trash_best.txt','r') as f:
    classnames = f.read().splitlines()

while True:

    data = ser.read(1)
    Receive_DaTa = data.decode()
    
    frame= picam2.capture_array()
    result = model(frame,stream=True)

    if Receive_DaTa == "C":
        print("Receive_DaTa")
        print(Receive_DaTa)
        mode_s=1
        Receive_DaTa=""
        data=""
        time.sleep(1)

    if mode_s==1:
        count_c=count_c+1
        none_send = 'o'
        print(count_c)
        
        if count_c >= 3:
            ser.write(none_send.encode())
            cvzone.putTextRect(frame, f'Other', [300, 200], thickness=2, scale=2)
            print("o")
            #ser.write(b'o')
            mode_s=0
            count_c=0


        for info in result:
            boxes = info.boxes
            for box in boxes:
                x1, y1, x2, y2 = box.xyxy[0]
                conf = box.conf[0]
                index_class = box.cls[0]
                conf = math.ceil(conf * 100)
                
                if count_c == 0 and conf > 60:
                    detected = True
                    
                elif count_c == 1 and conf > 50:
                    detected = True

                elif count_c == 2 and conf > 40:
                    detected = True
                else:
                    detected = False

                if detected:
                    x1, y1, x2, y2 = int(x1), int(y1), int(x2), int(y2)
                    index_class = int(index_class)
                    conf = int(conf)

                    cv2.rectangle(frame, (x1, y1), (x2, y2), (0, 255, 0), 2)

                    if index_class in [7, 11, 13, 14, 15, 21, 35, 57, 62]:
                        cvzone.putTextRect(frame, f'Can : {conf} %', [x1, y1+100], thickness=2, scale=2)
                        print("c")
                        ser.write(b'c')
                    elif index_class in [34, 38, 43, 49, 54, 61]:
                        cvzone.putTextRect(frame, f'Bottle : {conf} %', [x1, y1+100], thickness=2, scale=2)
                        print("b")
                        ser.write(b'b')
                    elif index_class in [2, 8, 9, 10, 18, 25, 26, 44, 45, 46, 47, 48, 67, 69]:
                        cvzone.putTextRect(frame, f'Paper : {conf} %', [x1, y1+100], thickness=2, scale=2)
                        print("p")
                        ser.write(b'p')
                    elif index_class in [70, 68, 66, 65]:
                        cvzone.putTextRect(frame, f'Glass : {conf} %', [x1, y1+100], thickness=2, scale=2)
                        print("g")
                        ser.write(b'g')
                    else:
                        cvzone.putTextRect(frame, f'Other : {conf} %', [x1, y1+100], thickness=2, scale=2)
                        print("o")
                        ser.write(b'o')
                        
                    cv2.imshow('Separate Trash',frame)
                    cv2.waitKey(1)

                    mode_s = 0
                    count_c = 0
                    time.sleep(1)

                print(index_class)
                print(conf)
            
    cv2.imshow('Separate Trash',frame)
    cv2.waitKey(1)
