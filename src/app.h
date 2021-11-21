/**
 * @file app.h
 * @author Ivo Blöchliger (ivo.bloechliger@ksbg.ch)
 * @brief 
 * @version 0.1
 * @date 2021-11-15
 * 
 * 
 */

#pragma once

#include <Adafruit_NeoPixel.h>



class App {
    public:
    App();
    virtual const char* buttonName(){return "App";}
    virtual bool loopFast() { return false; }
    virtual bool setGoodParams() { return false; }
    virtual void loop() {}
    float secs();
private:
    unsigned int start=0;
    unsigned int last=0;

};
