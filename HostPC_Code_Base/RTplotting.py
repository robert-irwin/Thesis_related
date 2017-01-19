import matplotlib.pyplot as plt
import numpy as np
import time
import os
import socket
import struct
import math

def filterData(data):
    M = 150.0
    beta = math.pow(0.05, 1/M)
    A = 0.0
    oldA = A
    B = 0.0
    oldB = B
    filteredData = [] 
    for i in range(0, len(data)):
        A = (beta*oldA) + data[i]
        oldA = A
        B = (beta*oldB) + 1.0
        oldB = B
        filteredData.append(float(A/B))
    return filteredData

def main():

    MAX_BFR_SIZE = 4096
    MY_IP = "127.0.0.1" #this machines IP. (MUST be static)   
    MY_PORT = 5000 #port this machine is listening on                                              
    SRVR_ADDR = (MY_IP, MY_PORT) 
    #create a socket                                           
    srvr = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    #bind the IP and port to the socket                                                     
    srvr.bind(SRVR_ADDR)
    x = []
    t = []
    t0 = 0.0
    d = []
    v = []
    plt.figure()
    plt.ion()
    i = 0
    flag = 0
    fmt = '<ffBf'
    while(1):
        #listen for incoming messages                                               
        data, client_addr = srvr.recvfrom(MAX_BFR_SIZE) 
        i = i+1
        unpackedBuf = list(struct.unpack(fmt, data))
        if (flag == 0):
            t0 = float(unpackedBuf[1])
            flag = 1
        t.append(float(unpackedBuf[1]) - t0)
#        print 'received: "%s"\nfrom %s' % (data, client_addr)
#        print "message is %s bytes" % len(data)
        x.append(float(unpackedBuf[0]))
        d.append(float(unpackedBuf[2])/255.0)
        v.append(float(unpackedBuf[3]))
        if (i == 1000):
            y = filterData(x)
            vg = filterData(v)   
            duty = filterData(d)
            plt.subplot(2,1,1)
            plt.plot(t, y, label = "Voltage at Controller")
            plt.plot(t,vg, color = 'r', label = "Voltage at Generator")
            plt.ylabel('Voltage (V)')
            plt.ylim([50, 100])
            plt.legend()
            plt.subplot(2,1,2)
            plt.plot(t, duty)
            plt.ylabel('Duty Cycle')
            plt.xlabel('t (ms)')
            plt.ylim([0.09, 0.6])
            plt.show()
            plt.pause(0.00001)
            plt.gcf().clear()
            i = 0 
if __name__ == '__main__':
    main()

