# Simple display module - only compile if ZMK_DISPLAY (LVGL) is disabled
# Enable on both left and right sides
if((CONFIG_SHIELD_SOUFFLE_V3_SWEEP_LEFT OR CONFIG_SHIELD_SOUFFLE_V3_SWEEP_RIGHT) AND CONFIG_DISPLAY AND NOT CONFIG_ZMK_DISPLAY)
    target_sources(app PRIVATE simple_display.c)
    message(STATUS "Simple display module enabled (LVGL-free)")
endif()