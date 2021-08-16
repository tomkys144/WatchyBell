/*
 File: WatchyBell.hpp
 Project: WatchyBell
 File Created: 11/08/2021, 19:00:52
 Author: Tomáš Kysela (tomkys144@gmail.com)
 -----
 Last Modified: 11/08/2021, 21:00:54
 Modified By: Tomáš Kysela
 -----
 Copyright (c) 2021 Tomáš Kysela
*/

/*
 Based on Bahn for Watchy (https://github.com/BraininaBowl/Bahn-for-Watchy)
*/

#ifndef WATCHYBELL_HPP
#define WATCHYBELL_HPP

#include "WatchyBase.hpp"

// ---- GUI ----------------
#define SPACING 12

#define BATTERY_WIDTH 36
#define BATTERY_HEIGHT 12
#define BATTERY_SPACING 2

RTC_DATA_ATTR bool DARKMODE;

// ---- Battery ----------------
RTC_DATA_ATTR float BatteryMax;
RTC_DATA_ATTR float BatteryMin;

// ---- CLass declaration ----------------
class WatchyBell : public WatchyBase
{
public:
  WatchyBell ();
  void drawWatchFace ();
  void drawTime ();
  void drawInfo ();
  void drawBattery ();
  void maintenance ();
};

#endif /* WATCHYBELL_HPP */
