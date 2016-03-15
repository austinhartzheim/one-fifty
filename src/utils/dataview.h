#pragma once

#include <pebble.h>

typedef enum {
  MODE_TODAY, MODE_WEEK
} view_t;

TextLayer *tl_m_complete;
TextLayer *tl_title;
TextLayer *tl_sub;

void data_view_init(Layer *);
int calculate_qualifying_minutes(time_t, time_t);
void show_data_week();
void show_data_today();
