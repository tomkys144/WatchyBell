/**
 @file WatchyBase.hpp
 @date 11/08/2021
 @author Tomáš Kysela (tomkys144@gmail.com)
 @copyright (c) 2021 Tomáš Kysela

 @attention Based on SQFMI's Watchy (https://github.com/sqfmi/Watchy)
 @attention Inspired by pxl999 (https://github.com/dezign999/pxl999)
*/

#ifndef WATCHYBASE_HPP
#define WATCHYBASE_HPP

#include <Watchy.h>
#include <time.h>

#include "fonts/Oswald_Regular46pt7b.h"
#include "fonts/Oswald_Regular9pt7b.h"

// ---- Time ----------------
#define NTP_TZ                                                                \
  "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00" ///< Timezone string
#define NTP_SERVER1 "pool.ntp.org"               ///< NTP server 1
#define NTP_SERVER2 nullptr                      ///< NTP server 2
#define NTP_SERVER3 nullptr                      ///< NTP server 3

// ---- Alarm ----------------
extern RTC_DATA_ATTR int16_t alarm_timer;
enum ALARM_OPTIONS
{
  OPTION_EDIT,
  OPTION_STOP
};
#define ALARM_INTERVAL 200

/**
 @class WatchyBase

 @brief Base class for Watchy

 @details Upgraded Watchy class with advance functions
 */
class WatchyBase : public Watchy
{
public:
  WatchyBase ();
  virtual void init ();
  virtual void handleButtonPress ();
  virtual void deepSleep ();
  esp_sleep_wakeup_cause_t wakeup_reason;
  void syncNtpTime ();
  bool connectWiFi ();
  void disableWiFI ();
  void setAlarm ();
  void triggerAlarm (bool darkmode);

private:
  void alarmConfig (uint8_t* hour, uint8_t* minute, int16_t* timer);
  void timerConvert (uint8_t* hours, uint8_t* minutes, int16_t* timer,
                     bool direction = false);
  void _rtcConfig ();
  void _bmaConfig ();
  static uint16_t _readRegister (uint8_t address, uint8_t reg, uint8_t* data,
                                 uint16_t len);
  static uint16_t _writeRegister (uint8_t address, uint8_t reg, uint8_t* data,
                                  uint16_t len);
};

#endif /* WATCHYBASE_HPP */
