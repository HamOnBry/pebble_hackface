#include <pebble.h>
#include "main.h"

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_battery_layer;
static int s_battery_level;

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
                                          "%H:%M" : "%I:%M", tick_time);

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void battery_callback(BatteryChargeState state) {
  s_battery_level = state.charge_percent;
}

static void battery_update_proc(Layer *layer, GContext *ctx) {
  
}

static void main_window_load(Window *window) {
  GFont font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_COUSINE_BOLD_44));
  GRect bounds = layer_get_bounds(window_get_root_layer(window));

  s_time_layer = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 52));
  s_battery_layer = text_layer_create(GRect(10, 100, 20, 20));
  
  text_layer_set_background_color(s_battery_layer, GColorBlack);
  layer_set_update_proc(s_battery_layer, battery_update_proc);
  
  text_layer_set_background_color(s_time_layer, GColorBlack);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_font(s_time_layer, font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_battery_layer));
}

static void main_window_unload(Window *window) {
  text_layer_destroy(s_time_layer);
}

static void init() {

  
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  battery_state_service_subscribe(battery_callback);
  
  update_time();

  
}

static void deinit() {
  window_destroy(s_main_window);  
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}