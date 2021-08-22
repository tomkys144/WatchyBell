/**
 @file WatchyBase.cpp
 @date 11/08/2021
 @author Tomáš Kysela (tomkys144@gmail.com)
 @copyright (c) 2021 Tomáš Kysela
*/

#include "WatchyBase.hpp"

RTC_DATA_ATTR int16_t alarm_timer = -1;

// ---- class WatchyBase ----------------

// ---- public --------------------------

/**
 @brief Construct a new Watchy Base:: Watchy Base object
 */
WatchyBase::WatchyBase () {}

/**
 @brief Init class on wakeup
 */
void WatchyBase::init ()
{
  wakeup_reason = esp_sleep_get_wakeup_cause ();
  Wire.begin (SDA, SCL);

  switch (wakeup_reason)
    {
    // RTC Alarm
    case ESP_SLEEP_WAKEUP_EXT0:
      RTC.alarm (ALARM_2); // reset the alarm flag in RTC
      if (guiState == WATCHFACE_STATE)
        {
          RTC.read (currentTime);
          showWatchFace (true); // partial update on tick
        }
      break;

    // Button Press
    case ESP_SLEEP_WAKEUP_EXT1:
      handleButtonPress ();
      break;

    // Reset
    default:
      _rtcConfig ();
      _bmaConfig ();
      showWatchFace (false); // full update on reset
      break;
    }

  deepSleep ();
}

/**
 @brief Handling other button press than default Watchy functions
 */
void WatchyBase::handleButtonPress ()
{
  uint64_t wakeupBit = esp_sleep_get_ext1_wakeup_status ();

  if (wakeupBit & BACK_BTN_MASK && guiState == WATCHFACE_STATE)
    {
      setAlarm ();
    }

  Watchy::handleButtonPress ();
}

/**
 @brief Enable interrupts and put watch to sleep
 */
void WatchyBase::deepSleep ()
{
  // enable deepsleep wake on RTC interrupt
  esp_sleep_enable_ext0_wakeup (RTC_PIN, 0);
  // enable deepsleep wake on button press
  esp_sleep_enable_ext1_wakeup (BTN_PIN_MASK, ESP_EXT1_WAKEUP_ANY_HIGH);

  esp_deep_sleep_start ();
}

/**
 @brief NTP time sync
 */
void WatchyBase::syncNtpTime ()
{
  if (connectWiFi ())
    {
      configTzTime (NTP_TZ, NTP_SERVER1, NTP_SERVER2, NTP_SERVER3);

      bool sync_fail = false;
      int sntp_time = 0;
      while (sntp_get_sync_status () != SNTP_SYNC_STATUS_COMPLETED
             && sntp_time < 10)
        {
          ++sntp_time;

          delay (1000);

          if (sntp_time == 10)
            {
              sync_fail = true;
            }
        }

      if (sync_fail == false)
        {
          time_t syncTime = time (nullptr);
          struct tm* localTime = localtime (&syncTime);

          currentTime.Year = localTime->tm_year + YEAR_OFFSET - 2040;
          currentTime.Month = localTime->tm_mon + 1;
          currentTime.Day = localTime->tm_mday;
          currentTime.Wday = localTime->tm_wday + 1;
          currentTime.Hour = localTime->tm_hour;
          currentTime.Minute = localTime->tm_min;
          currentTime.Second = localTime->tm_sec;

          RTC.write (currentTime);
          RTC.read (currentTime);
        }
    }
}

/**
 @brief connect to configured WiFi

 @return true
 @return false
 */
bool WatchyBase::connectWiFi ()
{
  if (WL_CONNECT_FAILED == WiFi.begin ())
    {
      WIFI_CONFIGURED = false;
    }
  else
    {
      if (WL_CONNECTED == WiFi.waitForConnectResult ())
        {
          WIFI_CONFIGURED = true;
        }
      else
        {
          WIFI_CONFIGURED = false;

          WiFi.mode (WIFI_OFF);
          btStop ();
        }
    }
  return WIFI_CONFIGURED;
}

/**
 @brief disable WiFi
 */
void WatchyBase::disableWiFI () { WiFi.mode (WIFI_OFF); }

/**
 @brief Setup alarm. If already set, gives option to edit or stop.
 */
void WatchyBase::setAlarm ()
{
  guiState = APP_STATE;
  display.init (0, true);
  display.setFullWindow ();

  uint16_t w, h;
  int16_t x1, y1;
  String text;

  if (alarm_timer < 0)
    {
      uint8_t alarm_hour = currentTime.Hour;
      uint8_t alarm_min = currentTime.Minute;

      alarmConfig (&alarm_hour, &alarm_min, &alarm_timer);
    }
  else
    {
      uint8_t optionIndex = OPTION_EDIT;

      pinMode (DOWN_BTN_PIN, INPUT);
      pinMode (UP_BTN_PIN, INPUT);
      pinMode (MENU_BTN_PIN, INPUT);
      pinMode (BACK_BTN_PIN, INPUT);

      while (true)
        {
          if (digitalRead (MENU_BTN_PIN) == 1)
            {
              if (optionIndex == OPTION_EDIT)
                {
                  uint8_t alarm_hour = 0;
                  uint8_t alarm_min = 0;

                  timerConvert (&alarm_hour, &alarm_min, &alarm_timer, true);

                  alarmConfig (&alarm_hour, &alarm_min, &alarm_timer);
                }
              else if (optionIndex == OPTION_STOP)
                {
                  alarm_timer = -1;
                }

              break;
            }

          if (digitalRead (BACK_BTN_PIN) == 1)
            {
              break;
            }

          if (digitalRead (DOWN_BTN_PIN) == 1)
            {
              optionIndex == OPTION_STOP ? (optionIndex = OPTION_EDIT)
                                         : optionIndex++;
            }

          if (digitalRead (UP_BTN_PIN) == 1)
            {
              optionIndex == OPTION_EDIT ? (optionIndex = OPTION_STOP)
                                         : optionIndex--;
            }

          display.fillScreen (GxEPD_BLACK);
          display.setFont (&Oswald_Regular9pt7b);

          int16_t y_pos = 30;

          text = "Edit alarm";
          display.getTextBounds (text, 0, y_pos, &x1, &y1, &w, &h);
          display.fillRect (x1 - 1, y1 - 10, 200, h + 15,
                            optionIndex == OPTION_EDIT ? GxEPD_WHITE
                                                       : GxEPD_BLACK);
          display.setTextColor (optionIndex == OPTION_EDIT ? GxEPD_BLACK
                                                           : GxEPD_WHITE);
          display.println (text);

          y_pos += (h + 30);

          text = "Stop alarm";
          display.getTextBounds (text, 0, y_pos, &x1, &y1, &w, &h);
          display.fillRect (x1 - 1, y1 - 10, 200, h + 15,
                            optionIndex == OPTION_STOP ? GxEPD_WHITE
                                                       : GxEPD_BLACK);
          display.setTextColor (optionIndex == OPTION_STOP ? GxEPD_BLACK
                                                           : GxEPD_WHITE);
          display.println (text);

          display.display (true);
        }
    }

  showMenu (menuIndex, false);
}

/**
 @brief Checks if alarm should be triggered and if so, triggers alarm.
 */
void WatchyBase::triggerAlarm (bool darkmode)
{
  if (alarm_timer == 0)
    {
      pinMode (DOWN_BTN_PIN, INPUT);
      pinMode (UP_BTN_PIN, INPUT);
      pinMode (MENU_BTN_PIN, INPUT);
      pinMode (BACK_BTN_PIN, INPUT);
      pinMode (VIB_MOTOR_PIN, OUTPUT);

      bool motorOn = false;

      display.init (0, true);
      display.setFullWindow ();
      display.fillScreen (darkmode ? GxEPD_BLACK : GxEPD_WHITE);
      display.setTextColor (darkmode ? GxEPD_WHITE : GxEPD_BLACK);
      display.setFont (&Oswald_Regular46pt7b);

      uint16_t w, h;
      int16_t x1, y1;
      String text = "ALARM";

      display.getTextBounds (text, 0, 0, &x1, &y1, &w, &h);

      display.setCursor ((DISPLAY_WIDTH - w) / 2, (DISPLAY_HEIGHT - h) / 2);

      display.print (text);

      while (true)
        {
          if ((digitalRead (DOWN_BTN_PIN) == 1)
              || (digitalRead (UP_BTN_PIN) == 1)
              || (digitalRead (MENU_BTN_PIN) == 1)
              || (digitalRead (BACK_BTN_PIN) == 1))
            {
              break;
            }

          motorOn = !motorOn;
          digitalWrite (VIB_MOTOR_PIN, motorOn);
          delay (ALARM_INTERVAL);
        }

      if (alarm_timer >= 0)
        {
          alarm_timer--;
        }
    }
}

/**
 @brief sets alarm time

 @param hour,minute,timer Timer and time vars pointers
 */
void WatchyBase::alarmConfig (uint8_t* hour, uint8_t* minute, int16_t* timer)
{
  display.init (0, true);
  display.setFullWindow ();

  int8_t setIndex = SET_HOUR;
  pinMode (DOWN_BTN_PIN, INPUT);
  pinMode (UP_BTN_PIN, INPUT);
  pinMode (MENU_BTN_PIN, INPUT);
  pinMode (BACK_BTN_PIN, INPUT);

  int8_t blink = 0;
  bool alarm_set = false;

  uint16_t text_width, text_height;
  int16_t x1, y1;
  String text;

  while (true)
    {
      if (digitalRead (MENU_BTN_PIN) == 1)
        {
          setIndex++;

          if (setIndex > SET_MINUTE)
            {
              alarm_set = true;
              break;
            }
        }

      if (digitalRead (BACK_BTN_PIN) == 1)
        {
          if (setIndex != SET_HOUR)
            {
              setIndex--;
            }
          else
            {
              break;
            }
        }

      blink = 1 - blink;

      if (digitalRead (DOWN_BTN_PIN) == 1)
        {
          blink = 1;
          switch (setIndex)
            {
            case SET_HOUR:
              *hour == 23 ? (*hour = 0) : *hour++;
              break;
            case SET_MINUTE:
              *minute == 59 ? (*minute = 0) : *minute++;
              break;
            default:
              break;
            }
        }

      if (digitalRead (UP_BTN_PIN) == 1)
        {
          blink = 1;
          switch (setIndex)
            {
            case SET_HOUR:
              *hour == 0 ? (*hour = 23) : *hour--;
              break;
            case SET_MINUTE:
              *minute == 0 ? (*minute = 59) : *minute--;
              break;
            default:
              break;
            }
        }

      display.fillScreen (GxEPD_BLACK);
      display.setTextColor (GxEPD_WHITE);
      display.setFont (&Oswald_Regular46pt7b);

      if (*hour < 10)
        {
          text = "0";
          text += *hour;
        }
      else
        {
          text = *hour;
        }

      text += ":";

      if (*minute < 10)
        {
          text += "0";
        }

      text += *minute;

      display.getTextBounds (text, 0, 0, &x1, &y1, &text_width, &text_height);

      display.setCursor ((DISPLAY_WIDTH - text_width) / 2,
                         text_height + ((DISPLAY_HEIGHT - text_height) / 2));

      if (setIndex == SET_HOUR)
        {
          display.setTextColor (blink ? GxEPD_WHITE : GxEPD_BLACK);
        }
      if (*hour < 10)
        {
          display.print ("0");
        }
      display.print (*hour);

      display.setTextColor (GxEPD_WHITE);
      display.print (":");

      if (setIndex == SET_MINUTE)
        {
          display.setTextColor (blink ? GxEPD_WHITE : GxEPD_BLACK);
        }
      if (*minute < 10)
        {
          display.print ("0");
        }
      display.print (*minute);

      display.display (true);
    }

  if (alarm_set)
    {
      uint8_t cur_hour = currentTime.Hour;
      uint8_t cur_min = currentTime.Minute;
      uint8_t hour_diff, minute_diff;

      // if hour is not the same
      if (*hour > cur_hour)
        {
          hour_diff = *hour - cur_hour;
        }
      else if (*hour <= cur_hour)
        {
          hour_diff = (24 - cur_hour) + *hour;
        }

      if (*minute > cur_min)
        {
          minute_diff = *minute - cur_min;
        }
      else if (*minute < cur_min)
        {
          minute_diff = (60 - cur_min) + *minute;
        }

      // if hour is the same
      if ((*hour == cur_hour) && (*minute <= cur_min))
        {
          hour_diff = 23;
          minute_diff = (60 - cur_min) + *minute;
        }
      else if ((*hour == cur_hour) && (*minute >= cur_min))
        {
          hour_diff = 0;
          minute_diff = *minute - cur_min;
        }

      timerConvert (&hour_diff, &minute_diff, timer, false);
    }
}

/**
 @brief converts Timer to Hours and minutes and back

 @param hours,minutes,timer Pointers to time variables
 @param direction
 @parblock
 false: Hours+Mins -> Timer

 true: Timer -> Hours+Mins

 Default: false
 @endparblock
 */
void WatchyBase::timerConvert (uint8_t* hours, uint8_t* minutes,
                               int16_t* timer, bool direction)
{
  if (direction)
    {
      *hours = (*timer / 60);
      *minutes = (*timer % 60);
    }
  else
    {
      *timer = *minutes;
      *timer += (*hours * 60);
    }
}

/**
 @brief Configuration of RTC
 */
void WatchyBase::_rtcConfig ()
{
  /// https://github.com/JChristensen/DS3232RTC
  RTC.squareWave (SQWAVE_NONE); // disable square wave output
  // RTC.set(compileTime()); // set RTC time to compile time
  RTC.setAlarm (ALM2_EVERY_MINUTE, 0, 0, 0,
                0);                   // alarm wakes up Watchy every minute
  RTC.alarmInterrupt (ALARM_2, true); // enable alarm interrupt
  RTC.read (currentTime);
}

/**
 @brief Configuration of BMA
 */
void WatchyBase::_bmaConfig ()
{
  if (sensor.begin (_readRegister, _writeRegister, delay) == false)
    {
      // fail to init BMA
      return;
    }

  // Accel parameter structure
  Acfg cfg;

  /**
   @brief Output data rate in Hz
   @details Optional parameters:
      - BMA4_OUTPUT_DATA_RATE_0_78HZ
      - BMA4_OUTPUT_DATA_RATE_1_56HZ
      - BMA4_OUTPUT_DATA_RATE_3_12HZ
      - BMA4_OUTPUT_DATA_RATE_6_25HZ
      - BMA4_OUTPUT_DATA_RATE_12_5HZ
      - BMA4_OUTPUT_DATA_RATE_25HZ
      - BMA4_OUTPUT_DATA_RATE_50HZ
      - BMA4_OUTPUT_DATA_RATE_100HZ
      - BMA4_OUTPUT_DATA_RATE_200HZ
      - BMA4_OUTPUT_DATA_RATE_400HZ
      - BMA4_OUTPUT_DATA_RATE_800HZ
      - BMA4_OUTPUT_DATA_RATE_1600HZ
   */
  cfg.odr = BMA4_OUTPUT_DATA_RATE_100HZ;

  /**
   @brief G-range
   @details Optional parameters
      - BMA4_ACCEL_RANGE_2G
      - BMA4_ACCEL_RANGE_4G
      - BMA4_ACCEL_RANGE_8G
      - BMA4_ACCEL_RANGE_16G
   */
  cfg.range = BMA4_ACCEL_RANGE_2G;

  /**
   @brief Bandwidth parameter, determines filter configuration
   @details Optional parameters:
      - BMA4_ACCEL_OSR4_AVG1
      - BMA4_ACCEL_OSR2_AVG2
      - BMA4_ACCEL_NORMAL_AVG4
      - BMA4_ACCEL_CIC_AVG8
      - BMA4_ACCEL_RES_AVG16
      - BMA4_ACCEL_RES_AVG32
      - BMA4_ACCEL_RES_AVG64
      - BMA4_ACCEL_RES_AVG128
   */
  cfg.bandwidth = BMA4_ACCEL_NORMAL_AVG4;

  /**
   @brief Filter performance mode
   @details Optional parameters:
      - BMA4_CIC_AVG_MODE
      - BMA4_CONTINUOUS_MODE
   */
  cfg.perf_mode = BMA4_CONTINUOUS_MODE;

  /// @brief Configure the BMA423 accelerometer
  sensor.setAccelConfig (cfg);

  /**
   @brief Enable BMA423 accelerometer
   @warning Need to use feature, you must first enable the accelerometer
   */
  sensor.enableAccel ();

  /**
   @brief Configure BMA423 pinout config
   @warning The correct trigger interrupt needs to be configured as needed
   */
  struct bma4_int_pin_config config;
  config.edge_ctrl = BMA4_LEVEL_TRIGGER;
  config.lvl = BMA4_ACTIVE_HIGH;
  config.od = BMA4_PUSH_PULL;
  config.output_en = BMA4_OUTPUT_ENABLE;
  config.input_en = BMA4_INPUT_DISABLE;

  sensor.setINTPinConfig (config, BMA4_INTR1_MAP);

  /**
   @brief Remap BMA423 axis
   @warning Need to raise the wrist function
   @warning Need to set the correct axis
   */
  struct bma423_axes_remap remap_data;
  remap_data.x_axis = 1;
  remap_data.x_axis_sign = 0xFF;
  remap_data.y_axis = 0;
  remap_data.y_axis_sign = 0xFF;
  remap_data.z_axis = 2;
  remap_data.z_axis_sign = 0xFF;

  sensor.setRemapAxes (&remap_data);

  /// @brief Enable BMA423 isStepCounter feature
  sensor.enableFeature (BMA423_STEP_CNTR, true);

  /// @brief Enable BMA423 isTilt feature
  sensor.enableFeature (BMA423_TILT, true);

  /// @brief Enable BMA423 isDoubleClick feature
  sensor.enableFeature (BMA423_WAKEUP, true);

  /// @brief Reset steps
  sensor.resetStepCounter ();

  /// @brief Turn on feature interrupt
  sensor.enableStepCountInterrupt ();
  sensor.enableTiltInterrupt ();
  sensor.enableWakeupInterrupt ();
}

/**
 * @brief Read from register
 *
 * @param address
 * @param reg
 * @param data
 * @param len
 * @return uint16_t
 */
uint16_t WatchyBase::_readRegister (uint8_t address, uint8_t reg,
                                    uint8_t* data, uint16_t len)
{
  Wire.beginTransmission (address);
  Wire.write (reg);
  Wire.endTransmission ();
  Wire.requestFrom ((uint8_t)address, (uint8_t)len);
  uint8_t i = 0;
  while (Wire.available ())
    {
      data[++i] = Wire.read ();
    }
  return 0;
}

/**
 * @brief Write to register
 *
 * @param address
 * @param reg
 * @param data
 * @param len
 * @return uint16_t
 */
uint16_t WatchyBase::_writeRegister (uint8_t address, uint8_t reg,
                                     uint8_t* data, uint16_t len)
{
  Wire.beginTransmission (address);
  Wire.write (reg);
  Wire.write (data, len);
  return (0 != Wire.endTransmission ());
}
