# ZMK Configuration Session Notes
**Date:** 2025-12-31

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

### ✅ Display Configuration
- Changed from LS011B7DH01 to LS011B7DH03 (correct for nice!view)
- Enabled `CONFIG_ZMK_EXT_POWER=y`
- Enabled WPM widget per user request

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

2. **Display status unknown**
   - Configuration updated but not yet tested with latest firmware

## File Structure

```
config/
├── info.json                                      # Keymap-editor layout
├── boards/shields/souffle_v3_sweep/
│   ├── souffle_v3_sweep.conf                     # Main config
│   ├── souffle_v3_sweep_left.conf                # Left encoder config
│   ├── souffle_v3_sweep_right.conf               # Right encoder config
│   ├── souffle_v3_sweep.dtsi                     # Matrix transform
│   ├── souffle_v3_sweep.physical_layout.dtsi     # Physical layout for Studio
│   ├── souffle_v3_sweep.keymap                   # Key bindings
│   ├── souffle_v3_sweep_left.overlay             # Left display config
│   └── souffle_v3_sweep_right.overlay            # Right display config
build.yaml                                         # Build configuration
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
```yaml
- board: nice_nano
  shield: souffle_v3_sweep_left
  snippet: studio-rpc-usb-uart
  cmake-args: -DCONFIG_ZMK_STUDIO=y
- board: nice_nano
  shield: souffle_v3_sweep_right
```

## Next Steps / TODO

1. **Fix physical layout positions for row 4 (thumb cluster)**
   - Currently keys overlap with row 3 in ZMK Studio
   - Need to adjust X/Y coordinates in `souffle_v3_sweep.physical_layout.dtsi`
   - Possibly adjust rotation angles for better visual representation

2. **Test nice!view displays**
   - Verify LS011B7DH03 configuration works
   - Check if WPM widget displays correctly
   - Confirm external power management works

3. **Test with right side connected**
   - Currently only left side available
   - Need to verify split communication
   - Test right encoder functionality

4. **Optional: Adapt for nickcoutsos/keymap-editor**
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
