#include <pebble.h>
static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
// Declare font globally
//static GFont s_time_font;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

static void main_window_load(Window* window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Create the TextLayer with specific bounds
  s_time_layer = text_layer_create(
    GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 50));
  s_date_layer = text_layer_create(
    GRect(0, PBL_IF_ROUND_ELSE(88, 82), bounds.size.w, 50));

  // Create GFont
  //s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_48));

  // Create GBitmap
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);

  // Create BitmapLayer to display the GBitmap
  s_background_layer = bitmap_layer_create(bounds);

  // Set the bitmap onto the layer and add to the window
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));

  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  //text_layer_set_text(s_time_layer, "Hello world");
  //text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_LECO_28_LIGHT_NUMBERS));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, GColorBlack);
  text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_LECO_28_LIGHT_NUMBERS));
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
}

static void main_window_unload(Window* window) {
  // Destroy TextLayers
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_date_layer);

  // Unload GFont
  //fonts_unload_custom_font(s_time_font);

  // Destroy GBitmap
  //gbitmap_destroy(s_background_bitmap);

  // Destroy BitmapLayer
  bitmap_layer_destroy(s_background_layer);
}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[14];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);

  // Write date into A DIFFERENT buffer
  static char s_buffer2[14];
  char* p = s_buffer2;
  strftime(s_buffer2, sizeof("mm/dd"), "%m/%d", tick_time);

  if (s_buffer2[0] == '0') {
    p += 1;
  }
  if (p[2] == '0') {
    p[2] = p[3];
    p[3] = '\0';
  } else if (p[2] == '/' && p[3] == '0') {
    p[3] = p[4];
    p[4] = '\0';
  }

  // Display date on the DateLayer
  text_layer_set_text(s_date_layer, p);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);

  // Set background color of Window to match background image
  //window_set_background_color(s_main_window, GColorWhite);

  // Make sure the time is displayed from the start
  update_time();
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main() {
  init();
  app_event_loop();
  deinit();
}
