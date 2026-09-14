// SPDX-License-Identifier: Apache-2.0
//
// Watchface layout for Pebble Time 2 (`emery`, 200x228, color).
//
//   +------------------------------------------------+
//   | (bt)  connected text            battery%  (bat)|   status row
//   |                                                |
//   |                  weekday / date line           |
//   |                     12:34                      |   time
//   |                  date line / weekday           |
//   |               top additional string            |
//   |              bottom additional string          |
//   +------------------------------------------------+
#include "watchface.h"

#include "settings.h"

#define CLAMP(value, lo, hi) ((value) < (lo) ? (lo) : ((value) > (hi) ? (hi) : (value)))
#define STATUS_ICON_SIZE 20
#define BATTERY_FRAME_MS 800
#define BATTERY_LEVELS 11  // 0, 10, ..., 100 percent; +1 == "full" icon

static Window *s_window;

static TextLayer *s_bluetooth_text_layer;
static TextLayer *s_battery_text_layer;
static TextLayer *s_date_above_layer;
static TextLayer *s_date_below_layer;
static TextLayer *s_time_layer;
static TextLayer *s_top_string_layer;
static TextLayer *s_bottom_string_layer;

static BitmapLayer *s_bluetooth_icon_layer;
static BitmapLayer *s_battery_icon_layer;

static GBitmap *s_bluetooth_icon;
static GBitmap *s_battery_icon;

static GFont s_font_status;
static GFont s_font_date;
static GFont s_font_string;
static GFont s_font_time_big;
static GFont s_font_time_small;

static AppTimer *s_battery_animation_timer;
static int s_battery_animation_level;

// Cached state so that a theme switch can repaint the icons.
static bool s_bluetooth_connected;
static int s_battery_percent;
static bool s_battery_charging;
static bool s_battery_plugged;
static SyncIndicator s_sync_indicator = SyncIndicatorHidden;
static bool s_theme_inverted;

static const uint32_t s_battery_icons[2][BATTERY_LEVELS + 1] = {
  {
    RESOURCE_ID_BATTERY_0_NORMAL, RESOURCE_ID_BATTERY_10_NORMAL,
    RESOURCE_ID_BATTERY_20_NORMAL, RESOURCE_ID_BATTERY_30_NORMAL,
    RESOURCE_ID_BATTERY_40_NORMAL, RESOURCE_ID_BATTERY_50_NORMAL,
    RESOURCE_ID_BATTERY_60_NORMAL, RESOURCE_ID_BATTERY_70_NORMAL,
    RESOURCE_ID_BATTERY_80_NORMAL, RESOURCE_ID_BATTERY_90_NORMAL,
    RESOURCE_ID_BATTERY_100_NORMAL, RESOURCE_ID_BATTERY_FULL_NORMAL,
  },
  {
    RESOURCE_ID_BATTERY_0_INVERTED, RESOURCE_ID_BATTERY_10_INVERTED,
    RESOURCE_ID_BATTERY_20_INVERTED, RESOURCE_ID_BATTERY_30_INVERTED,
    RESOURCE_ID_BATTERY_40_INVERTED, RESOURCE_ID_BATTERY_50_INVERTED,
    RESOURCE_ID_BATTERY_60_INVERTED, RESOURCE_ID_BATTERY_70_INVERTED,
    RESOURCE_ID_BATTERY_80_INVERTED, RESOURCE_ID_BATTERY_90_INVERTED,
    RESOURCE_ID_BATTERY_100_INVERTED, RESOURCE_ID_BATTERY_FULL_INVERTED,
  },
};

static const uint32_t s_bluetooth_icons[2][2] = {
  { RESOURCE_ID_BLUETOOTH_NO_NORMAL, RESOURCE_ID_BLUETOOTH_YES_NORMAL },
  { RESOURCE_ID_BLUETOOTH_NO_INVERTED, RESOURCE_ID_BLUETOOTH_YES_INVERTED },
};

static const uint32_t s_sync_icons[2][2] = {
  { RESOURCE_ID_SYNC_ACTIVE_NORMAL, RESOURCE_ID_SYNC_ERROR_NORMAL },
  { RESOURCE_ID_SYNC_ACTIVE_INVERTED, RESOURCE_ID_SYNC_ERROR_INVERTED },
};

static int theme_index(void) {
  return s_theme_inverted ? 1 : 0;
}

static void set_icon(BitmapLayer *layer, GBitmap **current, uint32_t resource_id) {
  if (*current != NULL) {
    gbitmap_destroy(*current);
  }
  *current = gbitmap_create_with_resource(resource_id);
  bitmap_layer_set_bitmap(layer, *current);
}

// ---------------------------------------------------------------- icons ----

static int s_battery_displayed_level;

// Icon level for a stationary (non-animating) battery.
static int battery_static_level(void) {
  return (s_battery_plugged && s_battery_percent >= 100) ? BATTERY_LEVELS
                                                         : s_battery_percent / 10;
}

static void show_battery_icon(int level) {
  s_battery_displayed_level = level;
  set_icon(s_battery_icon_layer, &s_battery_icon, s_battery_icons[theme_index()][level]);
}

// Redraws the battery icon in the current palette without touching timers.
static void repaint_battery_icon(void) {
  show_battery_icon(s_battery_animation_timer != NULL ? s_battery_displayed_level
                                                      : battery_static_level());
}

static void battery_animation_step(void *context) {
  (void)context;
  if (s_battery_animation_level > BATTERY_LEVELS - 1) {
    s_battery_animation_level = s_battery_percent / 10;
  }
  show_battery_icon(s_battery_animation_level);
  s_battery_animation_level += 1;

  if (s_battery_charging && s_battery_percent < 100) {
    s_battery_animation_timer = app_timer_register(BATTERY_FRAME_MS, battery_animation_step,
                                                   NULL);
  } else {
    s_battery_animation_timer = NULL;
    show_battery_icon(battery_static_level());
  }
}

static void start_battery_animation(void) {
  app_timer_cancel(s_battery_animation_timer);
  s_battery_animation_level = s_battery_percent / 10;
  battery_animation_step(NULL);
}

static void show_bluetooth_icon(void) {
  if (s_sync_indicator == SyncIndicatorHidden) {
    set_icon(s_bluetooth_icon_layer, &s_bluetooth_icon,
             s_bluetooth_icons[theme_index()][s_bluetooth_connected ? 1 : 0]);
  } else {
    set_icon(s_bluetooth_icon_layer, &s_bluetooth_icon,
             s_sync_icons[theme_index()][s_sync_indicator == SyncIndicatorError ? 1 : 0]);
  }
}

// -------------------------------------------------------------- layout ----

static TextLayer *create_text_layer(Layer *parent, GRect frame, GFont font,
                                    GTextAlignment alignment) {
  TextLayer *layer = text_layer_create(frame);
  text_layer_set_font(layer, font);
  text_layer_set_background_color(layer, GColorClear);
  text_layer_set_text_alignment(layer, alignment);
  text_layer_set_overflow_mode(layer, GTextOverflowModeTrailingEllipsis);
  layer_add_child(parent, text_layer_get_layer(layer));
  return layer;
}

static void window_load(Window *window) {
  Layer *root = window_get_root_layer(window);
  const int16_t w = layer_get_bounds(root).size.w;

  s_font_status = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_IMAGINE_FONT_12));
  s_font_date = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_IMAGINE_FONT_17));
  s_font_string = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_IMAGINE_FONT_14));
  s_font_time_big = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_IMAGINE_BIG_41));
  s_font_time_small = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_IMAGINE_SMALL_37));

  s_bluetooth_icon_layer = bitmap_layer_create(
    GRect(4, 4, STATUS_ICON_SIZE, STATUS_ICON_SIZE));
  layer_add_child(root, bitmap_layer_get_layer(s_bluetooth_icon_layer));

  s_battery_icon_layer = bitmap_layer_create(
    GRect(w - 4 - STATUS_ICON_SIZE, 4, STATUS_ICON_SIZE, STATUS_ICON_SIZE));
  layer_add_child(root, bitmap_layer_get_layer(s_battery_icon_layer));

  s_bluetooth_text_layer = create_text_layer(
    root, GRect(4 + STATUS_ICON_SIZE + 2, 7, w / 2 - STATUS_ICON_SIZE, 16), s_font_status,
    GTextAlignmentLeft);

  s_battery_text_layer = create_text_layer(
    root, GRect(w / 2, 7, w / 2 - STATUS_ICON_SIZE - 6, 16), s_font_status,
    GTextAlignmentRight);

  s_date_above_layer = create_text_layer(root, GRect(0, 48, w, 22), s_font_date,
                                         GTextAlignmentCenter);

  s_time_layer = create_text_layer(root, GRect(0, 72, w, 58), s_font_time_big,
                                   GTextAlignmentCenter);

  s_date_below_layer = create_text_layer(root, GRect(0, 138, w, 22), s_font_date,
                                         GTextAlignmentCenter);

  s_top_string_layer = create_text_layer(root, GRect(8, 182, w - 16, 18), s_font_string,
                                         GTextAlignmentCenter);

  s_bottom_string_layer = create_text_layer(root, GRect(8, 202, w - 16, 18), s_font_string,
                                            GTextAlignmentCenter);
}

static void window_unload(Window *window) {
  (void)window;
  app_timer_cancel(s_battery_animation_timer);
  s_battery_animation_timer = NULL;

  fonts_unload_custom_font(s_font_status);
  fonts_unload_custom_font(s_font_date);
  fonts_unload_custom_font(s_font_string);
  fonts_unload_custom_font(s_font_time_big);
  fonts_unload_custom_font(s_font_time_small);

  if (s_bluetooth_icon != NULL) {
    gbitmap_destroy(s_bluetooth_icon);
    s_bluetooth_icon = NULL;
  }
  if (s_battery_icon != NULL) {
    gbitmap_destroy(s_battery_icon);
    s_battery_icon = NULL;
  }

  text_layer_destroy(s_bluetooth_text_layer);
  text_layer_destroy(s_battery_text_layer);
  text_layer_destroy(s_date_above_layer);
  text_layer_destroy(s_date_below_layer);
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_top_string_layer);
  text_layer_destroy(s_bottom_string_layer);

  bitmap_layer_destroy(s_bluetooth_icon_layer);
  bitmap_layer_destroy(s_battery_icon_layer);
}

// ---------------------------------------------------------------- API -----

void watchface_init(void) {
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers){
    .load = window_load,
    .unload = window_unload,
  });
}

void watchface_push(void) {
  window_stack_push(s_window, /* animated = */ false);
}

void watchface_deinit(void) {
  window_destroy(s_window);
}

void watchface_apply_theme(bool inverted) {
  s_theme_inverted = inverted;
  const GColor background = inverted ? GColorBlack : GColorWhite;
  const GColor foreground = inverted ? GColorWhite : GColorBlack;

  window_set_background_color(s_window, background);
  TextLayer *layers[] = {
    s_bluetooth_text_layer, s_battery_text_layer, s_date_above_layer, s_date_below_layer,
    s_time_layer, s_top_string_layer, s_bottom_string_layer,
  };
  for (size_t i = 0; i < ARRAY_LENGTH(layers); i += 1) {
    text_layer_set_text_color(layers[i], foreground);
  }

  // Repaint the icons with the new palette.
  show_bluetooth_icon();
  repaint_battery_icon();
}

void watchface_apply_time_text_size(bool big) {
  text_layer_set_font(s_time_layer, big ? s_font_time_big : s_font_time_small);
  const int16_t w = layer_get_bounds(window_get_root_layer(s_window)).size.w;
  layer_set_frame(text_layer_get_layer(s_time_layer),
                                      big ? GRect(0, 72, w, 58) : GRect(0, 76, w, 54));
}

void watchface_set_time(const char *text) {
  text_layer_set_text(s_time_layer, text);
}

void watchface_set_date_lines(const char *above, const char *below) {
  text_layer_set_text(s_date_above_layer, above);
  text_layer_set_text(s_date_below_layer, below);
}

void watchface_set_strings(const char *top, const char *bottom) {
  text_layer_set_text(s_top_string_layer, top);
  text_layer_set_text(s_bottom_string_layer, bottom);
}

void watchface_set_bluetooth(bool connected) {
  s_bluetooth_connected = connected;
  show_bluetooth_icon();
}

void watchface_set_bluetooth_text(const char *text) {
  text_layer_set_text(s_bluetooth_text_layer, text);
}

void watchface_set_battery(int percent, bool charging, bool plugged) {
  s_battery_percent = CLAMP(percent, 0, 100);
  s_battery_charging = charging;
  s_battery_plugged = plugged;

  if (charging && s_battery_percent < 100) {
    start_battery_animation();
  } else {
    app_timer_cancel(s_battery_animation_timer);
    s_battery_animation_timer = NULL;
    show_battery_icon(battery_static_level());
  }

  char buffer[8];
  if (settings_get()->show_battery_text) {
    snprintf(buffer, sizeof(buffer), "%d%%", s_battery_percent);
  } else {
    buffer[0] = '\0';
  }
  text_layer_set_text(s_battery_text_layer, buffer);
}

void watchface_set_sync_indicator(SyncIndicator state) {
  s_sync_indicator = state;
  show_bluetooth_icon();
}
