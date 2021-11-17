#include "app.h"
#include "params.h"

App::App() {
    params.apps.push_back(this);
}

float App::secs() {
    float t = 0.0;  // Get time in seconds
    if (millis()-last<1000) { 
        t = (millis()-start)/1000.0;
    } else {
        start = millis();
        t = 0.0;
    }    
    last = millis();
    return t;
}