#pragma once

#include <pebble.h>

typedef enum {
  MODE_TODAY, MODE_WEEK
} view_t;

typedef struct _activity_summary {
  int total_minutes;
  int max_consecutive;
  int current;
} activity_summary_t;

TextLayer *tl_m_complete;
TextLayer *tl_title;
TextLayer *tl_sub;
TextLayer *tl_longest;
TextLayer *tl_current;

void data_view_init(Layer *);
int calculate_qualifying_minutes(activity_summary_t *, time_t, time_t);
void show_data_week();
void show_data_today();
