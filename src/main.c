#include <pebble.h>

enum {
  KEY_TEMPERATURE = 0,
  KEY_CONDITIONS = 1
};

Window *main_window;
TextLayer *time_layer;
TextLayer *date_layer;
TextLayer *weather_layer;
Layer *weather_icon;
GFont *font_time;
GFont *font_date;
GFont font_weather;


static void inbox_received_callback(DictionaryIterator *iterator, void *context) {

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
  font_time = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_COUSINE_REGULAR_44));
  font_date = fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21);
  font_weather = fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21);
  //create time layer
  time_layer = text_layer_create(GRect(0, 52, 144, 65));
  text_layer_set_background_color(time_layer, GColorBlack); //set background to black
  text_layer_set_text_color(time_layer, GColorWhite); //set text to white
  text_layer_set_text_alignment(time_layer, GTextAlignmentCenter); //center time on layer
  text_layer_set_font(time_layer, font_time); //change font
  update_time();

  //create date layer
  date_layer = text_layer_create(GRect(0, 10, 100, 42));
  text_layer_set_background_color(date_layer, GColorBlack);
  text_layer_set_text_color(date_layer, GColorWhite);
  text_layer_set_font(date_layer, font_date);
  //text_layer_set_text(date_layer, "10/30");
  
  //create weather layer
  weather_layer = text_layer_create(GRect(0, 120, 144, 40));
  text_layer_set_background_color(weather_layer, GColorBlack);
  text_layer_set_text_color(weather_layer, GColorWhite);
  text_layer_set_font(weather_layer, font_weather);
  text_layer_set_text(weather_layer, "Updating...");

  
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(date_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(time_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(weather_layer));
}

static void main_window_unload(Window *window) {
  text_layer_destroy(time_layer);
  text_layer_destroy(date_layer);
  text_layer_destroy(weather_layer);
  fonts_unload_custom_font(font_time);
  
}

void init(void) {
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  main_window = window_create();
  window_set_background_color(main_window, GColorWhite);
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
