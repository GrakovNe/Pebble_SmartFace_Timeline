var languageKey = 0;
var windowColorKey = 1;
var timeTextSizeKey = 2;
var dateFormatKey = 3;
var showBatteryTextKey = 4;
var showBluetoothTextKey = 5;
var vibeHourlyVibeKey = 6;
var vibeBluetoothStateChangeVibeKey = 7;
var nightModeEnabledKey = 8;
var nightModeStartedKey = 9;
var nightModeFinishedKey = 10;
var nightModeHourlyVibeKey = 11;
var topAdditionalStringTextKey = 12;
var bottomAdditionalStringTextKey = 13;
var nightModeDisplayInvertKey = 14;
var nightModeUpdateInfoKey = 15;
var nightModeVibeOnEventKey = 16;
var dataUpdatesFrequencyKey = 17;
var weatherLocationKey = 18;
var weatherTemperatureUnitsKey = 19;

var language;
var windowColor;
var timeTextSize;
var dateFormat;
var showBatteryText;
var showBluetoothText;
var vibeHourlyVibe = 1;
var vibeBluetoothStateChangeVibe;
var nightModeEnabled;
var nightModeStarted;
var nightModeFinished;
var nightModeHourlyVibe;
var topAdditionalStringText;
var bottomAdditionalStringText;
var nightModeDisplayInvert;
var nightModeUpdateInfo;
var nightModeVibeOnEvent;
var dataUpdatesFrequency;

var weatherLocation = "Omsk";
var weatherTemperatureUnits = "C";

var topAdditionalInfoURL = "http://grakovne.org/pebble/smartface_timeline/scripts/current_weather.php";
var topAdditionalInfoArguments = "language=" + "1" + "&location=" + "Omsk" + "&weather_units=" + "C";

var bottomAdditionalInfoURL = "http://grakovne.org/pebble/SmartFace/CustomStings/Rates_USD_Brent.php";
var bottomAdditionalInfoArguments = "";

function readPersistSettings(){
	language = localStorage.getItem(languageKey);
	windowColor = localStorage.getItem(windowColorKey);
	timeTextSize = localStorage.getItem(timeTextSizeKey);
	dateFormat = localStorage.getItem(dateFormatKey);
	showBatteryText = localStorage.getItem(showBatteryTextKey);
	showBluetoothText = localStorage.getItem(showBluetoothTextKey);
	vibeHourlyVibe = localStorage.getItem(vibeHourlyVibeKey);
	vibeBluetoothStateChangeVibe = localStorage.getItem(vibeBluetoothStateChangeVibeKey);
	nightModeEnabled = localStorage.getItem(nightModeEnabledKey);
	nightModeStarted = localStorage.getItem(nightModeStartedKey);
	nightModeFinished = localStorage.getItem(nightModeFinishedKey);
	nightModeHourlyVibe = localStorage.getItem(nightModeHourlyVibeKey);
	topAdditionalStringText = localStorage.getItem(topAdditionalStringTextKey);
	bottomAdditionalStringText = localStorage.getItem(bottomAdditionalStringTextKey);
	nightModeDisplayInvert = localStorage.getItem(nightModeDisplayInvertKey);
	nightModeUpdateInfo = localStorage.getItem(nightModeUpdateInfoKey);
	nightModeVibeOnEvent = localStorage.getItem(nightModeVibeOnEventKey);
	dataUpdatesFrequency = localStorage.getItem(dataUpdatesFrequencyKey);
	weatherLocation = localStorage.getItem(weatherLocationKey);
	weatherTemperatureUnits = localStorage.getItem(weatherTemperatureUnitsKey);
	
	if (!language){
		language = 1;
	}
	
	if (!windowColor){
		windowColor = 0;
	}
	
	if (!timeTextSize){
		timeTextSize = 0;
	}
	
	if (!dateFormat){
		dateFormat = 1;
	}
	
	if (!showBatteryText){
		showBatteryText = 1;
	}
	
	if (!showBluetoothText){
		showBluetoothText = 1;
	}
	
	if (!vibeHourlyVibe){
		vibeHourlyVibe = 1;
	}
	
	if (!vibeBluetoothStateChangeVibe){
		vibeBluetoothStateChangeVibe = 1;
	}
	
	if (!nightModeEnabled){
		nightModeEnabled = 1;
	}
	
	if (!nightModeStarted){
		nightModeStarted = 1400;
	}
	
	if (!nightModeFinished){
		nightModeFinished = 600;
	}
	
	if (!nightModeHourlyVibe){
		nightModeHourlyVibe = 0;
	}
	
	if (!topAdditionalStringText){
		topAdditionalStringText = "SmartFace";
	}
	
	if (!bottomAdditionalStringText){
		bottomAdditionalStringText = "Grakovne";
	}
	
	if (!nightModeDisplayInvert){
		nightModeDisplayInvert = 1;
	}
	
	if (!nightModeUpdateInfo){
		nightModeUpdateInfo = 1;
	}
	
	if (!nightModeVibeOnEvent){
		nightModeVibeOnEvent = 1;
	}
		
	if (!dataUpdatesFrequency){
		dataUpdatesFrequency = 3;
	}
	
	if (!weatherLocationKey){
		weatherLocation = "Omsk";
	}
	
	if (!weatherTemperatureUnits){
		weatherTemperatureUnits = "C";
	}

}

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

function getAdditionalInfo(){
	var topAdditionalUrl = topAdditionalInfoURL + "?" + topAdditionalInfoArguments;
	topAdditionalStringText = getHttpData(topAdditionalUrl);
	
	var bottomAdditionalURL = bottomAdditionalInfoURL + "?" + bottomAdditionalInfoArguments;
	bottomAdditionalStringText = getHttpData(bottomAdditionalURL);
}

function sendSettings(){
	getAdditionalInfo();
	
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
	readPersistSettings();
    console.log("PebbleKit JS ready!");
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log("AppMessage received!");
	sendSettings();
	  
	   console.log("language " + language);
	  console.log("windowColor " + windowColor);
	  console.log("timeTextSize " + timeTextSize);
	  console.log("dateFormat " + dateFormat);
	  console.log("showBatteryText " + showBatteryText);
	  console.log("showBluetoothText " + showBluetoothText);
	  console.log("vibeHourlyVibe " + vibeHourlyVibe);
	  console.log("vibeBluetoothStateChangeVibe " + vibeBluetoothStateChangeVibe);
	  console.log("nightModeEnabled " + nightModeEnabled);
	  console.log("nightModeStarted " + nightModeStarted);
	  console.log("nightModeFinished " + nightModeFinished);
	  console.log("nightModeHourlyVibe " + nightModeHourlyVibe);
	  console.log("topAdditionalStringText " + topAdditionalStringText);
	  console.log("bottomAdditionalStringText " + bottomAdditionalStringText);
	  console.log("nightModeDisplayInvert " + nightModeDisplayInvert);
	  console.log("nightModeUpdateInfo " + nightModeUpdateInfo);
	  console.log("nightModeVibeOnEvent " + nightModeVibeOnEvent);
	  console.log("dataUpdatesFrequency " + dataUpdatesFrequency);
	  console.log("weatherTemperatureUnits " + weatherTemperatureUnits);
	  console.log("location " + location);
  }                     
);
