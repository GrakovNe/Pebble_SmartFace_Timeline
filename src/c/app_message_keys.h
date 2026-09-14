// SPDX-License-Identifier: Apache-2.0
//
// AppMessage keys shared between the watchapp (C) and PebbleKit JS.
// Values must stay in sync with `pebble.messageKeys` in package.json.
#pragma once

enum {
  KEY_LANGUAGE = 0,
  KEY_WINDOW_COLOR = 1,
  KEY_TIME_TEXT_SIZE = 2,
  KEY_DATE_FORMAT = 3,
  KEY_SHOW_BATTERY_TEXT = 4,
  KEY_SHOW_BLUETOOTH_TEXT = 5,
  KEY_VIBE_HOURLY = 6,
  KEY_VIBE_BLUETOOTH = 7,
  KEY_NIGHT_MODE_ENABLED = 8,
  KEY_NIGHT_MODE_START = 9,
  KEY_NIGHT_MODE_END = 10,
  KEY_TOP_STRING = 11,
  KEY_BOTTOM_STRING = 12,
  KEY_NIGHT_MODE_INVERT = 13,
  KEY_NIGHT_MODE_UPDATE_INFO = 14,
  KEY_NIGHT_MODE_VIBE_EVENT = 15,
  KEY_DATA_UPDATE_FREQ = 16,
  KEY_NIGHT_MODE_VIBE_HOURLY = 17,
  KEY_DATE_STYLE = 18,
  KEY_SHOW_LAST_DISCONNECT = 19,
  KEY_BLINK_COLON = 20,
  KEY_NIGHT_MODE_BLINK_COLON = 21,
  // Watchapp -> phone: request fresh additional-info strings.
  KEY_ASK_DATA = 99,
};
