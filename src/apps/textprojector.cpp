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
    virtual const char* buttonName() { return "Display Text"; };
    virtual bool loopFast() { return true; };
    void updatetext();
    void shiftin();
    vector<float> projectedcoords [NUMPIXEL];
    pair<int, int> finals [NUMPIXEL];
    float projectto [3] = {5000, 5000, 180};
    float vectocen [3] = {-projectto[0], -projectto[1], 100 - projectto[2]};
    float xproj [3];
    float yproj [3];
    float dd, dx, dy;
    float minprop = 0.2;
    float maxprop = 0.95;
    float multpfac;
    String towrite = "HALLO";
    bool *on;
    bool inited = false;
    int padding = 30;
    int tonex = 200;
    int width = 300;
    int botx;
    int imind;
    int imheight;
    float lassec = 100000;
    File filee;
    vector<int> indx;
    vector<int> widths;
};

void TextProjector::shiftin(){
    if (botx + width> widths[imind]){
        imind += 1;
        if (imind == indx.size()){
            imind = 0;
            botx = -1*width-tonex;
        }
        String bas = "/letters/";
        filee = SD.open(bas + indx[imind] + ".txt", FILE_READ);
        filee.parseInt();
        imheight = filee.parseInt();
    }
    memmove(on, on + 256, 256*sizeof(bool)*(width-1));
    if (botx + width > 0 && botx + width < widths[imind] - padding){
        for (int yreader = 0; yreader < imheight; yreader++){
            if (filee){
                int data = filee.parseInt();
                on[(width - 1) * 256 + yreader] = false;
                if (data == 1){
                    on[(width - 1) * 256 + yreader] = true;
                }
            }
            else{
                on[(width - 1) * 256 + yreader] = false;
            }
        }
        for (int zeroer = imheight; zeroer < 256; zeroer++){
            on[(width - 1) * 256 + zeroer] = false;
        }
        
    }
    else{
        for (int yreader = 0; yreader < 256; yreader++){
            on[(width - 1) * 256 + yreader] = false;
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
            filee = SD.open(bas + ind + ".txt", FILE_READ);
            int widthy = filee.parseInt();
            len += widthy + padding;
            widths.push_back(len);
            indx.push_back(ind);
        }
    }

    delete on;
    on = new bool [width*256];
    for (int zeroer = 0; zeroer < width*256; zeroer++){
        on[zeroer] = false;
    }

    botx = -1*width;
    imind = 0;
    String bas = "/letters/";
    filee = SD.open(bas + indx[0] + ".txt", FILE_READ);
    filee.parseInt();
    imheight = filee.parseInt();
}

void TextProjector::loop(){
    if (!inited){
        inited = true;
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
            if (pointer[2] > ymax){
                ymax = pointer[2];
            }
            if (pointer[2] < ymin){
                ymin = pointer[2];
            }
        }
        for (int point = 0; point < NUMPIXEL; point++){
            pointer = projectedcoords[point];
            if (pointer[2] > ymin + minprop*(ymax - ymin) && pointer[2] < ymin + maxprop*(ymax - ymin)){   
                if (pointer[1] > xmax){
                    xmax = pointer[1];
                }
                if (pointer[1] < xmin){
                    xmin = pointer[1];
                }
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
    }

    if(secs() < lassec || towrite != params.text){
        towrite = params.text;
        updatetext();
        lassec = secs();
    }

    if (secs() > lassec + 0.001){
        shiftin();
        for (int pixel = 0; pixel < NUMPIXEL; pixel++){
            params.pixels->setPixelColor(pixel, 0);
            if (finals[pixel].second > 0 && finals[pixel].second < 256 && finals[pixel].first < width){
                if (on[finals[pixel].first*256+finals[pixel].second]){
                    params.pixels->setPixelColor(pixel, 0xFFFFFF);
                }
            }
        }
        params.pixels->show();
        lassec = secs();
    }
}

TextProjector textProjector;