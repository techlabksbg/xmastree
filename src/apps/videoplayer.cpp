#include "../app.h"
#include "../params.h"
#include "../utils.h"
#include <vector>

struct FileHeader {
    uint16_t framewidth;
    uint16_t frameheight;
    uint16_t scrolling;  // 0 No scrolling, classical video, 1 scrolling
    uint16_t bpp; // 3 rgb (3 Bytes), 1 gray (1 Byte)
};
// the image data follows in columns, top down.s

class VideoPlayer : App {
    public:
    virtual void loop();
    virtual const char* buttonName() { return "VideoPlayer"; }
    virtual bool setGoodParams();
    virtual bool loopFast() { return true; }
    virtual void stop();

    private:

    void getFileNames();
    float angle = 0.0; // PI/2;//PI/3; // 0.0 projection onto x or or PI/2 for projection onto y
    bool filesRead = false;
    std::vector<String> fileNames;

    File bitmap;
    int active = 0;
    unsigned int nextFrame = 0;
    FileHeader fileHeader;
    char* framedata=nullptr;
    size_t framesize;
    int framenumber = 0;
    bool autoplay = true;
};

// from https://randomnerdtutorials.com/esp32-microsd-card-arduino/
void VideoPlayer::getFileNames() {
    filesRead = true;
    File vids = SD.open("/vids");
    if (!vids || !vids.isDirectory()) {
        Serial.println("Directory /vids not found on SD-Card!");
        autoplay = false;
        return;
    }
    File file = vids.openNextFile();
    while (file) {
        String* fn = new String(file.name());
        Serial.println(*fn);
        if (fn->endsWith(".vid")) {
            Serial.println("   \\__ memorized");
            fileNames.push_back(*fn);
        } else {
            delete fn;
        }
        file = vids.openNextFile();
    }
    vids.close();
}

bool VideoPlayer::setGoodParams() {
    params.speed = 250;
    params.brightness = 255;
    return autoplay;
}

void VideoPlayer::stop() {
    nextFrame = 0;
    if (bitmap) {
        bitmap.close();
    }
    if (framedata != nullptr) {
        delete[] framedata;
        framedata = nullptr;
    }
    Serial.println("VideoPlayer::stop()");
    if (fileNames.capacity()>0) {
        active = (active+1)%fileNames.capacity();
    }
}

void VideoPlayer::loop() {
    if (!filesRead) {
        getFileNames();
    }
    if (fileNames.capacity()==0) {
        if (autoplay) {
            Serial.println("No /vids/*.vid file on SD-Card.");
        }
        autoplay = false;
        return;
    }
    if (millis()>nextFrame) {
        nextFrame = millis()+fmap(params.speed, 0, 255, 200, 15); // 15ms update time for 500 LEDs
        if (!bitmap) {
            bitmap = SD.open(fileNames[active]);
            if (!bitmap) {
                filesRead = false;
                return;
            }
            Serial.print("File open ");
            Serial.println(fileNames[active]);
            bitmap.readBytes((char*)(&fileHeader), sizeof(FileHeader));
            Serial.printf("Header %d x %d, scroll=%d, bpp=%d\n", fileHeader.framewidth, fileHeader.frameheight, fileHeader.scrolling, fileHeader.bpp);
            if (framedata!=nullptr) {
                delete[] framedata;
            }
            framesize = fileHeader.frameheight*fileHeader.framewidth*fileHeader.bpp;
            framedata = new char[framesize];
            framenumber = 0;
        }
        if (framenumber==0 || fileHeader.scrolling==0) {
            bitmap.readBytes(framedata, framesize);
        } else {
            char* pt1 = framedata;
            char* pt2 = framedata+fileHeader.frameheight*fileHeader.bpp;
            for (int i=0; i<(fileHeader.framewidth-1)*fileHeader.frameheight*fileHeader.bpp; i++) {
                *pt1 = *pt2;
                pt1++;
                pt2++;
            }
            //memmove(framedata, framedata+fileHeader.frameheight*fileHeader.bpp, (fileHeader.framewidth-1)*fileHeader.frameheight*fileHeader.bpp);
            bitmap.readBytes(framedata+(fileHeader.framewidth-1)*fileHeader.frameheight*fileHeader.bpp, fileHeader.frameheight*fileHeader.bpp);
        }
        framenumber++;
        float led[2];
        float mul = fileHeader.frameheight/(params.maxs[1]-params.mins[1]);
        float yoff = -params.mins[1];
        float xoff = -params.mins[0];     
        /*if (framenumber<10) {
        for (int y=0; y<fileHeader.frameheight; y++) {
            for (int x=0; x<fileHeader.framewidth; x++) {
                if (framedata[y*fileHeader.bpp + x*fileHeader.frameheight*fileHeader.bpp]) {
                    Serial.print('#');
                } else {
                    Serial.print(' ');
                }
            }
            Serial.println();
        }}*/
        for (int i=0; i<NUMPIXEL; i++) {
            RgbColor color = {0,0,0};
            led[0] = params.posdata[i][0];
            led[1] = params.posdata[i][1];
            int x = (led[0]+xoff)*mul;
            int y = (led[1]+yoff)*mul;
            if (x>=0 && y>=0 && x<fileHeader.framewidth && y<fileHeader.frameheight) {
                uint8_t* pt = (uint8_t*)framedata+y*fileHeader.bpp + x*fileHeader.frameheight*fileHeader.bpp;
                if (fileHeader.bpp==3) { 
                    color.R = *(pt+2);
                    color.G = *(pt+1);
                    color.B = *(pt+0);
                } else {
                    color = params.color1;
                    color.Darken(255-*pt);
                }
            } 
            params.pixels->SetPixelColor(i,color);
        }
        params.pixels->Show();
        if (! bitmap.available()) {
            stop();
        }
    }
}

VideoPlayer videoPlayer;
