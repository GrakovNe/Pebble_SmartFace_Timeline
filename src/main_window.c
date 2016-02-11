#include <pebble.h>
#include "resources.h"
#include "main_window.h"

Window *main_window;
TextLayer *text_layer;

TextLayer *bluetooth_text;
TextLayer *battery_text;
TextLayer *top_date_text;
TextLayer *time_text;
TextLayer *bottom_date_text;
TextLayer *top_additional_info_text;
TextLayer *bottom_additional_info_text;

BitmapLayer *bluetooth_icon_layer;
BitmapLayer *battery_icon_layer;

GBitmap *bluetooth_icon;
GBitmap *battery_icon;

GFont bluetooth_text_font;
GFont battery_text_font;
GFont time_text_font;
GFont date_text_font;
GFont top_additional_info_text_font;
GFont bottom_additional_info_text_font;

GColor foreground_color;
GColor background_color;

void create_icons(int window_color){
	bluetooth_icon_layer = bitmap_layer_create(GRect(0, 2, 16, 16));
  	layer_add_child(window_get_root_layer(main_window), bitmap_layer_get_layer(bluetooth_icon_layer));
	
	battery_icon_layer = bitmap_layer_create(GRect(127, 2, 16, 16));
  	layer_add_child(window_get_root_layer(main_window), bitmap_layer_get_layer(battery_icon_layer));
}

void create_texts(int text_size, int window_color){
	/*Battery percents*/
	battery_text_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_IMAGINE_FONT_12));
	battery_text = text_layer_create(GRect(16, 2, 112, 16));
	text_layer_set_text_alignment(battery_text, GTextAlignmentRight);
	text_layer_set_font(battery_text, battery_text_font);
	text_layer_set_background_color(battery_text, GColorClear);
	layer_add_child(window_get_root_layer(main_window), text_layer_get_layer(battery_text));	
	
	/*bluetooth text*/
	bluetooth_text_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_IMAGINE_FONT_12));
	bluetooth_text = text_layer_create(GRect(17, 2, 124, 16));
	text_layer_set_text_alignment(bluetooth_text, GTextAlignmentLeft);
	text_layer_set_font(bluetooth_text, bluetooth_text_font);
	text_layer_set_background_color(bluetooth_text, GColorClear);
	layer_add_child(window_get_root_layer(main_window), text_layer_get_layer(bluetooth_text));
	
	/*bottom date text*/
	date_text_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_IMAGINE_FONT_17));
	if (text_size == BIG_TIME_TEXT){
		bottom_date_text = text_layer_create(GRect(0, 29, 145, 30));
	}
	if (text_size == SMALL_TIME_TEXT){
    	bottom_date_text = text_layer_create(GRect(0, 34, 145, 30)); 
	}
	text_layer_set_text_alignment(bottom_date_text, GTextAlignmentCenter);
	text_layer_set_font(bottom_date_text, date_text_font);
	text_layer_set_background_color(bottom_date_text, GColorClear);
	layer_add_child(window_get_root_layer(main_window), text_layer_get_layer(bottom_date_text));
	
	/*time text*/
	if (text_size == BIG_TIME_TEXT){	// large font 
		time_text_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_IMAGINE_BIG_43));
  		time_text = text_layer_create(GRect(2, 38, 145, 50)); //34
		
	}
	if (text_size == SMALL_TIME_TEXT){
		time_text_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_IMAGINE_FONT_37));
  		time_text = text_layer_create(GRect(0, 46, 145, 50)); //34
		text_layer_set_text_alignment(time_text, GTextAlignmentCenter);
		text_layer_set_font(time_text, time_text_font);
		text_layer_set_background_color(time_text, GColorClear);
		layer_add_child(window_get_root_layer(main_window), text_layer_get_layer(time_text));
	}
	
	text_layer_set_text_alignment(time_text, GTextAlignmentCenter);
	text_layer_set_font(time_text, time_text_font);
	text_layer_set_background_color(time_text, GColorClear);
	layer_add_child(window_get_root_layer(main_window), text_layer_get_layer(time_text));
	
	/*current date*/
	if (text_size == BIG_TIME_TEXT){
		top_date_text = text_layer_create(GRect(0, 90, 145, 25)); 
	}
	if (text_size == SMALL_TIME_TEXT){
		top_date_text = text_layer_create(GRect(0, 88, 145, 25)); 
	}
	text_layer_set_text_alignment(top_date_text, GTextAlignmentCenter);
	text_layer_set_font(top_date_text, date_text_font);
	text_layer_set_background_color(top_date_text, GColorClear);
	layer_add_child(window_get_root_layer(main_window), text_layer_get_layer(top_date_text));
	
	/*top additional string*/
	top_additional_info_text_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_IMAGINE_FONT_14));
	top_additional_info_text = text_layer_create(GRect(0, 129, 145, 18));
	text_layer_set_text_alignment(top_additional_info_text, GTextAlignmentCenter);
	text_layer_set_font(top_additional_info_text, top_additional_info_text_font);
	text_layer_set_background_color(top_additional_info_text, GColorClear);
	layer_add_child(window_get_root_layer(main_window), text_layer_get_layer(top_additional_info_text));
	
	/*bottom additional string*/
	bottom_additional_info_text_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_IMAGINE_FONT_14));
	bottom_additional_info_text = text_layer_create(GRect(0, 147, 145, 18));
	text_layer_set_text_alignment(bottom_additional_info_text, GTextAlignmentCenter);
	text_layer_set_font(bottom_additional_info_text, bottom_additional_info_text_font);
	text_layer_set_background_color(bottom_additional_info_text, GColorClear);
	layer_add_child(window_get_root_layer(main_window), text_layer_get_layer(bottom_additional_info_text));
}

void create_window(int text_size, int window_color){
	/*Creating main window*/
	main_window = window_create();
	
	/*Making icons*/
	create_icons(window_color);
	
	/*Making texts*/
	create_texts(text_size, window_color);
	
	/*Applying colors*/
	set_window_color(window_color);
}

void set_window_color(int window_color){
	if (window_color == NORMAL_COLOR){
		foreground_color = GColorBlack;
		background_color = GColorWhite;
	} else {
		foreground_color = GColorWhite;
		background_color = GColorBlack;
	}
	
	window_set_background_color(main_window, background_color);
	
	text_layer_set_text_color(battery_text, foreground_color);
	text_layer_set_text_color(bluetooth_text, foreground_color);
	text_layer_set_text_color(top_date_text, foreground_color);
	text_layer_set_text_color(time_text, foreground_color);
	text_layer_set_text_color(bottom_date_text, foreground_color);
	text_layer_set_text_color(top_additional_info_text, foreground_color);
	text_layer_set_text_color(bottom_additional_info_text, foreground_color);
}

void destroy_window(){
	text_layer_destroy(bluetooth_text);
	text_layer_destroy(battery_text);
	text_layer_destroy(top_date_text);
	text_layer_destroy(time_text);
	text_layer_destroy(bottom_date_text);
	text_layer_destroy(top_additional_info_text);
	text_layer_destroy(bottom_additional_info_text);
	gbitmap_destroy(bluetooth_icon); 
	gbitmap_destroy(battery_icon); 
	bitmap_layer_destroy(bluetooth_icon_layer);
	bitmap_layer_destroy(battery_icon_layer);

	window_destroy(main_window);
}










