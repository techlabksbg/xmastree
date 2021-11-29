import pygame
from vector import Vector
from math import sin,cos
from time import sleep

import socket

NUMLEDS = 500

# Create a UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

try:

    server_address = ('192.168.42.2', 10000)
    print('starting up on %s port %s' % server_address)
    sock.bind(server_address)
    size = width, height = 800,800 
    screen = pygame.display.set_mode(size)
    
    
    from posdata import leds
    leds = [Vector(led) for led in leds]
    
    
    i=0
    colorData = b'';
    while True:
        i+=1
        fails = 0
        while len(colorData)<3*NUMLEDS:
            data, address = sock.recvfrom(2048)
            if (len(data)>0):
                #print("Got %d bytes" % len(data))
                colorData+=bytes(data)
                magic = colorData.find(b"MaGiC0");
                if (magic>=0 and magic<1500):
                    #print("Found magic at index %d, len(colorData)=%d" % (magic, len(colorData)))
                    colorData = colorData[(magic+6):]  #truncate data
                    #print("len(colorData)=%d" % len(colorData))

        auge = Vector([0,300,100+50*sin(i/20)])
        screen.fill((5,10,15))
        for l,led in enumerate(leds):
            p = led.rotatexy(-3.1415/2).projectxz(auge)
            p *= 3
            p += Vector([400,50])
            
            pygame.draw.circle(screen, (colorData[l*3], colorData[l*3+1], colorData[l*3+2]), (p.comp[0], 800-p.comp[1]),4,2)

        pygame.display.flip()
        # Shift data
        colorData = colorData[(3*NUMLEDS):]

except Exception as e:
    print(e)
    sock.close()
    sock = None
    
        
