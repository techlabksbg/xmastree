#include "params.h"

#include <lwip/sockets.h>

#ifdef WIFIDEBUG


MyNeoPixel::MyNeoPixel(uint16_t n, int16_t pin, neoPixelType type) :
        Adafruit_NeoPixel(n,pin,type) 
{
    buffer = new byte[3*NUMPIXEL+6];
    buffer[0] = 'M';
    buffer[1] = 'a';
    buffer[2] = 'G';
    buffer[3] = 'i';
    buffer[4] = 'C';
    buffer[5] = '0';
    colorData = buffer+6;
    sock = 0;
}


void MyNeoPixel::begin() {
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_UDP);
    Serial.printf("Socket %d\n",sock);
    if (sock==0) return;
    /*int optval = 1; // Keep alive
    setsockopt(sock,SOL_SOCKET,SO_KEEPALIVE,&optval,sizeof(optval)); */
    struct sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	inet_pton(AF_INET, "192.168.42.2", &serverAddress.sin_addr.s_addr);
	serverAddress.sin_port = htons(10000);

	int rc = connect(sock, (struct sockaddr *)&serverAddress, sizeof(struct sockaddr_in));
    Serial.printf("connect rc=%d\n",rc);
    if (rc) {
        closesocket(sock);
        sock=0;
    }
}

void MyNeoPixel::show() {
    Adafruit_NeoPixel::show();
    if (!sock) {
        begin();
        if (!sock) {
            //Serial.println("abort show");
            return;
        }
    }

    // get ColorData
    for (int i=0; i<NUMPIXEL; i++) {
        int c = getPixelColor(i);
        colorData[i*3] = c >> 16;
        colorData[i*3+1] = (c>>8) & 0xff;
        colorData[i*3+2] = c & 0xff;
    }
    // send ColorData
    //Serial.println("Sending color data");
    int written = write(sock, buffer, 1506);
    if (written==0) {
        Serial.println("0 Bytes written, not closing socket");
        //close(sock);
        //sock = 0;
    }
}


#endif