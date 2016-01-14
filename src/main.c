#include <pebble.h>
#include "main_window.h"
#include "resources.h"

extern Window *main_window;
extern TextLayer *text_layer;

struct Settings{
	int language;
	int window_color;
	int time_text_size;
	int date_format;
	int show_battery_text;
	int show_bluettoth_text;
	int show_top_additional_info;
	int show_bottom_additional_info;
} settings;

char top_additional_info_buffer    [36];
char bottom_additional_info_buffer [36];

void initialization(void) {
	
	/*applying settings*/
	settings.language = ENGLISH_LANGUAGE;
	settings.window_color = NORMAL_COLOR;
	settings.date_format = DD_MM_YYYY_DATE_FORMAT;
	settings.show_battery_text = 1;
	settings.show_bluettoth_text = 1;
	settings.show_top_additional_info = 1;
	settings.show_bottom_additional_info = 1;

	create_window(settings.time_text_size, settings.window_color);
	
	window_stack_push(main_window, true);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "SmartFace: application opened!");
}

void deinitialization(void) {
	// Destroy the text layer
	text_layer_destroy(text_layer);
	
	// Destroy the window
	window_destroy(main_window);
}

int main(void) {
	initialization();
	app_event_loop();
	deinitialization();
}
