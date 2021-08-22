/**
 @file WatchyBell.hpp
 @date 11/08/2021
 @author Tomáš Kysela (tomkys144@gmail.com)
 @copyright (c) 2021 Tomáš Kysela

 @attention Based on Bahn for Watchy (https://github.com/BraininaBowl/Bahn-for-Watchy)
*/

#ifndef WATCHYBELL_HPP
#define WATCHYBELL_HPP

#include "WatchyBase.hpp"

// ---- GUI ----------------
#define SPACING 12 ///< Space from edge of the display

#define BATTERY_WIDTH 36 ///< Width of the battery icon
#define BATTERY_HEIGHT 12 ///< Height of the battery icon
#define BATTERY_SPACING 2 ///< Width of the outside line of the battery icon

RTC_DATA_ATTR bool DARKMODE; ///< Darkmode state

// ---- Battery ----------------
RTC_DATA_ATTR float BatteryMax; ///< Battery maximum voltage
RTC_DATA_ATTR float BatteryMin; ///< Battery minimum voltage

/**
 @class WatchyBell

 @brief Watch face rendering
 */
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
