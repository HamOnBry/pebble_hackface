#include <pebble.h>

Window *main_window;
TextLayer *time_layer;
TextLayer *date_layer;
TextLayer *weather_layer;
GFont *time_font;

static void update_time() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  static char time_buffer[] = "00:00";
  static char date_buffer[] = "00/00";
  strftime(date_buffer, sizeof("00/00"), "%m/%d", tick_time);
  strftime(time_buffer, sizeof("00:00"), "%H:%M", tick_time);
  
  text_layer_set_text(date_layer, date_buffer);
  text_layer_set_text(time_layer, time_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time();
}

static void main_window_load(Window *window) {
  //create time layer
  time_layer = text_layer_create(GRect(0, 52, 144, 65));
  text_layer_set_background_color(time_layer, GColorBlack); //set background to black
  text_layer_set_text_color(time_layer, GColorWhite); //set text to white
  text_layer_set_text_alignment(time_layer, GTextAlignmentCenter); //center time on layer
  text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49)); //change font
  update_time();

  //create date layer
  date_layer = text_layer_create(GRect(0, 10, 100, 42));
  text_layer_set_background_color(date_layer, GColorBlack);
  text_layer_set_text_color(date_layer, GColorWhite);
  text_layer_set_font(date_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  //text_layer_set_text(date_layer, "10/30");
  
  //create weather layer
  weather_layer = text_layer_create(GRect(0, 120, 144, 40));
  text_layer_set_background_color(weather_layer, GColorBlack);
  text_layer_set_text_color(weather_layer, GColorWhite);
  text_layer_set_font(weather_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  text_layer_set_text(weather_layer, "15 C");
  
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(date_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(time_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(weather_layer));
}

static void main_window_unload(Window *window) {
  text_layer_destroy(time_layer);
}

void init(void) {
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  main_window = window_create();
  window_set_window_handlers(main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  window_stack_push(main_window, true);
}

void deinit(void) {
  window_destroy(main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
