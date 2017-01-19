import socket
import time

def main():
    REC_IP = "127.0.0.1" #receiving machines IP                           
    REC_PORT = 5000 #port receiving machine listens on                         
    SERVER_ADDR = (REC_IP,REC_PORT)
    #create the socket                                                         
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    data = raw_input("Enter the number you wish to plot")
    while True:
        sock.sendto(data, SERVER_ADDR)
        time.sleep(1)
        data = raw_input("Enter the number you wish to plot:\n")
if __name__ == '__main__':
    main()
