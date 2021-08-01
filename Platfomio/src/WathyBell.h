#ifndef WATCHY_BELL_H
#define WATCHY_BELL_H

#include <Watchy.h>
#include <time.h>
#include "fonts/Oswald_Regular46pt7b.h"
#include "fonts/Oswald_Regular9pt7b.h"

// ---- GUI ----------------
#define SPACING 12

#define BATTERY_WIDTH 36
#define BATTERY_HEIGHT 12
#define BATTERY_SPACING 2

RTC_DATA_ATTR bool DARKMODE;

// ---- Battery ----------------
RTC_DATA_ATTR float BatteryMax;
RTC_DATA_ATTR float BatteryMin;

// ---- Time ----------------
#define NTP_TZ "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00"
#define NTP_SERVER "pool.ntp.org"

// ---- CLass declaration ----------------
class WatchyBell : public Watchy
{
public:
    WatchyBell();
    void drawWatchFace();
    void drawTime();
    void drawInfo();
    void drawBattery();
    void maintenance();
    void timeSync();
};

#endif