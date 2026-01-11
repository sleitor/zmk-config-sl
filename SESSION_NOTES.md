# ZMK Configuration Session Notes
**Last Updated:** 2026-01-11 (Module migration and LVGL troubleshooting)

## Completed Work

### ✅ Encoder Configuration
- Fixed encoder rotation (was not working, only button worked)
- Created side-specific config files:
  - `souffle_v3_sweep_left.conf`
  - `souffle_v3_sweep_right.conf`
- Enabled `CONFIG_EC11_TRIGGER_GLOBAL_THREAD=y` on both sides
- Swapped A/B pins on left encoder to reverse direction

### ✅ Matrix Transform Fix
- Encoders are **physically** positioned in row 3 (after B key)
- Encoders are **electrically** wired to row 4 matrix positions: RC(4,5) and RC(4,6)
- Updated matrix transform in `souffle_v3_sweep.dtsi`:
  ```
  Row 3: RC(3,0-5) RC(4,5) RC(4,6) RC(3,6-11)  // Encoder buttons in middle
  Row 4: RC(4,0) RC(4,1-4) RC(4,7-11)          // Thumb cluster
  ```

### ✅ ZMK Studio Support
- Enabled via `cmake-args: -DCONFIG_ZMK_STUDIO=y` in build.yaml
- Added studio-rpc-usb-uart snippet to left side
- Created physical layout file: `souffle_v3_sweep.physical_layout.dtsi`
- Added unlock key to layer 1 (lower layer) - accessible from left side only
- Fixed devicetree rotation syntax:
  - Rotation values in hundredths of degrees (30° = 3000)
  - Negative values wrapped in parentheses: `(-3000)`

### ✅ Keymap-Editor Support
- Created `config/info.json` for https://nickcoutsos.github.io/keymap-editor/
- Based on Sofle V2 layout structure
- Includes proper encoder positions and sensor definitions

### ✅ Build Documentation (2026-01-10)
- Created comprehensive build instructions in `README.md`
- Created quick reference guide in `BUILD.md`
- Documented local build process with all required cmake flags
- Added troubleshooting section for common build issues
- Updated pin configuration documentation to match actual hardware

### ✅ Display Configuration (Fixed 2026-01-10)
- **Display Model:** Sharp LS011B7DH03 (160×68 Memory LCD)
- **Issue:** Display showed random frozen dots, not clearing
- **Root Cause:** Incorrect devicetree configuration
  - `zephyr,display` was pointing to SPI bus instead of display device
  - Missing display device label
- **Solution:**
  - Added proper label `nice_view_display` to display node
  - Updated `zephyr,display = &nice_view_display` in chosen node
  - Fixed both left and right overlay files
- **Build Requirements:** Must use cmake flags to force display configs:
  - `-DCONFIG_SPI=y`
  - `-DCONFIG_DISPLAY=y`
  - `-DCONFIG_LS0XX=y`
  - `-DCONFIG_ZMK_DISPLAY=y` (for widgets)
  - `-DCONFIG_LVGL=y` (for widgets)
- **Hardware:** Both sides have displays
  - SPI0 bus: MOSI=P0.17, SCK=P0.20, CS=P1.01
  - I2C disabled to avoid conflicts
- Enabled `CONFIG_ZMK_EXT_POWER=y`
- Enabled widgets: layer status, battery, output, WPM

## Current Status

### Working
- All 60 keys functional
- Encoder rotation working on both sides
- Encoder buttons mapped to C_MUTE (left) and C_PP (right)
- ZMK Studio connects and unlocks
- Builds complete successfully

### ⚠️ Known Issues
1. **Physical layout thumb cluster positions not correct in ZMK Studio**
   - Row 4 thumb keys still positioned incorrectly in visual layout
   - Needs further adjustment to match actual keyboard geometry
   - User accepted to fix later

2. **Display firmware ready but not yet tested** (2026-01-10)
   - Fixed devicetree configuration
   - Built firmware with display support (701KB left, 610KB right)
   - Files ready to flash: `souffle_v3_sweep_left_DISPLAY_WITH_WIDGETS.uf2` and `souffle_v3_sweep_right_DISPLAY_WITH_WIDGETS.uf2`
   - Needs testing to confirm displays now work correctly

## File Structure

```
config/
├── zephyr/
│   └── module.yml                                 # Module declaration
├── boards/shields/souffle_v3_sweep/
│   ├── souffle_v3_sweep.keymap                   # Key bindings (modern location)
│   ├── souffle_v3_sweep.zmk.yml                  # Shield metadata
│   ├── souffle_v3_sweep.conf                     # Main config
│   ├── souffle_v3_sweep_left.conf                # Left side config
│   ├── souffle_v3_sweep_right.conf               # Right side config
│   ├── souffle_v3_sweep.dtsi                     # Matrix transform
│   ├── souffle_v3_sweep.physical_layout.dtsi     # Physical layout for Studio
│   ├── souffle_v3_sweep_left.overlay             # Left hardware (display, pins)
│   ├── souffle_v3_sweep_right.overlay            # Right hardware (display, pins)
│   ├── Kconfig.shield                            # Shield flags
│   └── Kconfig.defconfig                         # Default config
├── info.json                                      # Keymap-editor layout
└── west.yml                                       # West manifest
build.yaml                                         # Build configuration
README.md                                          # Full documentation
BUILD.md                                           # Quick build reference
CLAUDE.md                                          # Instructions for Claude Code
SESSION_NOTES.md                                   # This file
```

## Key Technical Details

### Keyboard Layout
- **Total keys:** 60 (5 rows, but varying columns per row)
- **Split:** 30 keys per side
- **Encoders:** 2 (one per side, positioned between B and N keys physically)
- **Palm keys:** LALT (left outer) and RALT (right outer)

### Matrix Mapping (Rows 0-4, Cols 0-11)
- Row 0: 12 keys (GRAVE to MINUS)
- Row 1: 12 keys (TAB to BSLH)
- Row 2: 12 keys (LCTRL to SQT)
- Row 3: 12 keys (LSHFT to RSHFT, excluding encoder buttons)
- Row 4: 10 thumb keys + 2 encoder buttons

### Encoder Buttons
- Left encoder: RC(4,5) - C_MUTE
- Right encoder: RC(4,6) - C_PP
- Physically appear in row 3 between splits
- Matrix-wise part of row 4 (thumb cluster)

### Build Configuration
**GitHub Actions (`build.yaml`):**
```yaml
- board: nice_nano
  shield: souffle_v3_sweep_left
  snippet: zmk-usb-logging  # For debugging (conflicts with ZMK Studio)
- board: nice_nano
  shield: souffle_v3_sweep_right
```

**Local Build (with display support):**
```bash
python3.12 -m west build -s zmk/app -d build/left -b nice_nano -p -- \
  -DZMK_CONFIG=/path/to/config \
  -DSHIELD=souffle_v3_sweep_left \
  -DZEPHYR_TOOLCHAIN_VARIANT=gnuarmemb \
  -DGNUARMEMB_TOOLCHAIN_PATH=/opt/homebrew \
  -DCONFIG_SPI=y \
  -DCONFIG_DISPLAY=y \
  -DCONFIG_LS0XX=y \
  -DCONFIG_ZMK_DISPLAY=y \
  -DCONFIG_LVGL=y
```

See `BUILD.md` for complete build instructions.

## Next Steps / TODO

1. **✅ COMPLETED: Display basic driver working**
   - Basic display successfully clears random dots
   - 466KB firmware boots reliably
   - SPI communication verified functional

2. **⚠️ BLOCKED: LVGL widget display**
   - LVGL framework won't boot on generic Pro Micro boards
   - Tried all known configuration fixes (stack size, blank config, etc.)
   - May require official nice!nano hardware or custom display implementation
   - **User decision needed**: Accept basic display or pursue alternatives

3. **Fix physical layout positions for row 4 (thumb cluster)**
   - Currently keys overlap with row 3 in ZMK Studio
   - Need to adjust X/Y coordinates in `souffle_v3_sweep.physical_layout.dtsi`
   - Possibly adjust rotation angles for better visual representation

4. **Build right side firmware with basic display**
   - Create matching basic display firmware for right side
   - Test both displays working simultaneously
   - Verify split communication

5. **Optional: Custom display implementation**
   - Write lightweight text rendering without LVGL
   - Display layer number and battery percentage
   - Avoid LVGL overhead that causes boot issues

6. **Optional: Adapt for nickcoutsos/keymap-editor**
   - Basic support added via info.json
   - May need layout refinement

## Resources

- **Latest firmware:** https://github.com/sleitor/zmk-config-sl/actions
- **ZMK Studio:** https://zmk.studio/
- **Keymap Editor:** https://nickcoutsos.github.io/keymap-editor/
- **ZMK Docs - Physical Layouts:** https://zmk.dev/docs/development/hardware-integration/physical-layouts
- **Sofle V2 reference:** https://github.com/ergomechstore/sofle-v2-zmk

## Notes

- Devicetree rotation syntax requires parentheses for negative values
- Split keyboards need side-specific .conf files for EC11_TRIGGER_GLOBAL_THREAD
- cmake-args is the correct method to enable ZMK Studio (not just .conf)
- Physical layouts use hundredths of degrees for rotation (3000 = 30°)
- Encoder pins P0.29/P0.31 are low-frequency (<10kHz) but work despite this

### Display Troubleshooting Lessons (2026-01-10)
- **Config files may not be applied**: .conf files weren't being used, needed cmake overrides
- **Devicetree labels matter**: Must label the display device node, not just the SPI bus
- **LVGL needs device dimensions**: `width` and `height` properties required in display node
- **Generic driver is fine**: "sharp,ls0xx" works for all Sharp Memory LCD models
- **Dimensions identify model**: 160×68 uniquely identifies LS011B7DH03
- **SPI0 vs SPI1**: Both work, but must match overlay configuration
- **I2C conflicts**: Disable `pro_micro_i2c` when using SPI displays
- **Build toolchain**: Use `-DZEPHYR_TOOLCHAIN_VARIANT=gnuarmemb` if Zephyr SDK not installed
- **Firmware size**: With LVGL enabled, firmware is ~700KB (vs ~450KB without)

### ✅ Module System Migration (2026-01-11)
**Successfully migrated to modern ZMK module structure**

- Created `config/zephyr/module.yml` - Declares repository as ZMK keyboard module
- Created `souffle_v3_sweep.zmk.yml` - Shield metadata with features and dependencies
- Moved keymap to proper location: `boards/shields/souffle_v3_sweep/souffle_v3_sweep.keymap`
- **Eliminated deprecation warning** by using `-DBOARD_ROOT` instead of deprecated `-DZMK_CONFIG`
- Updated all build documentation (BUILD.md, README.md) with modern build commands

**Old approach (deprecated):**
```bash
-DZMK_CONFIG=/path/to/config
```

**New approach (modern):**
```bash
-DBOARD_ROOT=/path/to/config
```

### ⚠️ LVGL Display Issue (2026-01-11)
**Current Status:**
- ✅ **Basic display driver works perfectly** (466KB firmware)
  - Clears random dots successfully
  - Display initializes correctly
  - SPI communication functional
- ❌ **LVGL/ZMK_DISPLAY firmware won't boot** (701KB firmware)
  - Firmware builds successfully
  - Device won't boot - stays frozen
  - No response after flashing

**What was tried:**
1. Added `CONFIG_ZMK_DISPLAY_DEDICATED_THREAD_STACK_SIZE=4096` (from official nice_view config)
2. Added `CONFIG_ZMK_DISPLAY_BLANK_ON_IDLE=n`
3. Tested CS pin polarity (ACTIVE_HIGH works, ACTIVE_LOW breaks everything)
4. Removed USB logging to reduce firmware size (no change)
5. Disabled individual widgets to reduce size (still won't boot)
6. Verified devicetree configuration matches official nice_view exactly

**Technical Details:**
- Display: Sharp LS011B7DH03 (160×68) via SPI0
- Pin Configuration: MOSI=P0.17, SCK=P0.20, CS=P1.01 (ACTIVE_HIGH)
- Hardware: Generic "v1940 Pro Micro nRF52840" controllers (not official nice!nano)
- ZMK Version: Latest main branch (commit 19582174, Jan 8 2026)
- Zephyr Version: v4.1.0+zmk-fixes (latest ZMK fork)
- Memory Usage: FLASH 44.20%, RAM 24.39% (plenty of room)

**Hypothesis:**
LVGL initialization may have timing or compatibility issues with generic Pro Micro nRF52840 boards that differ from official nice!nano boards. The basic display driver works perfectly, suggesting hardware is functional but LVGL framework specifically has issues.

**Available Firmware Files:**
- `souffle_v3_sweep_left_BASIC_DISPLAY_FIXED.uf2` (466KB) - ✅ **WORKING**
  - Basic display driver only
  - Clears screen successfully
  - No widgets
- `souffle_v3_sweep_left_WITH_WIDGETS.uf2` (701KB) - ❌ Won't boot
  - Full LVGL framework
  - All widgets enabled
  - Device freezes on boot

### Display Troubleshooting Lessons - Additional (2026-01-11)
- **Deprecation warning avoidable**: Use `-DBOARD_ROOT` instead of `-DZMK_CONFIG`
- **Module structure required**: Must create `zephyr/module.yml` and `.zmk.yml` files
- **CS polarity critical**: ACTIVE_HIGH works for this hardware, ACTIVE_LOW completely breaks display
- **LVGL incompatibility**: Generic Pro Micro boards may not be fully compatible with LVGL framework
- **Stack size not the issue**: Even with proper 4096 stack size, LVGL won't boot
- **Basic display is reliable**: Low-level display driver works perfectly without LVGL overhead

## Current Status Summary

### Working Features ✅
- All 60 keys functional
- Encoder rotation on both sides
- Encoder buttons (C_MUTE left, C_PP right)
- Basic display driver (clears screen, no widgets)
- Modern module structure (no deprecation warnings)
- Proper devicetree configuration
- SPI communication verified working

### Known Limitations ⚠️
1. **Display widgets not available**
   - LVGL/ZMK_DISPLAY firmware won't boot on generic Pro Micro boards
   - Basic display clears screen but shows no layer/battery/output info
   - Likely hardware-specific incompatibility with LVGL framework

2. **Physical layout thumb cluster positions**
   - Row 4 keys positioned incorrectly in ZMK Studio visual
   - Functional but needs coordinate adjustments

### Firmware Recommendations

**For reliable operation:**
```bash
# Left side - basic display
souffle_v3_sweep_left_BASIC_DISPLAY_FIXED.uf2 (466KB)
```

**Configuration that works:**
- CONFIG_SPI=y
- CONFIG_DISPLAY=y  
- CONFIG_LS0XX=y
- CONFIG_ZMK_EXT_POWER=y
- NO CONFIG_ZMK_DISPLAY (causes boot failure)
- NO CONFIG_LVGL (causes boot failure)

**To re-enable widgets (if hardware compatible):**
Add back to .conf files:
```
CONFIG_ZMK_DISPLAY=y
CONFIG_ZMK_DISPLAY_BLANK_ON_IDLE=n
CONFIG_ZMK_DISPLAY_DEDICATED_THREAD_STACK_SIZE=4096
CONFIG_ZMK_WIDGET_LAYER_STATUS=y
CONFIG_ZMK_WIDGET_BATTERY_STATUS=y
CONFIG_ZMK_WIDGET_OUTPUT_STATUS=y
CONFIG_ZMK_WIDGET_WPM_STATUS=y
```

## Future Options

1. **Accept current solution**: Basic display clears screen, keyboard fully functional
2. **Custom display code**: Write lightweight text rendering without LVGL framework  
3. **Test with official nice!nano**: See if LVGL works on official hardware
4. **Investigate LVGL alternatives**: Look for lighter display frameworks in Zephyr

## Version Information

- **ZMK**: commit 19582174 (Jan 8, 2026) - Latest main
- **Zephyr**: v4.1.0+zmk-fixes - Current ZMK fork with keyboard patches
- **Build System**: Using modern `-DBOARD_ROOT` method (no deprecation warnings)
- **Display Driver**: Sharp LS0XX generic driver (works perfectly)
- **LVGL**: Version 9.3.0 (won't boot on generic hardware)
