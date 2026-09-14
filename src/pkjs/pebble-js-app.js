// SPDX-License-Identifier: Apache-2.0
//
// PebbleKit JS side of SmartFace TimeLine.
//
// Two jobs:
//  1. Serve the configuration webview (`configurationURL`) and relay the
//     chosen settings to the watchapp as an AppMessage on `webviewclosed`.
//  2. Answer KEY_ASK_DATA requests: a configured string is sent back as-is,
//     unless it is an http(s) URL — then its body is fetched and sent.
'use strict';

// The webview can only load http(s) pages, so the configuration page must be
// hosted somewhere. See README.md: put the `config/` directory on GitHub Pages
// and adjust this URL to your repository.
var CONFIG_URL = 'https://grakovne.github.io/Pebble_SmartFace_Timeline/config/index.html';

var SETTINGS_STORAGE_KEY = 'smartface-timeline-settings';
var MAX_STRING_LENGTH = 28; // settings.h buffer is 29 bytes including the NUL

function loadSettings() {
  try {
    return JSON.parse(localStorage.getItem(SETTINGS_STORAGE_KEY)) || {};
  } catch (e) {
    return {};
  }
}

function saveSettings(settings) {
  localStorage.setItem(SETTINGS_STORAGE_KEY, JSON.stringify(settings));
}

function trimString(value) {
  if (typeof value !== 'string') {
    return '';
  }
  return value.substring(0, MAX_STRING_LENGTH);
}

function isHttpUrl(value) {
  return typeof value === 'string' && /^https?:\/\//i.test(value);
}

// The watch requests fresh strings; a blocking fetch is acceptable here
// because the reply is the only thing this handler produces.
function fetchUrl(url) {
  var request = new XMLHttpRequest();
  request.open('GET', url, false);
  try {
    request.send();
  } catch (e) {
    return '';
  }
  return request.status === 200 ? request.responseText : '';
}

// First non-empty line of the response, trimmed to the watch's limit.
function firstLine(body) {
  var lines = (body || '').split(/\r?\n/);
  for (var i = 0; i < lines.length; i += 1) {
    if (lines[i].trim().length > 0) {
      return lines[i].trim();
    }
  }
  return '';
}

function resolveString(value) {
  return isHttpUrl(value) ? trimString(firstLine(fetchUrl(value))) : trimString(value);
}

// AppKeys is injected by the phone app from `pebble.messageKeys` in
// package.json; raw KEY_* identifiers are not defined in JS.
function messageDict(values) {
  var dict = {};
  for (var name in values) {
    if (Object.prototype.hasOwnProperty.call(values, name)) {
      dict[AppKeys[name]] = values[name];
    }
  }
  return dict;
}

function sendAdditionalStrings() {
  var settings = loadSettings();
  pebble.sendUIAppMessage(messageDict({
    KEY_TOP_STRING: resolveString(settings.topString),
    KEY_BOTTOM_STRING: resolveString(settings.bottomString)
  }));
}

// Called by the phone app when the user taps "Settings".
function configurationURL() {
  return CONFIG_URL + '?state=' + encodeURIComponent(JSON.stringify(loadSettings()));
}

pebble.addEventListener('webviewclosed', function (event) {
  if (!event.response) {
    return; // cancelled
  }
  var settings;
  try {
    settings = JSON.parse(decodeURIComponent(event.response));
  } catch (e) {
    return;
  }
  saveSettings(settings);
  pebble.sendAppMessage(messageDict({
    KEY_LANGUAGE: settings.language,
    KEY_WINDOW_COLOR: settings.windowColor,
    KEY_TIME_TEXT_SIZE: settings.timeTextSize,
    KEY_DATE_FORMAT: settings.dateFormat,
    KEY_SHOW_BATTERY_TEXT: settings.showBatteryText,
    KEY_SHOW_BLUETOOTH_TEXT: settings.showBluetoothText,
    KEY_VIBE_HOURLY: settings.vibeHourly,
    KEY_VIBE_BLUETOOTH: settings.vibeBluetooth,
    KEY_NIGHT_MODE_ENABLED: settings.nightModeEnabled,
    KEY_NIGHT_MODE_START: settings.nightModeStart,
    KEY_NIGHT_MODE_END: settings.nightModeEnd,
    KEY_TOP_STRING: trimString(settings.topString),
    KEY_BOTTOM_STRING: trimString(settings.bottomString),
    KEY_NIGHT_MODE_INVERT: settings.nightModeInvert,
    KEY_NIGHT_MODE_UPDATE_INFO: settings.nightModeUpdateInfo,
    KEY_NIGHT_MODE_VIBE_EVENT: settings.nightModeVibeEvent,
    KEY_DATA_UPDATE_FREQ: settings.dataUpdateFreq,
    KEY_NIGHT_MODE_VIBE_HOURLY: settings.nightModeVibeHourly,
    KEY_DATE_STYLE: settings.dateStyle,
    KEY_SHOW_LAST_DISCONNECT: settings.showLastDisconnect,
    KEY_BLINK_COLON: settings.blinkColon,
    KEY_NIGHT_MODE_BLINK_COLON: settings.nightModeBlinkColon
  }));
  sendAdditionalStrings();
});

pebble.listen(function (message) {
  if (message && typeof message.KEY_ASK_DATA !== 'undefined') {
    sendAdditionalStrings();
  }
});
