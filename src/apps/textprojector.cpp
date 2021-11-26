#include "../app.h"
#include "../params.h"
#include "../utils.h"
#include <vector>
#include "utils.h"
#include "string"
#include "SPIFFS.h"

using namespace std;

class TextProjector : App {
    public:
    virtual void loop();
    virtual const char* buttonName() { return "Display Text"; }
    void updatetext();
    void shiftin();
    bool inited = false;
    vector<float> projectedcoords [NUMPIXEL];
    pair<int, int> finals [NUMPIXEL];
    float projectto [3] = {100, 100, 100};
    float vectocen [3] = {-projectto[0], -projectto[1], 100 - projectto[2]};
    float xproj [3];
    float yproj [3];
    float dd, dx, dy;
    float minprop = 0.3;
    float maxprop = 0.7;
    float multpfac;
    String towrite = "ABC";
    bool **on;
    int padding = 30;
    int width = 100;
    int botx;
    int imind;
    int imheight;
    File filee;
    vector<int> indx;
    vector<int> widths;
};

void TextProjector::shiftin(){
    if (botx + width> widths[imind]){
        Serial.println("newchar");
        imind += 1;
        if (imind == indx.size()){
            Serial.println("done");
            imind = 0;
            botx = -300;
        }
        String bas = "/letters/";
        filee = SPIFFS.open(bas + indx[imind] + ".txt", FILE_READ);
        filee.parseInt();
        imheight = filee.parseInt();
    }
    memmove(on, on + 1, sizeof(bool*)*(width-1));
    if (botx + width < widths[imind] - padding){
        for (int yreader = 0; yreader < imheight; yreader++){
            if (filee){
                int data = filee.parseInt();
                on[width - 1][yreader] = false;
                if (data == 1){
                    on[width - 1][yreader] = true;
                }
            }
            else{
                on[width - 1][yreader] = false;
            }
        }
        for (int zeroer = imheight; zeroer < 256; zeroer++){
            on[width - 1][zeroer] = false;
        }
        
    }
    else{
        for (int yreader = 0; yreader < 256; yreader++){
            on[width - 1][yreader] = false;
        }
    }
    botx++;
}

void TextProjector::updatetext(){
    vector<char> charcs = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', ':', '.', '-', '_', '+', '*', '"', '\\', '?', '!', '^', '%', '&'};
    indx.clear();
    int len = 0;
    for (int charc = 0; charc < towrite.length(); charc++){
        int ind = 0;
        while (ind < charcs.size() && charcs[ind] != towrite[charc]){
            ind += 1;
        }
        if (charcs[ind] == towrite[charc]){
            String bas = "/letters/";
            filee = SPIFFS.open(bas + ind + ".txt", FILE_READ);
            int width = filee.parseInt();
            len += width + padding;
            widths.push_back(len);
            indx.push_back(ind);
        }
    }

    delete on;
    on = new bool* [width];
    for (int zeroer = 0; zeroer < width; zeroer++){
        on[zeroer] = new bool [256];
        for (int setter = 0; setter < 256; setter++){
            on[zeroer][setter] = false; 
        }
    }

    botx = -300;
    imind = 0;
    String bas = "/letters/";
    filee = SPIFFS.open(bas + 0 + ".txt", FILE_READ);
    filee.parseInt();
    imheight = filee.parseInt();
}

void TextProjector::loop(){
    if (!inited){
        Serial.println("helloo");
        float lto = sqrt(vectocen[0]*vectocen[0] + vectocen[1]*vectocen[1] + vectocen[2]*vectocen[2]);
        vectocen[0] /= lto;
        vectocen[1] /= lto;
        vectocen[2] /= lto;
        xproj[2] = 0;
        xproj[1] = 1;
        if (vectocen[0] != 0){
            xproj[0] = (-vectocen[1])/(vectocen[0]);
        }
        else{
            xproj[1] = 0;
            xproj[0] = 1;
        }
        float lx = sqrt(xproj[0]*xproj[0] + xproj[1]*xproj[1] + xproj[2]*xproj[2]);
        xproj[0] /= lx;
        xproj[1] /= lx;
        xproj[2] /= lx;
        vec_cross(yproj, xproj, vectocen);
        dd = -projectto[0]*vectocen[0]-projectto[1]*vectocen[1]-projectto[2]*vectocen[2];
        dx = -projectto[0]*xproj[0]-projectto[1]*xproj[1]-projectto[2]*xproj[2];
        dy = -projectto[0]*yproj[0]-projectto[1]*yproj[1]-projectto[2]*yproj[2];
        inited = true;
        for (int point = 0; point < NUMPIXEL; point++){
            float * ledcoords = params.posdata[point];
            float dst = ledcoords[0]*vectocen[0] + ledcoords[1]*vectocen[1] + ledcoords[2]*vectocen[2] + dd;
            if(dst <= 0){
                projectedcoords[point] = {0, 1, 1};
            }
            else{
                float x = ledcoords[0]*xproj[0] + ledcoords[1]*xproj[1] + ledcoords[2]*xproj[2] + dx;
                x /= dst;
                float y = ledcoords[0]*yproj[0] + ledcoords[1]*yproj[1] + ledcoords[2]*yproj[2] + dy;
                y /= dst;
                projectedcoords[point] = {1, x, y};
            }
        }
        float xmax = -100;
        float xmin = 100;
        float ymax = -100;
        float ymin = 100;
        vector<float> pointer;
        for (int point = 0; point < NUMPIXEL; point++){
            pointer = projectedcoords[point];
            if (pointer[1] > xmax){
                xmax = pointer[1];
            }
            if (pointer[1] < xmin){
                xmin = pointer[1];
            }
            if (pointer[2] > ymax){
                ymax = pointer[2];
            }
            if (pointer[2] < ymin){
                ymin = pointer[2];
            }
        }
        multpfac = 256/(maxprop - minprop)/(ymax - ymin);
        for (int finaler = 0; finaler < NUMPIXEL; finaler++){
            if(projectedcoords[finaler][0] == 1){
                int x = (projectedcoords[finaler][1] - xmin)*multpfac;
                int y = (projectedcoords[finaler][2] - ymin - (ymax - ymin)*minprop)*multpfac;
                finals[finaler] = make_pair(x, y); //this line is probably wrong
            }
        }
        updatetext();
    }
    shiftin();
    for (int pixel = 0; pixel < NUMPIXEL; pixel++){
        params.pixels->setPixelColor(pixel, 0);
        if (finals[pixel].second > 0 && finals[pixel].second < 256 && finals[pixel].first < width){
            if (on[finals[pixel].first][finals[pixel].second]){
                params.pixels->setPixelColor(pixel, 0xFFFFFF);
            }
        }
    }
    params.pixels->show();
}

TextProjector textProjector;