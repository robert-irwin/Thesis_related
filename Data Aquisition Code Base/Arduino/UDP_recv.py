#!/usr/bin/env python

import struct
import socket

def main():


    MAX_BFR_SIZE = 4096

    MY_IP = "192.168.0.1" #this machines IP. (MUST be static)
    MY_PORT = 5555 #port this machine is listening on
    SRVR_ADDR = (MY_IP, MY_PORT)
    
    #create a socket
    srvr = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    #bind the IP and port to the socket
    srvr.bind(SRVR_ADDR)

    # create the format specifier string required to unpack the data
    fmt = '<HHHHf'
#    fmt = 'H'
    count = 1
    while(1):
        #listen for incoming messages
        data, client_addr = srvr.recvfrom(MAX_BFR_SIZE)

        # unpack the data
        unpackedBuf = struct.unpack(fmt, data)

        # display the message
#        print "Count: %s" % count
#        print 'from: %s\nreceived: "%s"\n' % (client_addr, unpackedBuf)
#        print "message is %s bytes\n\n" % len(data)
#        count = count + 1
        print unpackedBuf 

if __name__ == '__main__':
    main()
