// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <pebble.h>

typedef enum {
  SyncIndicatorHidden,      // show the plain bluetooth state icon
  SyncIndicatorInProgress,  // data request sent, waiting for the phone
  SyncIndicatorError,       // the phone did not answer in time
} SyncIndicator;

// Creates the window. Layers and fonts are created in the window load handler,
// so call watchface_push() right after this.
void watchface_init(void);
void watchface_deinit(void);
void watchface_push(void);

// Appearance. apply_* methods only recolor / refont; the caller refreshes the
// icon-bearing setters afterwards so that bitmaps match the theme.
void watchface_apply_theme(bool inverted);
void watchface_apply_time_text_size(bool big);

// Content setters. All pointers must outlive the call (static buffers).
void watchface_set_time(const char *text);
void watchface_set_date_lines(const char *above, const char *below);
void watchface_set_strings(const char *top, const char *bottom);
void watchface_set_bluetooth(bool connected);
void watchface_set_bluetooth_text(const char *text);
void watchface_set_battery(int percent, bool charging, bool plugged);
void watchface_set_sync_indicator(SyncIndicator state);
