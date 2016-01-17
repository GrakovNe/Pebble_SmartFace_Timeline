#include <pebble.h>
#include "main_window.h"
#include "resources.h"

extern Window *main_window;
extern TextLayer *text_layer;
extern TextLayer *time_text;
extern TextLayer *date_text;
extern TextLayer *weekday_text;
extern TextLayer *bluetooth_text;
extern TextLayer *battery_text;
extern TextLayer *top_additional_info_text;
extern TextLayer *bottom_additional_info_text;

extern BitmapLayer *bluetooth_icon_layer;
extern BitmapLayer *battery_icon_layer;

extern GBitmap *bluetooth_icon;
extern GBitmap *battery_icon;

struct {
	int language;
	int window_color;
	int time_text_size;
	int date_format;
	int show_battery_text;
	int show_bluetooth_text;
	int show_top_additional_info;
	int show_bottom_additional_info;
	int vibe_hourly_vibe;
	int vibe_bluetooth_state_change;
	int night_mode_enabled;
	int night_mode_started;
	int night_mode_finished;
	int night_mode_display_invert;
	int night_mode_update_info;
	int night_mode_vibe_on_event;
	int data_updates_frequency;
} settings;

struct {
	int current_window_color;
	int is_bluetooth_connected;
} flags;

char top_additional_info_buffer    [24];
char bottom_additional_info_buffer [24];
char time_text_buffer              [6];
char date_text_buffer              [24];
char battery_text_buffer           [4];

AppTimer* is_receiving_data;

void update_time(struct tm* current_time, TimeUnits units_changed);
void update_date(struct tm* current_time, TimeUnits units_changed);
void update_bluetooth_connection(bool is_connected);
void update_battery_state(BatteryChargeState battery_state);
void update_additional_info(void);
void read_persist_settings(void);
void initialization(void);
void deinitialization(void);
void update_icons(void);

int main(void);

void request_data_error(){
	gbitmap_destroy(bluetooth_icon); 
	bluetooth_icon = gbitmap_create_with_resource(updating_icons[flags.current_window_color][1]); 
	bitmap_layer_set_bitmap(bluetooth_icon_layer, bluetooth_icon);
	layer_mark_dirty((Layer *)bluetooth_icon_layer);
}

void request_data_from_phone(){
	is_receiving_data = app_timer_register(RECEIVING_LATENCY_TIME, request_data_error, 0);
	
	gbitmap_destroy(bluetooth_icon); 
	bluetooth_icon = gbitmap_create_with_resource(updating_icons[flags.current_window_color][0]); 
	bitmap_layer_set_bitmap(bluetooth_icon_layer, bluetooth_icon);
	layer_mark_dirty((Layer *)bluetooth_icon_layer);
	
	// Begin dictionary
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    // Add a key-value pair
    dict_write_uint8(iter, 0, 0);

    // Send the message!
    app_message_outbox_send();
		
	APP_LOG(APP_LOG_LEVEL_INFO, "SmartFace: data is requested!");
	
	/*Сreating the timer again*/
	app_timer_register(MILLS_IN_HOUR / settings.data_updates_frequency, request_data_from_phone, 0);
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
	
 	Tuple *temp_tuple = dict_find(iterator, LANGUAGE_INFO);
	APP_LOG(APP_LOG_LEVEL_ERROR, "RECEIVED");
	
	gbitmap_destroy(bluetooth_icon); 
	bluetooth_icon = gbitmap_create_with_resource(bluetooth_icons[flags.current_window_color][flags.is_bluetooth_connected]); 
	bitmap_layer_set_bitmap(bluetooth_icon_layer, bluetooth_icon);
	layer_mark_dirty((Layer *)bluetooth_icon_layer);
	
	app_timer_cancel(is_receiving_data);
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

void update_icons(){
	gbitmap_destroy(bluetooth_icon); 
	bluetooth_icon = gbitmap_create_with_resource(bluetooth_icons[flags.current_window_color][flags.is_bluetooth_connected]); 
	bitmap_layer_set_bitmap(bluetooth_icon_layer, bluetooth_icon);
	layer_mark_dirty((Layer *)bluetooth_icon_layer);
}

bool is_night(){
	if (!settings.night_mode_enabled){
		return false;
	}
	
	time_t now = time(NULL);
	struct tm *current_time = localtime(&now);
	
	int current_time_minutes = (current_time -> tm_hour) * 60 + (current_time -> tm_min);
	
	if (settings.night_mode_started > settings.night_mode_finished){
		if ( (current_time_minutes >= settings.night_mode_started) || (current_time_minutes <= settings.night_mode_finished) ){
			return true;
		}
	}
	
	if (settings.night_mode_started < settings.night_mode_finished){
		if ( (current_time_minutes >= settings.night_mode_started) && (current_time_minutes <= settings.night_mode_finished) ){
			return true;
		}
	}
	
	return false;	
}

void read_persist_settings(void){
	if (persist_exists(LANGUAGE_KEY)){
		settings.language = persist_read_int(LANGUAGE_KEY);
	} else {
		settings.language = ENGLISH_LANGUAGE;
		persist_write_int(LANGUAGE_KEY, ENGLISH_LANGUAGE);
	}
	
	if (persist_exists(WINDOW_COLOR_KEY)){
		settings.window_color = persist_read_int(WINDOW_COLOR_KEY);
	} else {
		settings.window_color = NORMAL_COLOR;
		persist_write_int(WINDOW_COLOR_KEY, NORMAL_COLOR);
	}
	
	if (persist_exists(TIME_TEXT_SIZE_KEY)){
		settings.time_text_size = persist_read_int(TIME_TEXT_SIZE_KEY);
	} else {
		settings.time_text_size = SMALL_TIME_TEXT;
		persist_write_int(TIME_TEXT_SIZE_KEY, SMALL_TIME_TEXT);
	}
	
	if (persist_exists(DATE_FORMAT_KEY)){
		settings.date_format = persist_read_int(DATE_FORMAT_KEY);
	} else {
		settings.date_format = DD_MMMM_DATE_FORMAT;
		persist_write_int(DATE_FORMAT_KEY, DD_MMMM_DATE_FORMAT);
	}
	
	if (persist_exists(SHOW_BATTERY_TEXT_KEY)){
		settings.show_battery_text = persist_read_int(SHOW_BATTERY_TEXT_KEY);
	} else {
		settings.show_battery_text = 1;
		persist_write_int(SHOW_BATTERY_TEXT_KEY, 1);
	}
	
	if (persist_exists(SHOW_BLUETOOTH_TEXT_KEY)){
		settings.show_bluetooth_text = persist_read_int(SHOW_BLUETOOTH_TEXT_KEY);
	} else {
		settings.show_bluetooth_text = 1;
		persist_write_int(SHOW_BLUETOOTH_TEXT_KEY, 1);
	}
	
	if (persist_exists(SHOW_TOP_ADDITIONAL_INFO_KEY)){
		settings.show_top_additional_info = persist_read_int(SHOW_TOP_ADDITIONAL_INFO_KEY);
	} else {
		settings.show_top_additional_info = 1;
		persist_write_int(SHOW_TOP_ADDITIONAL_INFO_KEY, 1);
	}
	
	if (persist_exists(SHOW_BOTTOM_ADDITIONAL_INFO_KEY)){
		settings.show_bottom_additional_info = persist_read_int(SHOW_BOTTOM_ADDITIONAL_INFO_KEY);
	} else {
		settings.show_bottom_additional_info = 1;
		persist_write_int(SHOW_BOTTOM_ADDITIONAL_INFO_KEY, 1);
	}
	
	if (persist_exists(VIBE_HOURLY_VIBE_KEY)){
		settings.vibe_hourly_vibe = persist_read_int(VIBE_HOURLY_VIBE_KEY);
	} else {
		settings.vibe_hourly_vibe = 1;
		persist_write_int(VIBE_HOURLY_VIBE_KEY, 1);
	}
	
	if (persist_exists(VIBE_BLUETOOTH_STATE_CHANGE_KEY)){
		settings.vibe_bluetooth_state_change = persist_read_int(VIBE_BLUETOOTH_STATE_CHANGE_KEY);
	} else {
		settings.vibe_bluetooth_state_change = 1;
		persist_write_int(VIBE_BLUETOOTH_STATE_CHANGE_KEY, 1);
	}
	
	if (persist_exists(NIGHT_MODE_ENABLED_KEY)){
		settings.night_mode_enabled = persist_read_int(NIGHT_MODE_ENABLED_KEY);
	} else {
		settings.night_mode_enabled = 1;
		persist_write_int(NIGHT_MODE_ENABLED_KEY, 1);
	}
		
	if (persist_exists(NIGHT_MODE_STARTED_KEY)){
		settings.night_mode_started = persist_read_int(NIGHT_MODE_STARTED_KEY);
	} else {
		settings.night_mode_started = 1;
		persist_write_int(NIGHT_MODE_STARTED_KEY, 1);
	}
		
	if (persist_exists(NIGHT_MODE_FINISHED_KEY)){
		settings.night_mode_finished = persist_read_int(NIGHT_MODE_FINISHED_KEY);
	} else {
		settings.night_mode_finished = 1;
		persist_write_int(NIGHT_MODE_FINISHED_KEY, 1);
	}
	
	if (persist_exists(NIGHT_MODE_DISPLAY_INVERT_KEY)){
		settings.night_mode_display_invert = persist_read_int(NIGHT_MODE_DISPLAY_INVERT_KEY);
	} else {
		settings.night_mode_display_invert = 1;
		persist_write_int(NIGHT_MODE_DISPLAY_INVERT_KEY, 1);
	}
	
	if (persist_exists(NIGHT_MODE_UPDATE_INFO_KEY)){
		settings.night_mode_update_info = persist_read_int(NIGHT_MODE_UPDATE_INFO_KEY);
	} else {
		settings.night_mode_update_info = 1;
		persist_write_int(NIGHT_MODE_UPDATE_INFO_KEY, 1);
	}
		
	if (persist_exists(NIGHT_MODE_VIBE_ON_EVENT_KEY)){
		settings.night_mode_vibe_on_event = persist_read_int(NIGHT_MODE_VIBE_ON_EVENT_KEY);
	} else {
		settings.night_mode_vibe_on_event = 1;
		persist_write_int(NIGHT_MODE_VIBE_ON_EVENT_KEY, 1);
	}
	
	if (persist_exists(DATA_UPDATES_FREQUENCY_KEY)){
		settings.data_updates_frequency = persist_read_int(DATA_UPDATES_FREQUENCY_KEY);
	} else {
		settings.data_updates_frequency = 1;
		persist_write_int(DATA_UPDATES_FREQUENCY_KEY, 1);
	}
	
	if (persist_exists(TOP_ADDITIONAL_STRING_TEXT_KEY)){
		persist_read_string(TOP_ADDITIONAL_STRING_TEXT_KEY, top_additional_info_buffer, sizeof(top_additional_info_buffer));
	} else {
		strcpy(top_additional_info_buffer, "Smartface 0.1");
		persist_write_string(TOP_ADDITIONAL_STRING_TEXT_KEY, "Smartface 0.1");
	}
		
	if (persist_exists(BOTTOM_ADDITIONAL_STRING_TEXT_KEY)){
		persist_read_string(BOTTOM_ADDITIONAL_STRING_TEXT_KEY, bottom_additional_info_buffer, sizeof(bottom_additional_info_buffer));
	} else {
		strcpy(bottom_additional_info_buffer, "for timeline");
		persist_write_string(BOTTOM_ADDITIONAL_STRING_TEXT_KEY, "for timeline");
	}
}

void update_time(struct tm* current_time, TimeUnits units_changed){
	
	strftime(time_text_buffer, sizeof(time_text_buffer), "%H:%M", current_time);
	text_layer_set_text(time_text, time_text_buffer);
	
	update_date(current_time, SECOND_UNIT);
	
	if ( (settings.vibe_hourly_vibe == 1) && (!(current_time-> tm_min)) && ( (!is_night() || (settings.night_mode_vibe_on_event))) ){
		vibes_double_pulse();
	}
		
	settings.night_mode_enabled = 1;
	settings.night_mode_started = 22*60 + 3;
	settings.night_mode_finished = 07*60 + 30;
		
	if ( (is_night()) && (settings.night_mode_display_invert) ){
		flags.current_window_color = !settings.window_color;
		set_window_color(!settings.window_color);
	} else {
		flags.current_window_color = settings.window_color;
		set_window_color(settings.window_color);
	}
	
	update_icons();
}

void update_bluetooth_connection(bool is_connected){
	if (settings.vibe_bluetooth_state_change){
		vibes_long_pulse();
	}
	
	gbitmap_destroy(bluetooth_icon); 
	bluetooth_icon = gbitmap_create_with_resource(bluetooth_icons[flags.current_window_color][is_connected]); 
	bitmap_layer_set_bitmap(bluetooth_icon_layer, bluetooth_icon);
	
	if (settings.show_bluetooth_text){
		text_layer_set_text(bluetooth_text, bluetooth_states_names[settings.language][is_connected]);
	} else{
		text_layer_set_text(bluetooth_text, EMPTY_STRING);
	}
	
	flags.is_bluetooth_connected = is_connected;
}

void update_battery_state(BatteryChargeState battery_state){
	gbitmap_destroy(battery_icon); 
	
	if ( (battery_state.is_plugged) && (battery_state.charge_percent == 100)) {
		battery_icon = gbitmap_create_with_resource(battery_icons[flags.current_window_color][11]);
	} else {
		battery_icon = gbitmap_create_with_resource(battery_icons[flags.current_window_color][battery_state.charge_percent / 10]); 
	}
	
	bitmap_layer_set_bitmap(battery_icon_layer, battery_icon);
	
	if (settings.show_battery_text){
		snprintf(battery_text_buffer, sizeof(battery_text_buffer), "%d%%", battery_state.charge_percent);
		text_layer_set_text(battery_text, battery_text_buffer);
	} else {
		text_layer_set_text(battery_text, EMPTY_STRING);
	}
}

void update_date(struct tm* current_time, TimeUnits units_changed){
	text_layer_set_text(weekday_text, weekday_names[settings.language][current_time->tm_wday]);
	
	if (settings.date_format == DD_MM_YYYY_DATE_FORMAT){
		snprintf(date_text_buffer, sizeof(date_text_buffer), "%02d.%02d.%d", current_time->tm_mday, current_time->tm_mon+1, current_time->tm_year + 1900);
	}
	
	if (settings.date_format == DD_MMMM_DATE_FORMAT){
		snprintf(date_text_buffer, sizeof(date_text_buffer), "%02d %s", current_time->tm_mday, month_names[settings.language][current_time->tm_mon]);
	}
	
	text_layer_set_text(date_text, date_text_buffer);
}

void update_additional_info(){
	text_layer_set_text(top_additional_info_text, top_additional_info_buffer);
	text_layer_set_text(bottom_additional_info_text, bottom_additional_info_buffer);
}

void initialization(void) {
	time_t now;
	struct tm *current_time;
	
	persist_write_int(LANGUAGE_KEY, RUSSIAN_LANGUAGE);
	persist_write_int(WINDOW_COLOR_KEY, NORMAL_COLOR);
	persist_write_int(TIME_TEXT_SIZE_KEY, SMALL_TIME_TEXT);
	persist_write_int(DATE_FORMAT_KEY, DD_MMMM_DATE_FORMAT);
	persist_write_int(SHOW_BATTERY_TEXT_KEY, 1);
	persist_write_int(SHOW_BLUETOOTH_TEXT_KEY, 1);
	persist_write_int(SHOW_TOP_ADDITIONAL_INFO_KEY, 1);
	persist_write_int(SHOW_BOTTOM_ADDITIONAL_INFO_KEY, 1);
	persist_write_int(VIBE_HOURLY_VIBE_KEY, 1);
	persist_write_int(VIBE_BLUETOOTH_STATE_CHANGE_KEY, 1);
	persist_write_int(NIGHT_MODE_ENABLED_KEY, 1);
	persist_write_int(NIGHT_MODE_STARTED_KEY, 1);
	persist_write_int(NIGHT_MODE_FINISHED_KEY, 1);
	persist_write_int(DATA_UPDATES_FREQUENCY_KEY, 10);
	persist_write_string(TOP_ADDITIONAL_STRING_TEXT_KEY, "Smartface");
	persist_write_string(BOTTOM_ADDITIONAL_STRING_TEXT_KEY, "timeline");
	
	/*Reading settings*/
	read_persist_settings();

	/*Creating main window*/
	create_window(settings.time_text_size, settings.window_color);
	
	/*Updateing time and date*/
	now = time(NULL);
    current_time = localtime(&now);
    update_time(current_time, MINUTE_UNIT);
	
	/*Showing window*/
	window_stack_push(main_window, true);
	
	/*open connection with a phone*/
	app_message_register_inbox_received(inbox_received_callback);
	app_message_register_inbox_dropped(inbox_dropped_callback);
	app_message_register_outbox_failed(outbox_failed_callback);
	app_message_register_outbox_sent(outbox_sent_callback);
	app_message_open(64, 64);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "SmartFace: application opened!");
	
	/*updating additional info strings*/
	update_additional_info();
	
	/*Subscribing for a timers and events*/
	tick_timer_service_subscribe(MINUTE_UNIT, &update_time);
	bluetooth_connection_service_subscribe(update_bluetooth_connection);
	battery_state_service_subscribe(update_battery_state);
	
	update_bluetooth_connection(connection_service_peek_pebble_app_connection());
	update_battery_state(battery_state_service_peek());
	
	app_timer_register(5000, request_data_from_phone, 0);
}

void deinitialization(void) {
	text_layer_destroy(text_layer);
	window_destroy(main_window);
}

int main(void) {
	initialization();
	app_event_loop();
	deinitialization();
}
