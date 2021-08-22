/**
 @file WatchyBell.cpp
 @date 11/08/2021
 @author Tomáš Kysela (tomkys144@gmail.com)
 @copyright (c) 2021 Tomáš Kysela
*/

#include "WatchyBell.hpp"

bool DARKMODE = true;

float BatteryMax = 4.1;
float BatteryMin = 3.0;

/**
 @brief Construct a new Watchy Bell:: Watchy Bell object
 */
WatchyBell::WatchyBell () {} // constructor

/**
 @brief Rendering watch face
 */
void WatchyBell::drawWatchFace ()
{
  // ---- Maintenance ----------------
  maintenance ();

  // ---- Alarm ----------------
  WatchyBase::triggerAlarm(DARKMODE);

  // ---- Basic display rendering ----------------
  display.fillScreen (DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
  display.fillRoundRect (0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, 8,
                         DARKMODE ? GxEPD_BLACK : GxEPD_WHITE);
  display.setTextColor (DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);

  // ---- Rendering parts ----------------
  drawTime ();
  drawInfo ();
  drawBattery ();
}

/**
 @brief Rendering time
 */
void WatchyBell::drawTime ()
{
  display.setFont (&Oswald_Regular46pt7b);
  String time;
  int16_t x1, y1;
  uint16_t w, h;

  // ---- print hours ----------------
  if (currentTime.Hour < 10)
    {
      time = "0";
    }
  else
    {
      time = "";
    }

  time += currentTime.Hour;

  display.getTextBounds (time, 0, 0, &x1, &y1, &w, &h);
  display.setCursor (DISPLAY_WIDTH - SPACING - w, SPACING + h);
  display.print (time);

  // ---- print minutes ----------------
  if (currentTime.Minute < 10)
    {
      time = "0";
    }
  else
    {
      time = "";
    }

  time += currentTime.Minute;

  display.getTextBounds (time, 0, 0, &x1, &y1, &w, &h);
  display.setCursor (DISPLAY_WIDTH - SPACING - w, DISPLAY_HEIGHT - SPACING);
  display.print (time);
}

/**
 @brief Rendering steps and date
 */
void WatchyBell::drawInfo ()
{
  display.setFont (&Oswald_Regular9pt7b);

  String text;
  int16_t x1, y1, current_y = DISPLAY_HEIGHT - SPACING;
  uint16_t w, h;

  // ---- print steps ----------------
  text = sensor.getCounter ();
  text += " steps";

  display.getTextBounds (text, 0, 0, &x1, &y1, &w, &h);

  display.fillRoundRect (SPACING, current_y - h - 2, w + 8, h + 4, 2,
                         DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);

  display.setTextColor (DARKMODE ? GxEPD_BLACK : GxEPD_WHITE);

  display.setCursor (SPACING + 3, current_y - 2);
  display.print (text);

  current_y -= (h + 8);
  display.setTextColor (DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);

  // ---- print year ----------------
  text = (currentTime.Year + YEAR_OFFSET);

  display.getTextBounds (text, 0, 0, &x1, &y1, &w, &h);

  display.setCursor (SPACING, current_y);
  display.print (text);

  current_y -= (h + 2);

  // ---- print date ----------------
  text = monthShortStr (currentTime.Month);

  text += " ";

  text += currentTime.Day;

  display.getTextBounds (text, 0, 0, &x1, &y1, &w, &h);

  display.setCursor (SPACING, current_y);
  display.print (text);

  current_y -= (h + 2);

  // ---- print day ----------------
  text = dayStr (currentTime.Wday);

  display.getTextBounds (text, 0, 0, &x1, &y1, &w, &h);

  display.setCursor (SPACING, current_y);
  display.print (text);
}

/**
 @brief Rendering battery icon
 */
void WatchyBell::drawBattery ()
{
  // ---- print battery outline ----------------
  display.fillRoundRect (SPACING, SPACING, BATTERY_WIDTH, BATTERY_HEIGHT,
                         BATTERY_SPACING,
                         DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
  display.fillRoundRect (SPACING + BATTERY_SPACING, SPACING + BATTERY_SPACING,
                         BATTERY_WIDTH - (2 * BATTERY_SPACING),
                         BATTERY_HEIGHT - (2 * BATTERY_SPACING),
                         BATTERY_SPACING / 2,
                         DARKMODE ? GxEPD_BLACK : GxEPD_WHITE);

  // ---- calculate battery charge ----------------
  float BatteryVoltage = getBatteryVoltage ();

  if (BatteryVoltage > BatteryMax)
    {
      BatteryMax = BatteryVoltage;
    }

  if (BatteryVoltage < BatteryMin)
    {
      BatteryMin = BatteryVoltage;
    }

  float BatterySpan = BatteryMax - BatteryMin;

  float BatteryCharge = (BatteryVoltage - BatteryMin) / BatterySpan;

  // ---- print battery inside ----------------
  display.fillRoundRect (
      SPACING + (2 * BATTERY_SPACING), SPACING + (2 * BATTERY_SPACING),
      BatteryCharge * (BATTERY_WIDTH - (4 * BATTERY_SPACING)),
      BATTERY_HEIGHT - (4 * BATTERY_SPACING), BATTERY_SPACING / 4,
      DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
}

/**
 @brief Doing maintenance at noon and midnight
 */
void WatchyBell::maintenance ()
{
  if (currentTime.Hour == 0 && currentTime.Minute == 0)
    {
      sensor.resetStepCounter ();
    }

  if ((currentTime.Hour % 12) == 0 && currentTime.Minute == 0)
    {
      syncNtpTime ();
    }
}
