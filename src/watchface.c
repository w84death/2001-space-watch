/*
*
*
*   P1X, Krzysztof Jankowski
*   2001: Space Watch
*
*   abstract: Watch face for Pebble with procedural space background
*   created: 04-11-2014
*   license: do what you want and dont bother me
*
*   webpage: http://p1x.in
*   twitter: @w84death
*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "watchface.h"
#include <pebble.h>

#define MAX_WIDTH 144
#define MAX_HEIGHT 137
#define MIN_STARS_SMALL 8
#define MAX_STARS_SMALL 24
#define MAX_STARS_MEDIUM 6
#define MAX_STARS_BIG 4
#define MAX_ASTEROIDS_SMALL 3
#define MAX_ASTEROIDS_MEDIUM  2
#define MAX_ASTEROIDS_BIG 2

static Window *s_window;
static GBitmap *s_res_footer;
static GBitmap *s_res_frame;
static GFont s_res_gothic_14;
static GFont s_res_gothic_18;
static GFont s_res_gothic_28;
static GBitmap *s_res_logo;
static BitmapLayer *layer_footer;
static BitmapLayer *layer_frame;
static TextLayer *label_time;
static TextLayer *label_date;
static TextLayer *label_batt;
static TextLayer *label_day;
static BitmapLayer *layer_logo;
static char battery_level[] = "000%";
static Layer *layer_space;
static int seed;
static GBitmap *sprite_star_medium, *sprite_star_big_1, *sprite_star_big_2, *sprite_asteroid_small, *sprite_asteroid_medium, *sprite_asteroid_big;

void show_time(char *time){
	text_layer_set_text(label_time, time);
}

void show_date(char *date,char *day){
	text_layer_set_text(label_date, date);
	text_layer_set_text(label_day, day);
	seed = *day - 0;
}

void update_battery(int level){
	snprintf(battery_level, sizeof(battery_level), "%i%%", level);
	text_layer_set_text(label_batt, battery_level);
}

static void space_update_callback(Layer *me, GContext* ctx) {
	/*
	
		DEEP SPACE PROCEDURAL GENERATION
		work-in-progress
	
	*/
	graphics_context_set_compositing_mode(ctx,GCompOpOr);
  GRect bounds_star_medium = sprite_star_medium->bounds;
	GRect bounds_star_big_1 = sprite_star_big_1->bounds;
	GRect bounds_star_big_2 = sprite_star_big_2->bounds;
	GRect bounds_asteroid_small = sprite_asteroid_small->bounds;
	GRect bounds_asteroid_medium = sprite_asteroid_medium->bounds;
	GRect bounds_asteroid_big = sprite_asteroid_big->bounds;
	int x, y, i;
	
	srand(seed);
		
	// STARTS
	graphics_context_set_stroke_color(ctx, GColorWhite);
	
	for(i=0; i<MIN_STARS_SMALL + rand()%(MAX_STARS_SMALL-MIN_STARS_SMALL); i++){
		x = rand() % MAX_WIDTH;
		y = rand() % MAX_HEIGHT;
		graphics_draw_pixel(ctx, (GPoint){x,y});		
	}
		
	for(i=0; i<rand()%MAX_STARS_MEDIUM; i++){
		x = rand() % MAX_WIDTH;
		y = rand() % MAX_HEIGHT;
		graphics_draw_bitmap_in_rect(ctx, sprite_star_medium, (GRect) { 
			.origin = { x, y }, 
				.size = bounds_star_medium.size
		});		
	}
	
	for(i=0; i<rand()%MAX_STARS_BIG;i++){
		x = rand() % MAX_WIDTH;
		y = rand() % MAX_HEIGHT;
		if(rand()%10 > 4){
			graphics_draw_bitmap_in_rect(ctx, sprite_star_big_1, (GRect) { 
				.origin = { x, y }, 
					.size = bounds_star_big_1.size
			});
		}else{
			graphics_draw_bitmap_in_rect(ctx, sprite_star_big_2, (GRect) { 
				.origin = { x, y }, 
					.size = bounds_star_big_2.size
			});
		}
	}
	
	
	// ASTEROIDS
	
	for(i=0; i<rand()%MAX_ASTEROIDS_SMALL; i++){
		x = rand() % MAX_WIDTH;
		y = rand() % MAX_HEIGHT;
		graphics_draw_bitmap_in_rect(ctx, sprite_asteroid_small, (GRect) { 
			.origin = { x, y }, 
				.size = bounds_asteroid_small.size
		});	
	}
	
	for(i=0; i<rand()%MAX_ASTEROIDS_MEDIUM; i++){
		x = rand() % MAX_WIDTH;
		y = rand() % MAX_HEIGHT;
		graphics_draw_bitmap_in_rect(ctx, sprite_asteroid_medium, (GRect) { 
			.origin = { x, y }, 
				.size = bounds_asteroid_medium.size
		});	
	}
	
	for(i=0; i<rand()%MAX_ASTEROIDS_BIG; i++){
		x = rand() % MAX_WIDTH;
		y = rand() % MAX_HEIGHT;
		graphics_draw_bitmap_in_rect(ctx, sprite_asteroid_big, (GRect) { 
			.origin = { x, y }, 
				.size = bounds_asteroid_big.size
		});	
	}
	
	// CUSTOM EVENTS
	
}

static void initialise_ui(void) {
  s_window = window_create();
  window_set_background_color(s_window, GColorBlack);
  window_set_fullscreen(s_window, true);
  
  s_res_footer = gbitmap_create_with_resource(RESOURCE_ID_FOOTER);
  s_res_frame = gbitmap_create_with_resource(RESOURCE_ID_FRAME);
  s_res_gothic_14 = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  s_res_gothic_18 = fonts_get_system_font(FONT_KEY_GOTHIC_18);
	s_res_gothic_28 = fonts_get_system_font(FONT_KEY_GOTHIC_28);
  s_res_logo = gbitmap_create_with_resource(RESOURCE_ID_LOGO);
	
  // layer_footer
  layer_footer = bitmap_layer_create(GRect(0, 138, 144, 30));
  bitmap_layer_set_bitmap(layer_footer, s_res_footer);
  layer_add_child(window_get_root_layer(s_window), (Layer *)layer_footer);
  
  // layer_space
  layer_space = layer_create(GRect(0, 0, 144, 137));
  layer_add_child(window_get_root_layer(s_window), (Layer *)layer_space);
	layer_set_update_proc(layer_space, space_update_callback);
  
  // layer_frame
  layer_frame = bitmap_layer_create(GRect(0, 0, 144, 137));
	bitmap_layer_set_background_color(layer_frame, GColorClear);
  bitmap_layer_set_compositing_mode(layer_frame, GCompOpOr);
  bitmap_layer_set_bitmap(layer_frame, s_res_frame);
  layer_add_child(window_get_root_layer(s_window), (Layer *)layer_frame);
  
	// layer_time
	label_time = text_layer_create(GRect(42, 133, 61, 28));
  text_layer_set_background_color(label_time, GColorClear);
  text_layer_set_text(label_time, "00:00");
  text_layer_set_text_alignment(label_time, GTextAlignmentCenter);
  text_layer_set_font(label_time, s_res_gothic_28);
  layer_add_child(window_get_root_layer(s_window), (Layer *)label_time);
	
  // label_date
  label_date = text_layer_create(GRect(0, 114, 144, 14));
  text_layer_set_background_color(label_date, GColorClear);
  text_layer_set_text_color(label_date, GColorWhite);
  text_layer_set_text(label_date, "03 November 2014");
  text_layer_set_text_alignment(label_date, GTextAlignmentCenter);
  text_layer_set_font(label_date, s_res_gothic_14);
  layer_add_child(window_get_root_layer(s_window), (Layer *)label_date);
  
  // label_batt
  label_batt = text_layer_create(GRect(3, 140, 38, 18));
  text_layer_set_background_color(label_batt, GColorClear);
  text_layer_set_text(label_batt, "100%");
  text_layer_set_text_alignment(label_batt, GTextAlignmentCenter);
  text_layer_set_font(label_batt, s_res_gothic_18);
  layer_add_child(window_get_root_layer(s_window), (Layer *)label_batt);
  
  // label_day
  label_day = text_layer_create(GRect(104, 140, 38, 18));
  text_layer_set_background_color(label_day, GColorClear);
  text_layer_set_text(label_day, "0");
  text_layer_set_text_alignment(label_day, GTextAlignmentCenter);
  text_layer_set_font(label_day, s_res_gothic_18);
  layer_add_child(window_get_root_layer(s_window), (Layer *)label_day);
  
  // layer_logo
  layer_logo = bitmap_layer_create(GRect(14, 14, 15, 18));
  bitmap_layer_set_bitmap(layer_logo, s_res_logo);
  layer_add_child(window_get_root_layer(s_window), (Layer *)layer_logo);
	
	// sprites
	sprite_star_medium = gbitmap_create_with_resource(RESOURCE_ID_STAR_MEDIUM); 
	sprite_star_big_1 = gbitmap_create_with_resource(RESOURCE_ID_STAR_BIG_1); 
	sprite_star_big_2  = gbitmap_create_with_resource(RESOURCE_ID_STAR_BIG_2); 
	sprite_asteroid_small = gbitmap_create_with_resource(RESOURCE_ID_ASTEROID_SMALL); 
	sprite_asteroid_medium = gbitmap_create_with_resource(RESOURCE_ID_ASTEROID_MEDIUM); 
	sprite_asteroid_big = gbitmap_create_with_resource(RESOURCE_ID_ASTEROID_BIG); 
}

static void destroy_ui(void) {
  window_destroy(s_window);
  bitmap_layer_destroy(layer_footer);
  layer_destroy(layer_space);
  bitmap_layer_destroy(layer_frame);
	text_layer_destroy(label_time);
  text_layer_destroy(label_date);
  text_layer_destroy(label_batt);
  text_layer_destroy(label_day);
  bitmap_layer_destroy(layer_logo);
  gbitmap_destroy(s_res_footer);
  gbitmap_destroy(s_res_frame);
  gbitmap_destroy(s_res_logo);
	gbitmap_destroy(sprite_star_medium);
	gbitmap_destroy(sprite_star_big_1);
	gbitmap_destroy(sprite_star_big_2);
	gbitmap_destroy(sprite_asteroid_small);
	gbitmap_destroy(sprite_asteroid_medium);
	gbitmap_destroy(sprite_asteroid_big);
}

static void handle_window_unload(Window* window) {
  destroy_ui();
}

void show_watchface(void) {
  initialise_ui();
	
	layer_mark_dirty(layer_space);
	
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void hide_watchface(void) {
  window_stack_remove(s_window, true);
}
