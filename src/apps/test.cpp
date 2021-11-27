#include "../app.h"
#include "../utils.h"
#include "../params.h"
#include <SPIFFS.h>

class Test : App {
    public:
    virtual void loop();
    virtual const char* buttonName() { return "Test"; }
    virtual bool loopFast() { return false; }
};

void Test::loop() {
    static auto start = millis();
    static int count = 0;
    count++;
    if (count%100==0) {
        Serial.printf("c=%d, %.1f fps\n", count, 1000.0*count/(millis()-start));
    }
}


// Instantiate (this will also register it into params)
//Test test;
