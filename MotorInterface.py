#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun Jan  7 17:04:58 2018

@author: jordan
"""
import socket
from time import sleep

control=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
control.connect(('192.168.1.1',2001))

stop = b'\xff\x00\x00\x00\xff'
forward=b'\xff\x00\x01\x00\xff'
backward=b'\xff\x00\x02\x00\xff'
left = b'\xff\x00\x03\x00\xff'
right = b'\xff\x00\x04\x00\xff'


control.send(forward)
sleep(1) 
control.send(stop);
sleep(1) 
control.send(backward);
sleep(1) 
control.send(stop);
sleep(1) 
control.send(left);
sleep(1) 
control.send(stop);
sleep(1) 
control.send(right);
sleep(1) 
control.send(stop);

