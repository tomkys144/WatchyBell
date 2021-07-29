#ifndef WATCHY_BELL_H
#define WATCHY_BELL_H


#include <Watchy.h>
#include <time.h>
#include "fonts/Oswald_Regular46pt7b.h"
#include "fonts/Oswald_Regular9pt7b.h"


#define SPACING 12

#define BATTERY_WIDTH 36
#define BATTERY_HEIGHT 12
#define BATTERY_SPACING 2

#define TIME_API_URL "http://worldtimeapi.org/api/timezone/"
#define TIME_API_AREA "Europe"
#define TIME_API_REGION "Prague"

class WatchyBell : public Watchy
{
public:
    WatchyBell();
    void drawWatchFace();
    void drawTime();
    void drawInfo();
    void drawBattery();
    void MidnightMaintenance();
};

#endif