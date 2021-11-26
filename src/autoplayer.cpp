#include "params.h"

void autoplayer() {
    static unsigned long lastSwitch = 0;
    static int activeProgramm = 0;
    if (millis()>lastSwitch+60*1000 || lastSwitch==0) {
        lastSwitch = millis();
        do {
            activeProgramm = (activeProgramm+1)%params.apps.size();
        } while (!params.apps[activeProgramm]->setGoodParams());
        Serial.printf("Switching to %s\n", params.apps[activeProgramm]->buttonName());
        params.newProgram = activeProgramm;
    }
}
