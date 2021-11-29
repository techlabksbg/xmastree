import pygame
from vector import Vector
from math import sin,cos
from time import sleep

import socket

NUMLEDS = 500

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#sock.allow_reuse_address = True
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

try:

    server_address = ('192.168.42.2', 10000)
    print('starting up on %s port %s' % server_address)
    sock.bind(server_address)
    sock.listen(1)
    sock.settimeout(10);
    size = width, height = 800,800 
    screen = pygame.display.set_mode(size)
    
    
    from posdata import leds
    leds = [Vector(led) for led in leds]
    
    
    i=0
    while True:
        i+=1
        print("Waiting for connection...")
        connection, client_address = sock.accept()
        connection.settimeout(0.5);
        print("Connection established")
        colorData = b'';
        while True:
            #print("Reading next data block...");
            fails = 0
            while len(colorData)<3*NUMLEDS:
                #print('.',)
                data = connection.recv(1024)
                if (len(data)>0):
                    # print("Got %d bytes" % len(data))
                    colorData+=bytes(data)
                else:
                    fails+=1
                    # print("0 Bytes")
                    sleep(0.2)
                    if (fails>5):
                        print("Closing connection")
                        connection.close()
                        break   
            if len(colorData)<3*NUMLEDS:
                break
            auge = Vector([0,300,100+50*sin(i/20)])
            screen.fill((0,0,0))
            for l,led in enumerate(leds):
                p = led.rotatexy(i/300).projectxz(auge)
                p *= 3
                p += Vector([400,50])
                
                pygame.draw.circle(screen, (colorData[l*3], colorData[l*3+1], colorData[l*3+2]), (p.comp[0], 800-p.comp[1]),4,2)

            pygame.display.flip()
            connection.close()
            break
            # Shift data
            colorData = colorData[3*NUMLEDS:]

except Exception as e:
    print(e)
    sock.close()
    sock = None
    
        
