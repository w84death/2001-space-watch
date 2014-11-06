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

#include <pebble.h>
#include <watchface.h>

#define LAST_SEED_TIMESTAMP 1
static char the_time[] = "00:00";
static char date_text[] = "00 Xxxxxxxxx 0000";
static char day_num[] = "000";
static bool refresh_date = true;
static bool refresh_seed = true;

void tick_handler(struct tm *tick_time, TimeUnits units_changed){
	strftime(the_time, sizeof(the_time),clock_is_24h_style()?"%H:%M":"%I:%M", tick_time);
	strftime(date_text, sizeof(date_text), "%d %B %y", tick_time);
	strftime(day_num, sizeof(day_num), "%j", tick_time);
	
	// SPACE SEED RANDOMIZER
	if(units_changed & HOUR_UNIT){	
	//if(units_changed & MINUTE_UNIT){	
		refresh_seed = true;
	}
	
	if(refresh_seed){
		hyperspace();
		refresh_seed = false;
	}
	
	// DATE
	if(units_changed & DAY_UNIT){
		refresh_date = true;
	}
	
	if(refresh_date){
		show_date(date_text, day_num);		
		refresh_date = false;
	}
	
	show_time(the_time);
}

void batteryChanged(BatteryChargeState batt) {	
	update_battery(batt.charge_percent);
}

void handle_init(void) {
	show_watchface();
	
	// set time
	time_t now = time(NULL);
  struct tm *current_time = localtime(&now);
  tick_handler(current_time, SECOND_UNIT);

	// set battery
	BatteryChargeState batt = battery_state_service_peek();
	update_battery(batt.charge_percent);
	
	// subscribe time & battery
	tick_timer_service_subscribe(MINUTE_UNIT, (TickHandler)tick_handler);
	battery_state_service_subscribe(batteryChanged);
}

void handle_deinit(void) {
  hide_watchface();
	tick_timer_service_unsubscribe();
	battery_state_service_unsubscribe();
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
