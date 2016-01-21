var language = 1;
var windowColor = 0;
var timeTextSize = 0;
var dateFormat = 1;
var showBatteryText = 1;
var showBluetoothText = 1;

var vibeHourlyVibe = 1;
var vibeBluetoothStateChangeVibe = 0;
var nightModeEnabled = 1;
var nightModeStarted = 1380;
var nightModeFinished = 480;
var nightModeHourlyVibe = 0;
var topAdditionalStringText = "SmartFace";
var bottomAdditionalStringText = "GrakovNe";
var nightModeDisplayInvert = 1;
var nightModeUpdateInfo = 0;
var nightModeVibeOnEvent = 0;
var dataUpdatesFrequency = 1;

function getHttpData(url) {
    var req = new XMLHttpRequest();
		try {
			req.open("GET", url, false);
			req.send(null);
            }
		catch (err) {}
		if(req.status == 200)
			return req.responseText;
	return 0;
}

function sendSettings(){
	var dictionary = {
		"LANGUAGE_INFO": language,
		"WINDOW_COLOR_INFO": windowColor, 
		"TIME_TEXT_SIZE_INFO": timeTextSize,
		"DATE_FORMAT_INFO": dateFormat,
		"SHOW_BATTERY_TEXT_INFO": showBatteryText,
		"SHOW_BLUETOOTH_TEXT_INFO": showBluetoothText,
		"VIBE_HOURLY_VIBE_INFO": vibeHourlyVibe,
		"VIBE_BLUETOOTH_STATE_CHANGE_INFO": vibeBluetoothStateChangeVibe,
		"NIGHT_MODE_ENABLED_INFO": nightModeEnabled,
		"NIGHT_MODE_STARTED_INFO": nightModeStarted,
		"NIGTH_MODE_FINISHED_INFO": nightModeFinished,
		"TOP_ADDITIONAL_STRING_TEXT_INFO": topAdditionalStringText,
		"BOTTOM_ADDITIONAL_STRING_TEXT_INFO": bottomAdditionalStringText,
		"NIGTH_MODE_DISPLAY_INVERT_INFO": nightModeDisplayInvert,
		"NIGHT_MODE_UPDATE_INFO_INFO": nightModeUpdateInfo,
		"NIGTH_MODE_VIBE_ON_EVENT_INFO": nightModeVibeOnEvent,
		"DATA_UPDATE_FREQUENCY_INFO": dataUpdatesFrequency,
		"NIGHT_MODE_HOURLY_VIBE_INFO": nightModeHourlyVibe
      };
      Pebble.sendAppMessage(dictionary,
        function(e) {
          console.log("Info sent to Pebble successfully!");
        },
        function(e) {
          console.log("Error sending weather info to Pebble!");
        }
      );
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log("PebbleKit JS ready!");

    // Get the initial weather
    //getWeather();
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log("AppMessage received!");
	sendSettings();
  }                     
);
