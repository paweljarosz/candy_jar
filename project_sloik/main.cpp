#include <cstddef>
#include <cstdint>
#include "hFramework.h"
#include "hCloudClient.h"
#include <DistanceSensor.h>

using namespace hModules;

#define length_of_jar 13
/* in cm from the ultrasonic sensor to bottom of a jar
sensing longer distance activates the alarm */

int dist;
bool armed;
bool opened;
bool alarm_active;

void cfgHandler()
{

	platform.ui.loadHtml({Resource::WEBIDE, "/ui.html"});
	// auto l = platform.ui.label("l1");
	// l.setText("LOL");
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

void alarm()
{
	alarm_active = true;
	for (int i = 0; i < 10; i++) {
		if (!armed) {
			break;
		}
		hServoModule.enablePower();
		sys.delay(300);
		hServoModule.disablePower();
		sys.delay(300);
	}
	alarm_active = false;
}

void hMain()
{
	platform.begin(&RPi);
	platform.ui.setProjectId("8e597746e026b534");
	platform.ui.configHandler = cfgHandler;
	platform.ui.onButtonEvent = onButtonEvent;

	DistanceSensor sens(hSens1);
	sens.init();
	dist = 0;
	armed = false;
	opened = false;
	alarm_active = false;
	sys.delay(100);

	hServoModule.setVoltage6V();
	hServoModule.disablePower();

	hSens4.pin1.setIn_pd();

	for (;;) {
		sys.delay(300);

		// platform.printf("dist: %u\r\n", dist);
		platform.ui.label("l1").setText("%u", dist);

		opened = hSens4.pin1.read();
		platform.ui.label("l2").setText("%u", opened);
		
		if (!opened) {
			dist = sens.getDistance();
		}

		if (armed && opened && !alarm_active) {
			sys.taskCreate(alarm);
		}
	}
}
