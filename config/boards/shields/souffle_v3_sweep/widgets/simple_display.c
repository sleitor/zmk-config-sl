/*
 * Simple Display Module - Lightweight display without LVGL
 * Shows layer number and battery percentage
 */

#include <zephyr/device.h>
#include <zephyr/drivers/display.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <zmk/event_manager.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/events/battery_state_changed.h>
#include <zmk/keymap.h>
#include <zmk/battery.h>

LOG_MODULE_REGISTER(simple_display, CONFIG_ZMK_LOG_LEVEL);

static const struct device *display = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));

// Display dimensions (Sharp LS011B7DH03: 160x68)
#define DISPLAY_WIDTH 160
#define DISPLAY_HEIGHT 68

// Simple 5x7 font for digits (very basic)
static const uint8_t font_5x7[][7] = {
    // 0
    {0x1F, 0x11, 0x11, 0x11, 0x1F, 0x00, 0x00},
    // 1
    {0x00, 0x12, 0x1F, 0x10, 0x00, 0x00, 0x00},
    // 2
    {0x1D, 0x15, 0x15, 0x15, 0x17, 0x00, 0x00},
    // 3
    {0x11, 0x15, 0x15, 0x15, 0x1F, 0x00, 0x00},
    // 4
    {0x07, 0x04, 0x04, 0x1F, 0x04, 0x00, 0x00},
    // 5
    {0x17, 0x15, 0x15, 0x15, 0x1D, 0x00, 0x00},
    // 6
    {0x1F, 0x15, 0x15, 0x15, 0x1D, 0x00, 0x00},
    // 7
    {0x01, 0x01, 0x1D, 0x03, 0x01, 0x00, 0x00},
    // 8
    {0x1F, 0x15, 0x15, 0x15, 0x1F, 0x00, 0x00},
    // 9
    {0x17, 0x15, 0x15, 0x15, 0x1F, 0x00, 0x00},
};

static uint8_t display_buffer[DISPLAY_WIDTH * DISPLAY_HEIGHT / 8];
static uint8_t current_layer = 0;
static uint8_t battery_percent = 0;

static void clear_buffer(void) {
    memset(display_buffer, 0, sizeof(display_buffer));
}

static void set_pixel(int x, int y, bool on) {
    if (x < 0 || x >= DISPLAY_WIDTH || y < 0 || y >= DISPLAY_HEIGHT) {
        return;
    }

    int byte_idx = (y * DISPLAY_WIDTH + x) / 8;
    int bit_idx = (y * DISPLAY_WIDTH + x) % 8;

    if (on) {
        display_buffer[byte_idx] |= (1 << bit_idx);
    } else {
        display_buffer[byte_idx] &= ~(1 << bit_idx);
    }
}

static void draw_digit(int x, int y, uint8_t digit) {
    if (digit > 9) return;

    const uint8_t *glyph = font_5x7[digit];

    for (int col = 0; col < 5; col++) {
        for (int row = 0; row < 7; row++) {
            bool pixel = (glyph[row] & (1 << (4 - col))) != 0;
            set_pixel(x + col, y + row, pixel);
        }
    }
}

static void draw_number(int x, int y, uint8_t num) {
    if (num >= 100) {
        draw_digit(x, y, num / 100);
        draw_digit(x + 6, y, (num / 10) % 10);
        draw_digit(x + 12, y, num % 10);
    } else if (num >= 10) {
        draw_digit(x, y, num / 10);
        draw_digit(x + 6, y, num % 10);
    } else {
        draw_digit(x, y, num);
    }
}

static void draw_text(int x, int y, const char *text) {
    // Draw simple "L:" for layer
    if (strcmp(text, "L:") == 0) {
        // Draw L
        for (int row = 0; row < 7; row++) {
            set_pixel(x, y + row, true);
        }
        for (int col = 0; col < 4; col++) {
            set_pixel(x + col, y + 6, true);
        }

        // Draw :
        set_pixel(x + 6, y + 2, true);
        set_pixel(x + 6, y + 4, true);
    }
    // Draw simple "B:" for battery
    else if (strcmp(text, "B:") == 0) {
        // Draw B
        for (int row = 0; row < 7; row++) {
            set_pixel(x, y + row, true);
        }
        set_pixel(x + 1, y, true);
        set_pixel(x + 2, y, true);
        set_pixel(x + 3, y + 1, true);
        set_pixel(x + 1, y + 3, true);
        set_pixel(x + 2, y + 3, true);
        set_pixel(x + 3, y + 4, true);
        set_pixel(x + 1, y + 6, true);
        set_pixel(x + 2, y + 6, true);

        // Draw :
        set_pixel(x + 6, y + 2, true);
        set_pixel(x + 6, y + 4, true);
    }
}

static void update_display(void) {
    if (!device_is_ready(display)) {
        return;
    }

    clear_buffer();

    // Draw "L:X" (Layer)
    draw_text(10, 10, "L:");
    draw_number(25, 10, current_layer);

    // Draw "B:XX%" (Battery)
    draw_text(10, 30, "B:");
    draw_number(25, 30, battery_percent);
    draw_text(43, 30, "%");  // Just draw a simple % sign

    // Draw a border
    for (int x = 0; x < DISPLAY_WIDTH; x++) {
        set_pixel(x, 0, true);
        set_pixel(x, DISPLAY_HEIGHT - 1, true);
    }
    for (int y = 0; y < DISPLAY_HEIGHT; y++) {
        set_pixel(0, y, true);
        set_pixel(DISPLAY_WIDTH - 1, y, true);
    }

    // Write to display
    struct display_buffer_descriptor desc = {
        .buf_size = sizeof(display_buffer),
        .width = DISPLAY_WIDTH,
        .height = DISPLAY_HEIGHT,
        .pitch = DISPLAY_WIDTH,
    };

    int ret = display_write(display, 0, 0, &desc, display_buffer);
    if (ret != 0) {
        LOG_ERR("Display write failed: %d", ret);
    } else {
        LOG_DBG("Display updated: Layer %d, Battery %d%%", current_layer, battery_percent);
    }
}

static void update_display_work_handler(struct k_work *work) {
    update_display();
}

static K_WORK_DEFINE(update_display_work, update_display_work_handler);

// Only enable layer tracking on central (left) side
#if IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
static int layer_state_changed_listener(const zmk_event_t *eh) {
    const struct zmk_layer_state_changed *ev = as_zmk_layer_state_changed(eh);
    if (ev == NULL) {
        return 0;
    }

    current_layer = zmk_keymap_highest_layer_active();
    k_work_submit(&update_display_work);

    return 0;
}

ZMK_LISTENER(simple_display_layer, layer_state_changed_listener);
ZMK_SUBSCRIPTION(simple_display_layer, zmk_layer_state_changed);
#endif

static int battery_state_changed_listener(const zmk_event_t *eh) {
    const struct zmk_battery_state_changed *ev = as_zmk_battery_state_changed(eh);
    if (ev == NULL) {
        return 0;
    }

    battery_percent = ev->state_of_charge;
    k_work_submit(&update_display_work);

    return 0;
}

ZMK_LISTENER(simple_display_battery, battery_state_changed_listener);
ZMK_SUBSCRIPTION(simple_display_battery, zmk_battery_state_changed);

static int simple_display_init(void) {
    if (!device_is_ready(display)) {
        LOG_ERR("Display device not ready");
        return -ENODEV;
    }

    LOG_INF("Simple display module initialized");

    // Get initial state
#if IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
    current_layer = zmk_keymap_highest_layer_active();
#else
    current_layer = 0; // Peripheral side doesn't have layer info
#endif
    battery_percent = zmk_battery_state_of_charge();

    // Schedule initial display update after 1 second
    k_work_submit(&update_display_work);

    return 0;
}

SYS_INIT(simple_display_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);