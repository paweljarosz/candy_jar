#include <cstddef>
#include <cstdint>
#include "hFramework.h"
#include "hCloudClient.h"
#include <DistanceSensor.h>
#define length_of_jar 13
using namespace hModules;
/* in cm from the ultrasonic sensor to bottom of a jar
sensing longer distance activates the alarm */
int dist;
bool armed;
bool opened;
hSemaphore sem;
void cfgHandler()
{
    platform.ui.loadHtml({Resource::WEBIDE, "/ui.html"});
}
void onButtonEvent(hId id, ButtonEventType type)
{
    if (id == "arm") {
        if (type == ButtonEventType::Pressed) {
            armed = true;
        }
    }
    if (id == "disarm") {
        if (type == ButtonEventType::Pressed) {
            armed = false;
        }
    }
}
void alarmTask()
{
    while (1) {
        sem.take();
        
        for (int i = 0; i < 10; i++) {
            if (!armed) {
                break;
            }
            hServoModule.enablePower();
            sys.delay(300);
            hServoModule.disablePower();
            sys.delay(300);
        }
        sem.take(0);
    }
}
void hMain()
{
    platform.begin(&RPi);
    platform.ui.setProjectId("8e597746e026b534");
    platform.ui.configHandler = cfgHandler;
    platform.ui.onButtonEvent = onButtonEvent;
    DistanceSensor sens(hSens1);
    
    sem.take(0);
    sys.taskCreate(alarmTask);
    
    sens.init();
    dist = 0;
    armed = false;
    opened = false;
    sys.delay(100);
    hServoModule.setVoltage6V();
    hServoModule.disablePower();
    hSens4.pin1.setIn_pd();
    for (;;) {
        sys.delay(300);
        platform.ui.label("l1").setText("%u", dist);
        opened = hSens4.pin1.read();
        platform.ui.label("l2").setText("%u", opened);
        if (!opened) {
            dist = sens.getDistance();
        }
        if (armed && opened) {
            sem.give();
        }
    }
}