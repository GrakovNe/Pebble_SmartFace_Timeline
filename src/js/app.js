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
var topAdditionalInfoURLKey = 20;
var bottomAdditionalInfoURLKey = 21;
var dateStyleKey = 22;
var showLastDisconnectTimeKey = 23;

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
var topAdditionalInfoURL;
var bottomAdditionalInfoURL;
var dateStyle;
var showLastDisconnectTime;

var weatherLocation = "Omsk";
var weatherTemperatureUnits = "C";

function readPersistSettings(){
	language = localStorage.getItem(languageKey);
	windowColor = localStorage.getItem(windowColorKey);
	timeTextSize = localStorage.getItem(timeTextSizeKey);
	dateFormat = localStorage.getItem(dateFormatKey);
	dateStyle = localStorage.getItem(dateStyleKey);
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
	topAdditionalInfoURL = localStorage.getItem(topAdditionalInfoURLKey);
	bottomAdditionalInfoURL = localStorage.getItem(bottomAdditionalInfoURLKey);
	showLastDisconnectTime = localStorage.getItem(showLastDisconnectTimeKey);
	
	if (!language){
		language = 1;
	}
	
	if (!showLastDisconnectTime){
		showLastDisconnectTime = 0;
	}
	
	if (!windowColor){
		windowColor = 0;
	}
	
	if (!timeTextSize){
		timeTextSize = 0;
	}
	
	if (!dateFormat){
		dateFormat = 0;
	}
	
	if (!dateStyle){
		dateFormat = 0;
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
	
	if (!topAdditionalInfoURL){
		topAdditionalInfoURL = "http://grakovne.org/pebble/smartface_timeline/scripts/current_weather.php" + "?language=" + "0" + "&location=" + "Omsk" + "&weather_units=" + "C";
	}
	
	if (!bottomAdditionalInfoURL){
		bottomAdditionalInfoURL = "http://grakovne.org/pebble/SmartFace/CustomStings/Rates_USD_Brent.php";
	}
	
	/*console.log("language " + language);
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
	console.log("location " + weatherLocation);*/

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
	var serverData;
	
	if (topAdditionalInfoURL.search("http://grakovne.org") === 0){
		serverData = getHttpData(topAdditionalInfoURL);
		if (serverData !== 0){
			topAdditionalStringText = serverData;
		}
	}
	
	else {
		topAdditionalStringText = topAdditionalInfoURL;
	}
	
	topAdditionalStringText = cropAdditionalInfo(topAdditionalStringText);
	
	if (bottomAdditionalInfoURL.search("http://grakovne.org") === 0){
		serverData = getHttpData(bottomAdditionalInfoURL);
		if (serverData !== 0){
			bottomAdditionalStringText = serverData;
		}
	}
	
	else {
		bottomAdditionalStringText = bottomAdditionalInfoURL;
	}
	
	bottomAdditionalStringText = cropAdditionalInfo(bottomAdditionalStringText);
}

function sendSettings(){
	getAdditionalInfo();
	
	var dictionary = {
		"LANGUAGE_INFO": parseInt(language),
		"WINDOW_COLOR_INFO": parseInt(windowColor), 
		"TIME_TEXT_SIZE_INFO": parseInt(timeTextSize),
		"DATE_FORMAT_INFO": parseInt(dateFormat),
		"SHOW_BATTERY_TEXT_INFO": parseInt(showBatteryText),
		"SHOW_BLUETOOTH_TEXT_INFO": parseInt(showBluetoothText),
		"VIBE_HOURLY_VIBE_INFO": parseInt(vibeHourlyVibe),
		"VIBE_BLUETOOTH_STATE_CHANGE_INFO": parseInt(vibeBluetoothStateChangeVibe),
		"NIGHT_MODE_ENABLED_INFO": parseInt(nightModeEnabled),
		"NIGHT_MODE_STARTED_INFO": parseInt(nightModeStarted),
		"NIGTH_MODE_FINISHED_INFO": parseInt(nightModeFinished),
		"TOP_ADDITIONAL_STRING_TEXT_INFO": topAdditionalStringText,
		"BOTTOM_ADDITIONAL_STRING_TEXT_INFO": bottomAdditionalStringText,
		"NIGTH_MODE_DISPLAY_INVERT_INFO": parseInt(nightModeDisplayInvert),
		"NIGHT_MODE_UPDATE_INFO_INFO": parseInt(nightModeUpdateInfo),
		"NIGTH_MODE_VIBE_ON_EVENT_INFO": parseInt(nightModeVibeOnEvent),
		"DATA_UPDATE_FREQUENCY_INFO": parseInt(dataUpdatesFrequency),
		"NIGHT_MODE_HOURLY_VIBE_INFO": parseInt(nightModeHourlyVibe),
		"DATE_STYLE_INFO": parseInt(dateStyle),
		"SHOW_LAST_DISCONNECT_TIME_INFO": parseInt(showLastDisconnectTime)
      };
      Pebble.sendAppMessage(dictionary,
        function(e) {
          //console.log("Info sent to Pebble successfully!");
        },
        function(e) {
          //console.log("Error sending weather info to Pebble!");
        }
      );
}

function sendAdditionalInfo(){
	getAdditionalInfo();
	
	var dictionary = {
		"TOP_ADDITIONAL_STRING_TEXT_INFO": topAdditionalStringText,
		"BOTTOM_ADDITIONAL_STRING_TEXT_INFO": bottomAdditionalStringText
      };
      Pebble.sendAppMessage(dictionary,
        function(e) {
          //console.log("Info sent to Pebble successfully!");
        },
        function(e) {
          //console.log("Error sending weather info to Pebble!");
        }
      );
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
	readPersistSettings();
    //console.log("PebbleKit JS ready!");
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    //console.log("AppMessage received!");
	//sendSettings();
	  sendAdditionalInfo();
  }                     
);

Pebble.addEventListener("showConfiguration",
  function(e) {
		Pebble.openURL("http://grakovne.org/pebble/smartface_timeline/settings/settings.html");
	}
);

function parseTime(timeText){
	return parseInt(timeText.substring(0, timeText.search(":"))) * 60 + parseInt(timeText.substring(1 + timeText.search(":")));
}

function cropAdditionalInfo(textString){
	if (textString.length > 15){
		textString = textString.substring(0, 12) + "...";
	}
	
	return textString;
}

Pebble.addEventListener("webviewclosed",
  function(e) {
	  //console.log('Configuration window returned: ' + e.response);
	  
	  var configuration = JSON.parse(decodeURIComponent(e.response));
	  
	  localStorage.setItem(languageKey, parseInt(configuration.language));
	  localStorage.setItem(windowColorKey, parseInt(configuration.windowColor));
	  localStorage.setItem(timeTextSizeKey, parseInt(configuration.timeTextSize));
	  localStorage.setItem(dateFormatKey, parseInt(configuration.dateFormat));
	  localStorage.setItem(dateStyleKey, parseInt(configuration.dateStyle));
	  localStorage.setItem(showBatteryTextKey, parseInt(configuration.showBatteryText));
	  localStorage.setItem(showBluetoothTextKey, parseInt(configuration.showBluetoothText));
	  localStorage.setItem(vibeHourlyVibeKey, parseInt(configuration.vibeHourlyVibe));
	  localStorage.setItem(vibeBluetoothStateChangeVibeKey, parseInt(configuration.vibeBluetoothStateChangeVibe));
	  localStorage.setItem(nightModeEnabledKey, parseInt(configuration.nightModeEnabled));
	  localStorage.setItem(nightModeStartedKey, parseInt(parseTime(configuration.nightModeStarted)));
	  localStorage.setItem(nightModeFinishedKey, parseInt(parseTime(configuration.nightModeFinished)));
	  localStorage.setItem(nightModeHourlyVibeKey, parseInt(configuration.nightModeHourlyVibe));
	  localStorage.setItem(nightModeDisplayInvertKey, parseInt(configuration.nightModeDisplayInvert));
	  localStorage.setItem(nightModeUpdateInfoKey, parseInt(configuration.nightModeUpdateInfo));
	  localStorage.setItem(nightModeVibeOnEventKey, parseInt(configuration.nightModeVibeOnEvent));
	  localStorage.setItem(dataUpdatesFrequencyKey, parseInt(configuration.dataUpdatesFrequency));
	  localStorage.setItem(weatherLocationKey, configuration.location);
	  localStorage.setItem(weatherTemperatureUnitsKey, configuration.weatherTemperatureUnits);
	  localStorage.setItem(topAdditionalInfoURLKey, configuration.topAdditionalStringURL);
	  localStorage.setItem(bottomAdditionalInfoURLKey, configuration.bottomAdditionalStringURL);
	  localStorage.setItem(showLastDisconnectTimeKey, configuration.showLastDisconnectTime);
	  
	  
	  readPersistSettings();
	  getAdditionalInfo();
	  sendSettings();
  }
);
