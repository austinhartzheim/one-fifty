#include <pebble.h>
#include "utils/dataview.h"

static view_t view_mode = MODE_TODAY;

static Window *window_today;

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  switch (view_mode) {
  case MODE_TODAY:
    view_mode = MODE_WEEK;
    show_data_week();
    break;
  case MODE_WEEK:
    view_mode = MODE_TODAY;
    show_data_today();
    break;
  default:
    view_mode = MODE_TODAY;
  }
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void health_handler(HealthEventType event, void *context) {
  switch (view_mode) {
  case MODE_WEEK:
    show_data_week();
    break;
  case MODE_TODAY:
  default:
    view_mode = MODE_TODAY;
    show_data_today();
  }
  /*  switch(event) {
  case HealthEventSignificantUpdate:
    health_read();
    break;
  case HealthEventMovementUpdate:
    health_read();
    break;
  default:
    break;
    }*/
}

static void window_today_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  data_view_init(window_layer);
}

static void window_today_unload(Window *window) {
  text_layer_destroy(tl_m_complete);
  text_layer_destroy(tl_sub);
}

static void init(void) {
  window_today = window_create();
  window_set_click_config_provider(window_today, click_config_provider);
  window_set_window_handlers(window_today, (WindowHandlers) {
    .load = window_today_load,
    .unload = window_today_unload,
  });
  const bool animated = true;
  window_stack_push(window_today, animated);

  // Subscribe to health data updates
  if(!health_service_events_subscribe(health_handler, NULL)) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Health not available!");
  }

  show_data_today();
}

static void deinit(void) {
  window_destroy(window_today);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window_today);

  app_event_loop();
  deinit();
}
