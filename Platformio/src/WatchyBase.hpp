/*
 File: WatchyBase.hpp
 Project: WatchyBell
 File Created: 11/08/2021, 19:00:51
 Author: Tomáš Kysela (tomkys144@gmail.com)
 -----
 Last Modified: 11/08/2021, 21:00:23
 Modified By: Tomáš Kysela
 -----
 Copyright (c) 2021 Tomáš Kysela
*/

/*
 Based on SQFMI's Watchy(https://github.com/sqfmi/Watchy)
 Inspired by pxl999 (https://github.com/dezign999/pxl999)
*/

#ifndef WATCHYBASE_HPP
#define WATCHYBASE_HPP

#include <Watchy.h>
#include <time.h>

#include "fonts/Oswald_Regular46pt7b.h"
#include "fonts/Oswald_Regular9pt7b.h"

// ---- Time ----------------
#define NTP_TZ "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00"
#define NTP_SERVER1 "pool.ntp.org"
#define NTP_SERVER2 nullptr
#define NTP_SERVER3 nullptr

/**
 * @class WatchyBase
 * 
 * @brief Base class for Watchy
 * 
 * Upgraded Watchy class with advance functions
 * 
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

private:
  void _rtcConfig ();
  void _bmaConfig ();
  static uint16_t _readRegister (uint8_t address, uint8_t reg, uint8_t* data,
                                 uint16_t len);
  static uint16_t _writeRegister (uint8_t address, uint8_t reg, uint8_t* data,
                                  uint16_t len);
};

#endif /* WATCHYBASE_HPP */
