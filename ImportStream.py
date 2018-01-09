# -*- coding: utf-8 -*-
"""
Spyder Editor

This is a temporary script file.
"""
import matplotlib.pyplot as plt
import cv2
import urllib 
import numpy as np

stream=urllib.request.urlopen('http://192.168.1.1:8080/?action=stream')

bytes=''.encode()
n=0
while n==0:
    bytes+=stream.read(1024)
    a = bytes.find(b'\xff\xd8') #0xff 0xd8
    b = bytes.find(b'\xff\xd9')
    if a!=-1 and b!=-1:
        jpg = bytes[a:b+2]
        bytes= bytes[b+2:]
        i = cv2.imdecode(np.fromstring(jpg, dtype=np.uint8), cv2.IMREAD_COLOR)
        n+=1
        
plt.imshow(i)
