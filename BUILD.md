# Quick Build Guide

## Prerequisites

```bash
# macOS with Homebrew
brew install python@3.12
brew install arm-none-eabi-binutils arm-none-eabi-gcc arm-none-eabi-newlib
pip3 install west
```

## Setup ZMK Workspace (first time only)

```bash
mkdir -p ~/Projects/zmk-workspace
cd ~/Projects/zmk-workspace
west init -l ~/Projects/zmk-config-sl/config
west update
```

## Build Commands

### Build Left Side

```bash
cd ~/Projects/zmk-workspace

python3.12 -m west build -s zmk/app -d build/left -b nice_nano -p -- \
  -DBOARD_ROOT=~/Projects/zmk-config-sl/config \
  -DSHIELD=souffle_v3_sweep_left \
  -DZEPHYR_TOOLCHAIN_VARIANT=gnuarmemb \
  -DGNUARMEMB_TOOLCHAIN_PATH=/opt/homebrew \
  -DCONFIG_SPI=y \
  -DCONFIG_DISPLAY=y \
  -DCONFIG_LS0XX=y \
  -DCONFIG_ZMK_DISPLAY=y \
  -DCONFIG_LVGL=y
```

Output: `build/left/zephyr/zmk.uf2`

### Build Right Side

```bash
cd ~/Projects/zmk-workspace

python3.12 -m west build -s zmk/app -d build/right -b nice_nano -p -- \
  -DBOARD_ROOT=~/Projects/zmk-config-sl/config \
  -DSHIELD=souffle_v3_sweep_right \
  -DZEPHYR_TOOLCHAIN_VARIANT=gnuarmemb \
  -DGNUARMEMB_TOOLCHAIN_PATH=/opt/homebrew \
  -DCONFIG_SPI=y \
  -DCONFIG_DISPLAY=y \
  -DCONFIG_LS0XX=y \
  -DCONFIG_ZMK_DISPLAY=y \
  -DCONFIG_LVGL=y
```

Output: `build/right/zephyr/zmk.uf2`

## Alternative: Build Without Widgets

If the display shows issues, try building without ZMK widgets:

```bash
# Left (basic display only)
python3.12 -m west build -s zmk/app -d build/left -b nice_nano -p -- \
  -DBOARD_ROOT=~/Projects/zmk-config-sl/config \
  -DSHIELD=souffle_v3_sweep_left \
  -DZEPHYR_TOOLCHAIN_VARIANT=gnuarmemb \
  -DGNUARMEMB_TOOLCHAIN_PATH=/opt/homebrew \
  -DCONFIG_SPI=y \
  -DCONFIG_DISPLAY=y \
  -DCONFIG_LS0XX=y

# Right (basic display only)
python3.12 -m west build -s zmk/app -d build/right -b nice_nano -p -- \
  -DBOARD_ROOT=~/Projects/zmk-config-sl/config \
  -DSHIELD=souffle_v3_sweep_right \
  -DZEPHYR_TOOLCHAIN_VARIANT=gnuarmemb \
  -DGNUARMEMB_TOOLCHAIN_PATH=/opt/homebrew \
  -DCONFIG_SPI=y \
  -DCONFIG_DISPLAY=y \
  -DCONFIG_LS0XX=y
```

## Notes

- Replace `~/Projects/zmk-config-sl` and `~/Projects/zmk-workspace` with your actual paths
- The `-p` flag does a pristine build (cleans before building)
- If toolchain is in a different location, adjust `-DGNUARMEMB_TOOLCHAIN_PATH`
- For Linux, toolchain path might be `/usr` or `/usr/local`

## Troubleshooting

**Command not found: python3.12**
```bash
# Use whatever Python 3.10+ you have
python3 -m west build ...
# or
python3.11 -m west build ...
```

**Could not find Zephyr-sdk**
Make sure you include both:
- `-DZEPHYR_TOOLCHAIN_VARIANT=gnuarmemb`
- `-DGNUARMEMB_TOOLCHAIN_PATH=/opt/homebrew`

**Wrong paths**
Update the paths in the commands above to match your system:
- ZMK config: Path to this repository's `config` folder
- ZMK workspace: Path to where you ran `west init`
- Toolchain path: Where ARM GCC is installed (`which arm-none-eabi-gcc` will show you)