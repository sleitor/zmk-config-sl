# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Overview

This is a ZMK firmware configuration for the Soufflé v3 Sweep split keyboard with nice!nano v2 controllers and nice!view displays. The keyboard features 62 keys total (31 per side) with encoders and ZMK Studio support.

## Build Commands

### GitHub Actions (Automatic)
Push changes to trigger automatic build via GitHub Actions. Firmware artifacts are available in the Actions tab.

### Local Build
```bash
# Build left half
west build -d build/left -b nice_nano_v2 -- -DSHIELD=souffle_v3_sweep_left

# Build right half
west build -d build/right -b nice_nano_v2 -- -DSHIELD=souffle_v3_sweep_right
```

Requirements: ZMK development environment installed (see https://zmk.dev/docs/development/setup)

## Architecture

### File Structure

**Keymap**: `config/souffle_v3_sweep.keymap`
- Main keymap configuration with layer definitions
- Contains 4 layers: Base, Lower (symbols), Raise (numbers/nav/BT), and an empty Layer 3
- Encoder bindings defined per layer

**Hardware Definition**: `config/boards/shields/souffle_v3_sweep/`
- `souffle_v3_sweep.dtsi`: Core hardware definition (matrix transform, kscan, encoder configuration)
- `souffle_v3_sweep_left.overlay` / `souffle_v3_sweep_right.overlay`: Side-specific pin mappings and display configuration
- `souffle_v3_sweep.conf`: Feature configuration (encoders, displays, BT power, sleep, ZMK Studio)
- `souffle_v3_sweep.physical_layout.dtsi`: Physical layout for ZMK Studio
- `info.json`: Keyboard metadata and layout positions for visual editors

### Hardware Configuration

**Matrix**: 5 rows × 12 columns (split across two halves)
- Diode direction: col2row
- Row pins: P0.24, P1.00, P0.11, P1.04, P1.06
- Column pins (left half): P0.02, P1.15, P1.13, P1.11, P0.10, P0.09

**Encoders**:
- Both encoders use pins P0.29 (B) and P0.31 (A) - note the A/B swap
- 80 steps per rotation, 20 triggers-per-rotation in keymap
- Configured in `souffle_v3_sweep.dtsi`, enabled per-side in overlay files

**Display (nice!view)**:
- SPI1 interface: MOSI on P0.17, SCK on P0.20, CS on P1.01
- Sharp LS011B7DH03 (160×68)
- Configured only in left/right overlay files

### Build Matrix

`build.yaml` defines two build targets:
1. Left half with ZMK Studio support via USB UART (`studio-rpc-usb-uart` snippet)
2. Right half (standard build)

### ZMK Features Enabled

- EC11 rotary encoders with global thread triggering
- BT transmit power: +8dBm (extended range)
- Deep sleep after 15 minutes idle
- ZMK Studio for runtime configuration
- nice!view widgets: layer, battery, output, WPM status
- External power control for displays

## Common Modifications

### Changing Keymap
Edit `config/souffle_v3_sweep.keymap`. Each layer's `bindings` array must have exactly 62 key positions matching the matrix transform. Encoder behavior defined in `sensor-bindings`.

### Adjusting Pin Mappings
If PCB pins differ from defaults, modify:
- Row/column GPIOs in `souffle_v3_sweep.dtsi` (rows) and overlay files (columns)
- Encoder pins in `souffle_v3_sweep.dtsi` (a-gpios, b-gpios)

### Display Configuration
Display settings in left/right overlay files under `&spi1` and `nice_view_spi` nodes.

### Feature Toggles
Modify `souffle_v3_sweep.conf` to enable/disable:
- Encoders (CONFIG_EC11)
- Sleep timeout (CONFIG_ZMK_IDLE_SLEEP_TIMEOUT, in milliseconds)
- Display widgets
- ZMK Studio (CONFIG_ZMK_STUDIO)

### ZMK Studio
Access ZMK Studio by connecting left half via USB (only left half has studio-rpc-usb-uart). Use `&studio_unlock` binding to unlock runtime configuration.