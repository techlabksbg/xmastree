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

    virtual void loop(unsigned int &counter) {}
};
