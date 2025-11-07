#include <Arduino.h>
#include <string>
#include <pin_config.h>
#include <lvgl.h>

#define BUF_SIZE 320 * 50
uint8_t lv_buffer[BUF_SIZE];

typedef struct {
    lv_obj_t *scale;
    lv_obj_t *needle_line;
    lv_obj_t *scale_value_label;
    lv_obj_t *scale_unit_label;
} scale_widget_t;

static scale_widget_t cpuTempWidget;
static scale_widget_t cpuLoadWidget;
static scale_widget_t gpuTempWidget;

static int32_t init_scale_value = 0;

typedef struct {
    lv_style_t items;
    lv_style_t indicator;
    lv_style_t main;
} section_styles_t;

static section_styles_t zone1_styles;
static section_styles_t zone2_styles;
static section_styles_t zone3_styles;
static section_styles_t zone4_styles;
static section_styles_t zone5_styles;

static int32_t zone1_temperature =  35;
static int32_t zone2_temperature =  55;
static int32_t zone3_temperature =  75;
static int32_t zone4_temperature = 100;

static int32_t zone1_load =  40;
static int32_t zone2_load =  60;
static int32_t zone3_load =  80;
static int32_t zone4_load = 100;

static int32_t scale_temperature_min = 20;
static int32_t scale_temperature_max = 100;
static std::string scale_temperature_unit = "°C";

static int32_t scale_load_min = 0;
static int32_t scale_load_max = 100;
static std::string scale_load_unit = "%";

static lv_color_t zone1_color = lv_palette_main(LV_PALETTE_BLUE);
static lv_color_t zone2_color = lv_palette_main(LV_PALETTE_GREEN);
static lv_color_t zone3_color = lv_palette_main(LV_PALETTE_ORANGE);
static lv_color_t zone4_color = lv_palette_main(LV_PALETTE_RED);

static lv_color_t needle_color = lv_palette_main(LV_PALETTE_YELLOW);

enum scale_type_t {
    SCALE_TYPE_LOAD,
    SCALE_TYPE_TEMPERATURE
};

/* Tick source, tell LVGL how much time (milliseconds) has passed */
static uint32_t my_tick(void)
{
    return millis();
}

static lv_color_t get_scale_zone_color(int32_t value, enum scale_type_t type)
{
    if(type == SCALE_TYPE_LOAD) {
        if(value < zone1_load) return zone1_color;
        else if(value < zone2_load) return zone2_color;
        else if(value < zone3_load) return zone3_color;
        else return zone4_color;
    } else {
        if(value < zone1_temperature) return zone1_color;
        else if(value < zone2_temperature) return zone2_color;
        else if(value < zone3_temperature) return zone3_color;
        else return zone4_color;
    }
}

static void set_scale_value(scale_widget_t *widget, enum scale_type_t scale_type, int32_t value)
{
    /* Update needle */
    lv_scale_set_line_needle_value(widget->scale, widget->needle_line, -8, value);

    /* Update value text */
    lv_label_set_text_fmt(widget->scale_value_label, "%d", value);

    /* Update text color based on zone */
    lv_color_t zone_color = get_scale_zone_color(value, scale_type);
    lv_obj_set_style_text_color(widget->scale_value_label, zone_color, 0);
    lv_obj_set_style_text_color(widget->scale_unit_label, zone_color, 0);
}

static void init_section_styles(section_styles_t * styles, lv_color_t color)
{
    lv_style_init(&styles->items);
    lv_style_set_line_color(&styles->items, color);
    lv_style_set_line_width(&styles->items, 0);

    lv_style_init(&styles->indicator);
    lv_style_set_line_color(&styles->indicator, color);
    lv_style_set_line_width(&styles->indicator, 0);

    lv_style_init(&styles->main);
    lv_style_set_arc_color(&styles->main, color);
    lv_style_set_arc_width(&styles->main, 10);
}

static void add_section(lv_obj_t * target_scale, int32_t from, int32_t to, const section_styles_t * styles)
{
    lv_scale_section_t * sec = lv_scale_add_section(target_scale);
    lv_scale_set_section_range(target_scale, sec, from, to);
    lv_scale_set_section_style_items(target_scale, sec, &styles->items);
    lv_scale_set_section_style_indicator(target_scale, sec, &styles->indicator);
    lv_scale_set_section_style_main(target_scale, sec, &styles->main);
}

scale_widget_t create_scale(scale_type_t scale_type, lv_align_t scale_align)
{
    int32_t scale_min;
    int32_t scale_max;

    int32_t scale_zone1_end;
    int32_t scale_zone2_end;
    int32_t scale_zone3_end;
    int32_t scale_zone4_end; 

    int32_t scale_offset;
    int32_t circle_offset;

    std::string scale_unit;    

    if(scale_type == scale_type_t::SCALE_TYPE_LOAD) {
        scale_min = scale_load_min;
        scale_max = scale_load_max;
        scale_zone1_end = zone1_load;
        scale_zone2_end = zone2_load;
        scale_zone3_end = zone3_load;
        scale_zone4_end = zone4_load;
        scale_unit = scale_load_unit;
    } else if (scale_type == scale_type_t::SCALE_TYPE_TEMPERATURE) {
        scale_min = scale_temperature_min;
        scale_max = scale_temperature_max;
        scale_zone1_end = zone1_temperature;
        scale_zone2_end = zone2_temperature;
        scale_zone3_end = zone3_temperature;
        scale_zone4_end = zone4_temperature;
        scale_unit = scale_temperature_unit;
    }

    if (scale_align == LV_ALIGN_LEFT_MID) {
        scale_offset = 8;
        circle_offset = 21;
    } else if (scale_align == LV_ALIGN_RIGHT_MID) {
        scale_offset = -8;
        circle_offset = -21;
    } else if (scale_align == LV_ALIGN_CENTER) {
        scale_offset = 0;
        circle_offset = 0;
    }

    lv_obj_t * scale = lv_scale_create(lv_screen_active());
    lv_obj_center(scale);
    lv_obj_set_size(scale, 76, 76);

    lv_scale_set_mode(scale, LV_SCALE_MODE_ROUND_INNER);
    lv_scale_set_range(scale, scale_min, scale_max);
    lv_scale_set_total_tick_count(scale, 15);
    lv_scale_set_major_tick_every(scale, 3);
    lv_scale_set_angle_range(scale, 270);
    lv_scale_set_rotation(scale, 135);
    lv_scale_set_label_show(scale, false);

    lv_obj_set_style_length(scale, 3, LV_PART_ITEMS);
    lv_obj_set_style_length(scale, 10, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(scale, 0, LV_PART_MAIN);

    /* Zone 1: (Blue) */
    init_section_styles(&zone1_styles, zone1_color);
    add_section(scale, scale_min, scale_zone1_end, &zone1_styles);

    /* Zone 2: (Green) */
    init_section_styles(&zone2_styles, zone2_color);
    add_section(scale, scale_zone1_end, scale_zone2_end, &zone2_styles);

    /* Zone 3: (Orange) */
    init_section_styles(&zone3_styles, zone3_color);
    add_section(scale, scale_zone2_end, scale_zone3_end, &zone3_styles);

    /* Zone 4: (Red) */
    init_section_styles(&zone4_styles, zone4_color);
    add_section(scale, scale_zone3_end, scale_zone4_end, &zone4_styles);

    lv_obj_t *needle_line = lv_line_create(scale);

    /* Optional styling */
    lv_obj_set_style_line_color(needle_line, needle_color, LV_PART_MAIN);
    lv_obj_set_style_line_width(needle_line, 4, LV_PART_MAIN);
    lv_obj_set_style_length(needle_line, 10, LV_PART_MAIN);
    lv_obj_set_style_line_rounded(needle_line, true, LV_PART_MAIN);
    lv_obj_set_style_pad_right(needle_line, 20, LV_PART_MAIN);

    int32_t current_value = init_scale_value;

    lv_scale_set_line_needle_value(scale, needle_line, 32, current_value);

    lv_obj_t *circle = lv_obj_create(lv_screen_active());
    lv_obj_set_size(circle, 52, 52);
    lv_obj_align(circle, scale_align, circle_offset, 0);
    lv_obj_set_style_radius(circle, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(circle, lv_obj_get_style_bg_color(lv_screen_active(), LV_PART_MAIN), 0);
    lv_obj_set_style_bg_opa(circle, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(circle, 0, LV_PART_MAIN);
    lv_obj_set_scrollbar_mode(circle, LV_SCROLLBAR_MODE_OFF);

    lv_obj_t *hr_container = lv_obj_create(circle);
    lv_obj_center(hr_container);
    lv_obj_set_size(hr_container, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(hr_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(hr_container, 0, 0);
    lv_obj_set_layout(hr_container, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(hr_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(hr_container, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_row(hr_container, 0, 0);
    lv_obj_set_flex_align(hr_container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    
    lv_obj_t *scale_value_label = lv_label_create(hr_container);
    lv_label_set_text_fmt(scale_value_label, "%d", current_value);
    lv_obj_set_style_text_font(scale_value_label, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_align(scale_value_label, LV_TEXT_ALIGN_CENTER, 0);

    lv_obj_t *scale_unit_label = lv_label_create(hr_container);
    lv_label_set_text(scale_unit_label, scale_unit.c_str());
    lv_obj_set_style_text_font(scale_unit_label, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_align(scale_unit_label, LV_TEXT_ALIGN_CENTER, 0);

    lv_color_t zone_color = get_scale_zone_color(current_value, scale_type);
    lv_obj_set_style_text_color(scale_value_label, zone_color, 0);
    lv_obj_set_style_text_color(scale_unit_label, zone_color, 0);

    scale_widget_t widget = {
        .scale = scale,
        .needle_line = needle_line,
        .scale_value_label = scale_value_label,
        .scale_unit_label = scale_unit_label
    };
    return widget;
}

void setup() {

    Serial.begin(115200);

    /* Initialize LVGL */
    lv_init();
	analogWrite(TFT_BL, 128); // Turn the backlight on
    /* Set the tick callback */
    lv_tick_set_cb(my_tick);
    /* Initialize the display driver */
    lv_lovyan_gfx_create(284, 76, lv_buffer, BUF_SIZE, true);

    // Define CPU temperature scale
    cpuTempWidget = create_scale(scale_type_t::SCALE_TYPE_TEMPERATURE, LV_ALIGN_LEFT_MID);
    lv_obj_set_parent(cpuTempWidget.scale, lv_screen_active());
    lv_obj_align(cpuTempWidget.scale, LV_ALIGN_LEFT_MID, 8, 0);
    
    // Define CPU load scale
    cpuLoadWidget = create_scale(scale_type_t::SCALE_TYPE_LOAD, LV_ALIGN_CENTER);
    lv_obj_set_parent(cpuLoadWidget.scale, lv_screen_active());
    lv_obj_align(cpuLoadWidget.scale, LV_ALIGN_CENTER, 0, 0);

    // Define GPU temperature scale
    gpuTempWidget = create_scale(scale_type_t::SCALE_TYPE_TEMPERATURE, LV_ALIGN_RIGHT_MID);
    lv_obj_set_parent(gpuTempWidget.scale, lv_screen_active());
    lv_obj_align(gpuTempWidget.scale, LV_ALIGN_RIGHT_MID, -8, 0);
}

void loop() {
    if (Serial.available()) {
        String input = Serial.readStringUntil('\n');
        input.trim();
        int firstComma = input.indexOf(',');
        int secondComma = input.indexOf(',', firstComma + 1);

        if (firstComma > 0 && secondComma > firstComma) {
            int cpuTemp = input.substring(0, firstComma).toInt();
            int cpuLoad = input.substring(firstComma + 1, secondComma).toInt();
            int gpuTemp = input.substring(secondComma + 1).toInt();

            Serial.printf("CPU Temp: %d, CPU Load: %d, GPU Temp: %d\n", cpuTemp, cpuLoad, gpuTemp);

            // Update CPU temperature scale (left)
            set_scale_value(&cpuTempWidget, SCALE_TYPE_TEMPERATURE, cpuTemp);

            // Update CPU load scale (center)
            set_scale_value(&cpuLoadWidget, SCALE_TYPE_LOAD, cpuLoad);

            // Update GPU temperature scale (right)
            set_scale_value(&gpuTempWidget, SCALE_TYPE_TEMPERATURE, gpuTemp);
        }
    }
    lv_timer_handler(); // Update the UI-
    delay(5);
}
