// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <pebble.h>

#include "i18n.h"

// Sentinel value for data_update_freq meaning "never poll the phone".
#define DATA_UPDATE_FREQ_NEVER 30

#define ADDITIONAL_STRING_MAX_LEN 29 // 28 visible characters + NUL

typedef struct {
  Language language;
  bool inverted_theme;          // false: black on white, true: white on black
  bool big_time_text;
  int32_t date_format;              // DateFormat
  int32_t date_style;               // DateStyle
  bool show_battery_text;
  bool show_bluetooth_text;
  bool vibe_hourly;
  bool vibe_bluetooth;
  bool night_mode_enabled;
  int32_t night_start_min;          // minutes since midnight
  int32_t night_end_min;            // minutes since midnight
  bool night_invert;
  bool night_update_info;
  bool night_vibe_event;
  bool night_vibe_hourly;
  bool night_blink_colon;
  int32_t data_update_freq;         // requests per hour, or DATA_UPDATE_FREQ_NEVER
  bool show_last_disconnect;
  bool blink_colon;
  char top_string[ADDITIONAL_STRING_MAX_LEN];
  char bottom_string[ADDITIONAL_STRING_MAX_LEN];
} Settings;

enum {
  DateFormatDayMonthYear = 0,   // 31.12.2026
  DateFormatDayMonth = 1,       // 31 DECEMBER
  DateFormatMonthDayYear = 2,   // 12/31/2026
  DateFormatCount,
};

enum {
  DateStyleWeekdayAbove = 0,
  DateStyleWeekdayBelow = 1,
  DateStyleCount,
};

// Loads settings from persistent storage, falling back to defaults.
void settings_load(void);

// The live settings. Valid after settings_load(); updated by settings_apply_inbox().
const Settings *settings_get(void);

// Reads AppMessage tuples, stores them into persistent storage and into the
// live settings. Unknown or missing keys keep their previous value.
void settings_apply_inbox(DictionaryIterator *iterator, void *context);
