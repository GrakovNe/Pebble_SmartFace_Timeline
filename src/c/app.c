// SPDX-License-Identifier: Apache-2.0
//
// SmartFace TimeLine — a customizable watchface for Pebble Time 2 (`emery`).
//
// Responsibilities of this module: application lifecycle, ticking, night
// mode, service subscriptions and the AppMessage protocol with the phone.
// All drawing lives in watchface.c, all configuration in settings.c.
#include <pebble.h>

#include "app_message_keys.h"
#include "i18n.h"
#include "settings.h"
#include "watchface.h"

#define MILLISECONDS_PER_HOUR (60 * 60 * 1000)
#define DATA_REQUEST_LATENCY_MS (30 * 1000)
#define FIRST_DATA_REQUEST_DELAY_MS (3 * 1000)

// Weekday names indexed by tm_wday (0 = Sunday).
static const char *const s_weekday_names[LanguageCount][7] = {
  {
    "SUNDAY", "MONDAY", "TUESDAY", "WEDNESDAY", "THURSDAY", "FRIDAY", "SATURDAY",
  },
  {
    "ВОСКРЕСЕНЬЕ", "ПОНЕДЕЛЬНИК", "ВТОРНИК", "СРЕДА", "ЧЕТВЕРГ", "ПЯТНИЦА", "СУББОТА",
  },
  {
    "SONNTAG", "MONTAG", "DIENSTAG", "MITTWOCH", "DONNERSTAG", "FREITAG", "SAMSTAG",
  },
};

// Month names indexed by tm_mon (0 = January), genitive case for "5 MAY" style.
static const char *const s_month_names[LanguageCount][12] = {
  {
    "JANUARY", "FEBRUARY", "MARCH", "APRIL", "MAY", "JUNE",
    "JULY", "AUGUST", "SEPTEMBER", "OCTOBER", "NOVEMBER", "DECEMBER",
  },
  {
    "ЯНВАРЯ", "ФЕВРАЛЯ", "МАРТА", "АПРЕЛЯ", "МАЯ", "ИЮНЯ",
    "ИЮЛЯ", "АВГУСТА", "СЕНТЯБРЯ", "ОКТЯБРЯ", "НОЯБРЯ", "ДЕКАБРЯ",
  },
  {
    "JANUAR", "FEBRUAR", "MÄRZ", "APRIL", "MAI", "JUNI",
    "JULI", "AUGUST", "SEPTEMBER", "OKTOBER", "NOVEMBER", "DEZEMBER",
  },
};

// Bluetooth link state captions, indexed by Language.
static const char *const s_bt_connected_names[LanguageCount] = { "OK", "OK", "OK" };
static const char *const s_bt_disconnected_names[LanguageCount] = { "LOST", "НЕТ", "NEIN" };

static AppTimer *s_data_request_timer;
static AppTimer *s_sync_timeout_timer;

static bool s_bluetooth_connected;
static BatteryChargeState s_battery_state;
static time_t s_last_disconnect_time;

static bool s_night_now;
static bool s_theme_inverted;
static bool s_hourly_vibe_armed = true;

static char s_time_buffer[8];
static char s_date_buffer[32];
static char s_bluetooth_text_buffer[16];

static void reschedule_tick_service(void);
static void on_minute_tick(struct tm *now, TimeUnits units_changed);
static void on_second_tick(struct tm *now, TimeUnits units_changed);

// Returns a copy: the SDK's localtime() uses a shared static buffer, so a
// pointer into it must never outlive the call that produced it.
static struct tm system_time(void) {
  time_t now = time(NULL);
  return *localtime(&now);
}

// ------------------------------------------------------------ night mode --

static bool night_mode_active(const struct tm *now) {
  const Settings *s = settings_get();
  if (!s->night_mode_enabled) {
    return false;
  }
  const int minutes = now->tm_hour * 60 + now->tm_min;
  if (s->night_start_min == s->night_end_min) {
    return false;
  }
  if (s->night_start_min < s->night_end_min) {
    return minutes >= s->night_start_min && minutes < s->night_end_min;
  }
  // Window wraps over midnight, e.g. 22:00 -> 07:00.
  return minutes >= s->night_start_min || minutes < s->night_end_min;
}

static bool desired_theme_inverted(void) {
  const Settings *s = settings_get();
  const bool invert_now = s_night_now && s->night_invert;
  return invert_now ? !s->inverted_theme : s->inverted_theme;
}

// ----------------------------------------------------------------- clock --

static void refresh_time_text(const struct tm *now, bool blinking) {
  const char delimiter = (blinking && (now->tm_sec % 2) != 0) ? ' ' : ':';
  snprintf(s_time_buffer, sizeof(s_time_buffer), "%02d%c%02d", now->tm_hour, delimiter,
           now->tm_min);
  watchface_set_time(s_time_buffer);
}

static void refresh_date(const struct tm *now) {
  const Settings *s = settings_get();

  switch (s->date_format) {
    case DateFormatDayMonthYear:
      snprintf(s_date_buffer, sizeof(s_date_buffer), "%02d.%02d.%04d", now->tm_mday,
               now->tm_mon + 1, now->tm_year + 1900);
      break;
    case DateFormatMonthDayYear:
      snprintf(s_date_buffer, sizeof(s_date_buffer), "%02d/%02d/%04d", now->tm_mon + 1,
               now->tm_mday, now->tm_year + 1900);
      break;
    case DateFormatDayMonth:
    default:
      snprintf(s_date_buffer, sizeof(s_date_buffer), "%02d %s", now->tm_mday,
               s_month_names[s->language][now->tm_mon]);
      break;
  }

  const char *weekday = s_weekday_names[s->language][now->tm_wday];
  if (s->date_style == DateStyleWeekdayAbove) {
    watchface_set_date_lines(weekday, s_date_buffer);
  } else {
    watchface_set_date_lines(s_date_buffer, weekday);
  }
}

static void hourly_vibe(const struct tm *now) {
  const Settings *s = settings_get();
  if (now->tm_min != 0) {
    s_hourly_vibe_armed = true;
    return;
  }
  if (!s_hourly_vibe_armed) {
    return;
  }
  if (s->vibe_hourly && (!s_night_now || s->night_vibe_hourly)) {
    vibes_double_pulse();
  }
  s_hourly_vibe_armed = false;
}

// Re-applies theme and tick cadence when the day/night state flips: night
// mode can invert colors and disable the blinking colon.
static void update_night_state(const struct tm *now) {
  const bool night = night_mode_active(now);
  if (night == s_night_now) {
    return;
  }
  s_night_now = night;

  const bool inverted = desired_theme_inverted();
  if (inverted != s_theme_inverted) {
    s_theme_inverted = inverted;
    watchface_apply_theme(inverted);
  }
  reschedule_tick_service();
}

static void reschedule_tick_service(void) {
  const Settings *s = settings_get();
  const bool blink = s->blink_colon && (!s_night_now || s->night_blink_colon);
  tick_timer_service_unsubscribe();
  if (blink) {
    tick_timer_service_subscribe(SECOND_UNIT, on_second_tick);
  } else {
    tick_timer_service_subscribe(MINUTE_UNIT, on_minute_tick);
  }
}

static void on_minute_tick(struct tm *now, TimeUnits units_changed) {
  (void)units_changed;
  refresh_time_text(now, /* blinking = */ false);
  hourly_vibe(now);
  if (now->tm_hour == 0 && now->tm_min == 0) {
    refresh_date(now);
  }
  update_night_state(now);
}

static void on_second_tick(struct tm *now, TimeUnits units_changed) {
  (void)units_changed;
  refresh_time_text(now, /* blinking = */ true);
  hourly_vibe(now);
  if (now->tm_sec == 0) {
    if (now->tm_hour == 0 && now->tm_min == 0) {
      refresh_date(now);
    }
    update_night_state(now);
  }
}

// -------------------------------------------------------------- bluetooth --

static void refresh_bluetooth_text(void) {
  const Settings *s = settings_get();
  const char *text = "";

  if (s->show_bluetooth_text) {
    if (s_bluetooth_connected) {
      text = s_bt_connected_names[s->language];
    } else if (s->show_last_disconnect && s_last_disconnect_time != 0) {
      struct tm *disconnected_at = localtime(&s_last_disconnect_time);
      snprintf(s_bluetooth_text_buffer, sizeof(s_bluetooth_text_buffer), "%02d:%02d",
               disconnected_at->tm_hour, disconnected_at->tm_min);
      text = s_bluetooth_text_buffer;
    } else {
      text = s_bt_disconnected_names[s->language];
    }
  }
  watchface_set_bluetooth_text(text);
}

static void cancel_sync_timeout(void) {
  app_timer_cancel(s_sync_timeout_timer);
  s_sync_timeout_timer = NULL;
}

static void on_bluetooth_connection(bool connected) {
  s_bluetooth_connected = connected;

  if (!connected) {
    s_last_disconnect_time = time(NULL);
    cancel_sync_timeout();
    watchface_set_sync_indicator(SyncIndicatorHidden);
  }

  const Settings *s = settings_get();
  if (s->vibe_bluetooth && (!s_night_now || s->night_vibe_event)) {
    vibes_long_pulse();
  }

  watchface_set_bluetooth(connected);
  refresh_bluetooth_text();
}

// --------------------------------------------------------- data requests --

static void on_sync_timeout(void *context) {
  (void)context;
  s_sync_timeout_timer = NULL;
  watchface_set_sync_indicator(SyncIndicatorError);
}

static void request_data_from_phone(void *context) {
  (void)context;
  const Settings *s = settings_get();
  if (s->data_update_freq == DATA_UPDATE_FREQ_NEVER || !s_bluetooth_connected) {
    return;
  }
  if (s_night_now && !s->night_update_info) {
    return;
  }

  DictionaryIterator *iter;
  if (app_message_outbox_begin(&iter) != APP_MSG_OK) {
    return;
  }
  dict_write_uint8(iter, KEY_ASK_DATA, 0);
  app_message_outbox_send();

  watchface_set_sync_indicator(SyncIndicatorInProgress);
  cancel_sync_timeout();
  s_sync_timeout_timer = app_timer_register(DATA_REQUEST_LATENCY_MS, on_sync_timeout, NULL);
}

static void schedule_data_request(uint32_t delay_ms) {
  const Settings *s = settings_get();
  app_timer_cancel(s_data_request_timer);
  s_data_request_timer = NULL;
  if (s->data_update_freq == DATA_UPDATE_FREQ_NEVER || s->data_update_freq <= 0) {
    return;
  }
  if (delay_ms == 0) {
    delay_ms = (uint32_t)MILLISECONDS_PER_HOUR / (uint32_t)s->data_update_freq;
  }
  s_data_request_timer = app_timer_register(delay_ms, request_data_from_phone, NULL);
}

// ------------------------------------------------------------ app message --

static void refresh_entire_face(void) {
  const Settings *s = settings_get();
  const struct tm now = system_time();

  watchface_apply_time_text_size(s->big_time_text);
  refresh_time_text(&now, /* blinking = */ false);
  refresh_date(&now);
  refresh_bluetooth_text();
  watchface_set_battery(s_battery_state.charge_percent, s_battery_state.is_charging,
                        s_battery_state.is_plugged);
  watchface_set_strings(s->top_string, s->bottom_string);

  const bool inverted = desired_theme_inverted();
  if (inverted != s_theme_inverted) {
    s_theme_inverted = inverted;
    watchface_apply_theme(inverted);
  }
}

static void on_inbox_received(DictionaryIterator *iter, void *context) {
  settings_apply_inbox(iter, context);

  cancel_sync_timeout();
  watchface_set_sync_indicator(SyncIndicatorHidden);

  refresh_entire_face();
  reschedule_tick_service();
  schedule_data_request(/* delay_ms = */ 0);
}

static void on_inbox_dropped(AppMessageResult reason, void *context) {
  (void)context;
  APP_LOG(APP_LOG_LEVEL_WARNING, "Inbox message dropped: %d", (int)reason);
}

static void on_outbox_failed(DictionaryIterator *iter, AppMessageResult reason,
                             void *context) {
  (void)iter;
  (void)context;
  APP_LOG(APP_LOG_LEVEL_WARNING, "Outbox send failed: %d", (int)reason);
}

static void on_battery_updated(BatteryChargeState state) {
  s_battery_state = state;
  watchface_set_battery(state.charge_percent, state.is_charging, state.is_plugged);
}

// ------------------------------------------------------------- lifecycle --

static void init(void) {
  settings_load();

  watchface_init();
  watchface_push();

  s_bluetooth_connected = connection_service_peek_pebble_app_connection();
  s_battery_state = battery_state_service_peek();
  s_last_disconnect_time = 0;

  const struct tm boot_time = system_time();
  s_night_now = night_mode_active(&boot_time);
  s_theme_inverted = desired_theme_inverted();

  watchface_apply_theme(s_theme_inverted);
  refresh_entire_face();

  app_message_register_inbox_received(on_inbox_received);
  app_message_register_inbox_dropped(on_inbox_dropped);
  app_message_register_outbox_failed(on_outbox_failed);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

  connection_service_subscribe((ConnectionHandlers){
    .pebble_app_connection_handler = on_bluetooth_connection,
  });
  battery_state_service_subscribe(on_battery_updated);

  reschedule_tick_service();
  schedule_data_request(FIRST_DATA_REQUEST_DELAY_MS);
}

static void deinit(void) {
  tick_timer_service_unsubscribe();
  connection_service_unsubscribe();
  battery_state_service_unsubscribe();
  app_timer_cancel(s_data_request_timer);
  app_timer_cancel(s_sync_timeout_timer);
  watchface_deinit();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
