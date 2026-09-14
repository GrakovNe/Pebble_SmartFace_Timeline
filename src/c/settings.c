// SPDX-License-Identifier: Apache-2.0
#include "settings.h"

#include "app_message_keys.h"

// Persistent-storage keys. The numeric values are inherited from the 2014
// release so that existing users keep their configuration after the update;
// do not renumber them.
enum {
  PERSIST_KEY_LANGUAGE = 0,
  PERSIST_KEY_WINDOW_COLOR = 1,
  PERSIST_KEY_TIME_TEXT_SIZE = 2,
  PERSIST_KEY_DATE_FORMAT = 3,
  PERSIST_KEY_SHOW_BATTERY_TEXT = 4,
  PERSIST_KEY_SHOW_BLUETOOTH_TEXT = 5,
  PERSIST_KEY_VIBE_HOURLY = 6,
  PERSIST_KEY_VIBE_BLUETOOTH = 7,
  PERSIST_KEY_NIGHT_MODE_ENABLED = 8,
  PERSIST_KEY_NIGHT_MODE_START = 9,
  PERSIST_KEY_NIGHT_MODE_END = 10,
  PERSIST_KEY_TOP_STRING = 13,
  PERSIST_KEY_BOTTOM_STRING = 14,
  PERSIST_KEY_NIGHT_MODE_INVERT = 15,
  PERSIST_KEY_NIGHT_MODE_UPDATE_INFO = 16,
  PERSIST_KEY_NIGHT_MODE_VIBE_EVENT = 17,
  PERSIST_KEY_DATA_UPDATE_FREQ = 18,
  PERSIST_KEY_NIGHT_MODE_VIBE_HOURLY = 19,
  PERSIST_KEY_DATE_STYLE = 20,
  PERSIST_KEY_SHOW_LAST_DISCONNECT = 21,
  PERSIST_KEY_BLINK_COLON = 22,
  PERSIST_KEY_NIGHT_MODE_BLINK_COLON = 23,
};

static Settings s_settings;

static int32_t persist_read_int_or(int32_t key, int32_t fallback) {
  return persist_exists(key) ? persist_read_int(key) : fallback;
}

static bool persist_read_bool_or(int32_t key, bool fallback) {
  return persist_exists(key) ? persist_read_int(key) != 0 : fallback;
}

void settings_load(void) {
  s_settings = (Settings){
    .language = language_clamp(persist_read_int_or(PERSIST_KEY_LANGUAGE, LanguageEnglish)),
    .inverted_theme = persist_read_bool_or(PERSIST_KEY_WINDOW_COLOR, false),
    .big_time_text = persist_read_bool_or(PERSIST_KEY_TIME_TEXT_SIZE, false),
    .date_format = (int)persist_read_int_or(PERSIST_KEY_DATE_FORMAT, DateFormatDayMonth),
    .date_style = (int)persist_read_int_or(PERSIST_KEY_DATE_STYLE, DateStyleWeekdayBelow),
    .show_battery_text = persist_read_bool_or(PERSIST_KEY_SHOW_BATTERY_TEXT, true),
    .show_bluetooth_text = persist_read_bool_or(PERSIST_KEY_SHOW_BLUETOOTH_TEXT, true),
    .vibe_hourly = persist_read_bool_or(PERSIST_KEY_VIBE_HOURLY, true),
    .vibe_bluetooth = persist_read_bool_or(PERSIST_KEY_VIBE_BLUETOOTH, true),
    .night_mode_enabled = persist_read_bool_or(PERSIST_KEY_NIGHT_MODE_ENABLED, true),
    .night_start_min = (int)persist_read_int_or(PERSIST_KEY_NIGHT_MODE_START, 22 * 60),
    .night_end_min = (int)persist_read_int_or(PERSIST_KEY_NIGHT_MODE_END, 7 * 60),
    .night_invert = persist_read_bool_or(PERSIST_KEY_NIGHT_MODE_INVERT, true),
    .night_update_info = persist_read_bool_or(PERSIST_KEY_NIGHT_MODE_UPDATE_INFO, true),
    .night_vibe_event = persist_read_bool_or(PERSIST_KEY_NIGHT_MODE_VIBE_EVENT, true),
    .night_vibe_hourly = persist_read_bool_or(PERSIST_KEY_NIGHT_MODE_VIBE_HOURLY, false),
    .night_blink_colon = persist_read_bool_or(PERSIST_KEY_NIGHT_MODE_BLINK_COLON, false),
    .data_update_freq = (int)persist_read_int_or(PERSIST_KEY_DATA_UPDATE_FREQ, 3),
    .show_last_disconnect = persist_read_bool_or(PERSIST_KEY_SHOW_LAST_DISCONNECT, false),
    .blink_colon = persist_read_bool_or(PERSIST_KEY_BLINK_COLON, false),
  };

  if (!persist_exists(PERSIST_KEY_TOP_STRING)) {
    persist_write_string(PERSIST_KEY_TOP_STRING, "SmartFace");
  }
  if (!persist_exists(PERSIST_KEY_BOTTOM_STRING)) {
    persist_write_string(PERSIST_KEY_BOTTOM_STRING, "GrakovNe");
  }
  persist_read_string(PERSIST_KEY_TOP_STRING, s_settings.top_string,
                      sizeof(s_settings.top_string));
  persist_read_string(PERSIST_KEY_BOTTOM_STRING, s_settings.bottom_string,
                      sizeof(s_settings.bottom_string));
}

const Settings *settings_get(void) {
  return &s_settings;
}

static int32_t tuple_int(const Tuple *tuple, int32_t fallback) {
  return (tuple != NULL && tuple->type == TUPLE_INT) ? tuple->value->int32 : fallback;
}

// Reads one integer tuple, returns false when it is absent. When present,
// stores it into persistent storage and into *out.
static bool take_int(DictionaryIterator *iterator, uint32_t msg_key, int32_t persist_key,
                     int32_t *out) {
  const Tuple *tuple = dict_find(iterator, msg_key);
  if (tuple == NULL) {
    return false;
  }
  const int32_t value = tuple_int(tuple, *out);
  persist_write_int(persist_key, value);
  *out = value;
  return true;
}

static bool take_bool(DictionaryIterator *iterator, uint32_t msg_key, int32_t persist_key,
                      bool *out) {
  int32_t as_int = *out ? 1 : 0;
  if (!take_int(iterator, msg_key, persist_key, &as_int)) {
    return false;
  }
  *out = as_int != 0;
  return true;
}

static void take_string(DictionaryIterator *iterator, uint32_t msg_key, int32_t persist_key,
                        char *out, size_t out_size) {
  const Tuple *tuple = dict_find(iterator, msg_key);
  if (tuple == NULL || tuple->type != TUPLE_CSTRING) {
    return;
  }
  snprintf(out, out_size, "%s", tuple->value->cstring);
  persist_write_string(persist_key, out);
}

void settings_apply_inbox(DictionaryIterator *iterator, void *context) {
  (void)context;
  Settings *s = &s_settings;

  int32_t language = (int32_t)s->language;
  take_int(iterator, KEY_LANGUAGE, PERSIST_KEY_LANGUAGE, &language);
  s->language = language_clamp(language);

  take_bool(iterator, KEY_WINDOW_COLOR, PERSIST_KEY_WINDOW_COLOR, &s->inverted_theme);
  take_bool(iterator, KEY_TIME_TEXT_SIZE, PERSIST_KEY_TIME_TEXT_SIZE, &s->big_time_text);
  take_int(iterator, KEY_DATE_FORMAT, PERSIST_KEY_DATE_FORMAT, &s->date_format);
  take_int(iterator, KEY_DATE_STYLE, PERSIST_KEY_DATE_STYLE, &s->date_style);
  take_bool(iterator, KEY_SHOW_BATTERY_TEXT, PERSIST_KEY_SHOW_BATTERY_TEXT,
            &s->show_battery_text);
  take_bool(iterator, KEY_SHOW_BLUETOOTH_TEXT, PERSIST_KEY_SHOW_BLUETOOTH_TEXT,
            &s->show_bluetooth_text);
  take_bool(iterator, KEY_VIBE_HOURLY, PERSIST_KEY_VIBE_HOURLY, &s->vibe_hourly);
  take_bool(iterator, KEY_VIBE_BLUETOOTH, PERSIST_KEY_VIBE_BLUETOOTH, &s->vibe_bluetooth);
  take_bool(iterator, KEY_NIGHT_MODE_ENABLED, PERSIST_KEY_NIGHT_MODE_ENABLED,
            &s->night_mode_enabled);
  take_int(iterator, KEY_NIGHT_MODE_START, PERSIST_KEY_NIGHT_MODE_START,
           &s->night_start_min);
  take_int(iterator, KEY_NIGHT_MODE_END, PERSIST_KEY_NIGHT_MODE_END,
           &s->night_end_min);
  take_bool(iterator, KEY_NIGHT_MODE_INVERT, PERSIST_KEY_NIGHT_MODE_INVERT, &s->night_invert);
  take_bool(iterator, KEY_NIGHT_MODE_UPDATE_INFO, PERSIST_KEY_NIGHT_MODE_UPDATE_INFO,
            &s->night_update_info);
  take_bool(iterator, KEY_NIGHT_MODE_VIBE_EVENT, PERSIST_KEY_NIGHT_MODE_VIBE_EVENT,
            &s->night_vibe_event);
  take_int(iterator, KEY_DATA_UPDATE_FREQ, PERSIST_KEY_DATA_UPDATE_FREQ,
           &s->data_update_freq);
  take_bool(iterator, KEY_NIGHT_MODE_VIBE_HOURLY, PERSIST_KEY_NIGHT_MODE_VIBE_HOURLY,
            &s->night_vibe_hourly);
  take_bool(iterator, KEY_SHOW_LAST_DISCONNECT, PERSIST_KEY_SHOW_LAST_DISCONNECT,
            &s->show_last_disconnect);
  take_bool(iterator, KEY_BLINK_COLON, PERSIST_KEY_BLINK_COLON, &s->blink_colon);
  take_bool(iterator, KEY_NIGHT_MODE_BLINK_COLON, PERSIST_KEY_NIGHT_MODE_BLINK_COLON,
            &s->night_blink_colon);

  take_string(iterator, KEY_TOP_STRING, PERSIST_KEY_TOP_STRING, s->top_string,
              sizeof(s->top_string));
  take_string(iterator, KEY_BOTTOM_STRING, PERSIST_KEY_BOTTOM_STRING, s->bottom_string,
              sizeof(s->bottom_string));

  // Defensive clamping: never trust a malformed dictionary.
  if (s->date_format >= DateFormatCount) {
    s->date_format = DateFormatDayMonth;
  }
  if (s->date_style >= DateStyleCount) {
    s->date_style = DateStyleWeekdayBelow;
  }
  if (s->night_start_min >= 24 * 60) {
    s->night_start_min = 22 * 60;
  }
  if (s->night_end_min >= 24 * 60) {
    s->night_end_min = 7 * 60;
  }
  if (s->data_update_freq != DATA_UPDATE_FREQ_NEVER &&
      (s->data_update_freq < 1 || s->data_update_freq > 29)) {
    s->data_update_freq = 3;
  }
}
