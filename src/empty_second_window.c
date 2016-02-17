#include <pebble.h>
#include "empty_second_window.h"
#include "resources.h"

Window *empty_second_window;
TextLayer *text_label;
GFont text_font;

GColor foreground_color;
GColor background_color;

void create_second_window_empty(int window_color){
	text_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_IMAGINE_FONT_23));
	empty_second_window = window_create();
	text_label = text_layer_create(GRect(0, 55, 145, 50));
	text_layer_set_text_alignment(text_label, GTextAlignmentCenter);
	text_layer_set_font(text_label, text_font);
	text_layer_set_background_color(text_label, GColorClear);
	layer_add_child(window_get_root_layer(empty_second_window), text_layer_get_layer(text_label));
}

void set_second_window_empty_colors(int window_color){
	
	if (window_color == NORMAL_COLOR){
		foreground_color = GColorBlack;
		background_color = GColorWhite;
	} else {
		foreground_color = GColorWhite;
		background_color = GColorBlack;
	}
	
	text_layer_set_text_color(text_label, foreground_color);
	window_set_background_color(empty_second_window, background_color);
}

void update_second_window_empty(){
	text_layer_set_text(text_label, "Second Screen!");
}


