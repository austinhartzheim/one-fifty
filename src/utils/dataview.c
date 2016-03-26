#include "dataview.h"

#define ACTIVE_STEP_THRESHOLD 45
#define CDC_CONSECUTIVE_RECOMMEND 10

static char data_display[10];
static char data_longest[12];
static char data_current[12];


void data_view_init(Layer *window_layer) {
  GRect bounds = layer_get_bounds(window_layer);

  // Create title text layer
  tl_title = text_layer_create(GRect(0, bounds.size.h/3 - 38/2 - 26, bounds.size.w, 28));
  text_layer_set_text(tl_title, "Loading");
  text_layer_set_font(tl_title, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(tl_title, GTextAlignmentCenter);

  // Create data text layer
  tl_m_complete = text_layer_create(GRect(0, bounds.size.h/3 - 38/2 - 1, bounds.size.w, 42));
  text_layer_set_text(tl_m_complete, "...");
  text_layer_set_font(tl_m_complete, fonts_get_system_font(FONT_KEY_LECO_38_BOLD_NUMBERS));
  text_layer_set_text_alignment(tl_m_complete, GTextAlignmentCenter);

  // Create subtitle text layer
  tl_sub = text_layer_create(GRect(0, bounds.size.h/3 + 38/2, bounds.size.w, 18));
  text_layer_set_text(tl_sub, "minutes");
  text_layer_set_font(tl_sub, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(tl_sub, GTextAlignmentCenter);

  // Create longest duration text layer
  tl_longest = text_layer_create(GRect(0, bounds.size.h/3*2, bounds.size.w, 22));
  text_layer_set_text(tl_longest, "-");
  text_layer_set_font(tl_longest, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(tl_longest, GTextAlignmentCenter);

  // Create the current duration text layer
  tl_current = text_layer_create(GRect(0, bounds.size.h/3*2 + 24, bounds.size.w, 22));
  text_layer_set_text(tl_current, "-");
  text_layer_set_font(tl_current, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(tl_current, GTextAlignmentCenter);

  // Add text layers to window
  layer_add_child(window_layer, text_layer_get_layer(tl_m_complete));
  layer_add_child(window_layer, text_layer_get_layer(tl_sub));
  layer_add_child(window_layer, text_layer_get_layer(tl_title));
  layer_add_child(window_layer, text_layer_get_layer(tl_longest));
  layer_add_child(window_layer, text_layer_get_layer(tl_current));
}

// Count the number of minutes of walking that qualify as part of a
// ten-minute exercise window.
// Takes the start and end times to check as parameters.
// Return -1 if malloc fails.
// Return the total number of qualifying minutes.
int calculate_qualifying_minutes(activity_summary_t *summary, time_t start, time_t stop) {
  HealthMinuteData *minute_data;
  int num_records;

  minute_data = (HealthMinuteData*)malloc(MINUTES_PER_DAY*sizeof(HealthMinuteData));
  if (minute_data == NULL) {
    free(minute_data);
    return -1;
  }
  num_records = health_service_get_minute_history(minute_data, MINUTES_PER_DAY, &start, &stop);
  if (num_records == 0) {
    free(minute_data);
    return 0;
  }
  
  int consecutive = 0;
  int total_minutes = 0;
  for (int i = 0; i < num_records; i++) {
    if ((int)minute_data[i].steps > ACTIVE_STEP_THRESHOLD) {
      consecutive++;
    } else {
      if (consecutive >= CDC_CONSECUTIVE_RECOMMEND) {
        total_minutes += consecutive;
      }
      if (consecutive > 0)
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Found %d consecutive minutes.", consecutive);
      if (consecutive > summary->max_consecutive)
        summary->max_consecutive = consecutive;
      consecutive = 0;
    }
  }

  free(minute_data);
  summary->total_minutes = total_minutes;
  summary->current = consecutive;
  return 0;
}


void show_data_today() {
  activity_summary_t summary;
  summary.max_consecutive = 0;
  
  calculate_qualifying_minutes(&summary, time_start_of_today(), time(NULL));

  // Update text buffers
  snprintf(data_display, 10, "%d/30", summary.total_minutes);
  snprintf(data_longest, 12, "Longest: %d", summary.max_consecutive);
  snprintf(data_current, 12, "Current: %d", summary.current);

  // Set text layers
  text_layer_set_text(tl_title, "Today");
  text_layer_set_text(tl_m_complete, data_display);
  text_layer_set_text(tl_longest, data_longest);
  text_layer_set_text(tl_current, data_current);
}

void show_data_week() {
  // text_layer_set_text(tl_title, "Week");

}
