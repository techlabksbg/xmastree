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

    void getFileNames();
    float angle = 0.0; // 0.0 projection onto x or or PI/2 for projection onto y
    bool filesRead = false;
    std::vector<String> fileNames;

};

// from https://randomnerdtutorials.com/esp32-microsd-card-arduino/
void VideoPlayer::getFileNames() {
    filesRead = true;
    File vids = SD.open("/vids");
    if (!vids || !vids.isDirectory()) return;
    File file = vids.openNextFile();
    while (file) {
        String* fn = new String(file.name());
        Serial.println(*fn);
        if (fn->endsWith(".vid")) {
            Serial.print("   \\__ memorized");
            fileNames.push_back(*fn);
        } else {
            delete fn;
        }
        file = vids.openNextFile();
    }
    vids.close();
}

bool VideoPlayer::setGoodParams() {
    params.speed = 170;
    params.brightness = 255;
    return true;
}

void VideoPlayer::loop() {
    static File bitmap;
    static int active = 0;
    static unsigned int nextFrame = 0;
    static FileHeader fileHeader;
    static char* framedata=nullptr;
    static size_t framesize;
    static int framenumber = 0;
    if (!filesRead) {
        getFileNames();
    }
    if (fileNames.capacity()==0) return;
    if (millis()>nextFrame) {
        nextFrame = millis()+fmap(params.speed, 0, 255, 200, 4);
        if (!bitmap) {
            bitmap = SD.open(fileNames[active]);
            bitmap.readBytes((char*)(&fileHeader), sizeof(FileHeader));
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
            memmove(framedata, framedata+fileHeader.frameheight*fileHeader.bpp, (fileHeader.framewidth-1)*fileHeader.frameheight*fileHeader.bpp);
            bitmap.readBytes(framedata+(fileHeader.framewidth-1)*fileHeader.frameheight*fileHeader.bpp, fileHeader.frameheight*fileHeader.bpp);
        }
        float led[3];
        float mul = fileHeader.frameheight/(params.maxs[2]-params.mins[2]);
        float zoff = -params.mins[2];
        float xoff = -params.mins[0];        
        for (int i=0; i<NUMPIXEL; i++) {
            uint32_t color = 0;
            vec_rotxy(led, params.posdata[i], -angle);
            int x = (led[0]+xoff)*mul;
            int y = (fileHeader.frameheight-(led[2]+zoff)*mul);
            if (x>=0 && y>=0 && x<fileHeader.framewidth && y<fileHeader.frameheight) {
                uint32_t color = 0;
                char* pt = framedata+y*fileHeader.bpp + x*fileHeader.frameheight*fileHeader.bpp;
                if (fileHeader.bpp==3) {
                    color = (*pt << 16) | (*(pt+1) << 8) | *(pt+2);
                } else {
                    color = scale(*pt/255.0, params.color1);
                }
            } 
            params.pixels->setPixelColor(i,color);
        }
        params.pixels->show();
        if (! bitmap.available()) {
            bitmap.close();
            delete[] framedata;
            framedata = nullptr;
            active = (active+1)%fileNames.capacity();
        }
    }

}

VideoPlayer videoPlayer;