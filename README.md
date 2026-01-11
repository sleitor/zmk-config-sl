# ZMK Config for Soufflé v3 Sweep

This is a ZMK configuration for the Soufflé v3 Sweep keyboard with nRF52840-based controllers and nice!view displays.

## Keyboard Specifications

- **Layout**: 6 columns × 4 rows + 5 thumb buttons + 1 palm key + 1 encoder button per side (31 keys per side, 62 total)
- **Controller**: Generic Pro Micro nRF52840 (v1940) or nice!nano compatible
- **Display**: nice!view Sharp Memory LCD (LS011B7DH03, 160×68) on both sides
- **Encoders**: 2 (one per side)
- **Connectivity**: Bluetooth Low Energy

## Building Firmware

### Automatic Build (GitHub Actions)

1. Fork this repository to your GitHub account
2. Enable GitHub Actions in your fork (Settings → Actions → Allow all actions)
3. Push changes to trigger a build
4. Download firmware from the Actions tab

### Local Build

#### Prerequisites

1. **Python 3.10 or later** (Python 3.12 recommended)
   ```bash
   # macOS with Homebrew
   brew install python@3.12
   ```

2. **West** (Zephyr's build tool)
   ```bash
   pip3 install west
   ```

3. **ARM GCC Toolchain**
   ```bash
   # macOS with Homebrew
   brew install arm-none-eabi-binutils arm-none-eabi-gcc arm-none-eabi-newlib
   ```

4. **ZMK Workspace** (if not already set up)
   ```bash
   west init -l config
   west update
   ```

#### Build Commands

Navigate to your ZMK workspace directory and run:

**Left side (with display and widgets):**
```bash
python3.12 -m west build -s zmk/app -d build/left -b nice_nano -p -- \
  -DBOARD_ROOT=/path/to/zmk-config-sl/config \
  -DSHIELD=souffle_v3_sweep_left \
  -DZEPHYR_TOOLCHAIN_VARIANT=gnuarmemb \
  -DGNUARMEMB_TOOLCHAIN_PATH=/opt/homebrew \
  -DCONFIG_SPI=y \
  -DCONFIG_DISPLAY=y \
  -DCONFIG_LS0XX=y \
  -DCONFIG_ZMK_DISPLAY=y \
  -DCONFIG_LVGL=y
```

**Right side (with display and widgets):**
```bash
python3.12 -m west build -s zmk/app -d build/right -b nice_nano -p -- \
  -DBOARD_ROOT=/path/to/zmk-config-sl/config \
  -DSHIELD=souffle_v3_sweep_right \
  -DZEPHYR_TOOLCHAIN_VARIANT=gnuarmemb \
  -DGNUARMEMB_TOOLCHAIN_PATH=/opt/homebrew \
  -DCONFIG_SPI=y \
  -DCONFIG_DISPLAY=y \
  -DCONFIG_LS0XX=y \
  -DCONFIG_ZMK_DISPLAY=y \
  -DCONFIG_LVGL=y
```

Replace `/path/to/zmk-config-sl` with the actual path to this config repository.

#### Build Options

You can customize the build by adjusting the `-DCONFIG_*` flags:

- **Basic display (no widgets):** Remove `-DCONFIG_ZMK_DISPLAY=y` and `-DCONFIG_LVGL=y`
- **Different toolchain path:** Adjust `-DGNUARMEMB_TOOLCHAIN_PATH` if your ARM toolchain is installed elsewhere
- **USB logging:** Add `-DCONFIG_ZMK_USB_LOGGING=y` for debugging

#### Output

Firmware files will be located at:
- Left: `build/left/zephyr/zmk.uf2`
- Right: `build/right/zephyr/zmk.uf2`

#### Troubleshooting

**"Could not find Zephyr-sdk"**: Specify the ARM toolchain manually with `-DZEPHYR_TOOLCHAIN_VARIANT=gnuarmemb` and `-DGNUARMEMB_TOOLCHAIN_PATH`

**"Python version too old"**: Use Python 3.10 or later. On macOS: `brew install python@3.12`

**Display shows random dots**: The configuration files include proper devicetree setup. Make sure you're using the commands above with all the `-DCONFIG_*` flags.

## Flashing

1. Connect the controller via USB
2. Double-tap the reset button (or short RST to GND twice) to enter bootloader mode
3. A drive named "NICENANO" or similar should appear
4. Drag and drop the `.uf2` file to the mounted drive
5. The controller will automatically restart with the new firmware

**Note**: Flash the left firmware to the left controller and right firmware to the right controller.

## Customizing

### Keymap
Edit `config/souffle_v3_sweep.keymap` to customize your key layout, layers, and encoder behavior.

### Hardware Configuration
- `config/boards/shields/souffle_v3_sweep/souffle_v3_sweep.dtsi` - Core hardware definition (matrix, encoders)
- `config/boards/shields/souffle_v3_sweep/souffle_v3_sweep_left.overlay` - Left side pins and display
- `config/boards/shields/souffle_v3_sweep/souffle_v3_sweep_right.overlay` - Right side pins and display

### Features
- `config/boards/shields/souffle_v3_sweep/souffle_v3_sweep.conf` - Feature toggles (display, sleep, BT power, etc.)
- `config/boards/shields/souffle_v3_sweep/souffle_v3_sweep_left.conf` - Left-side specific settings

## Pin Configuration

### Matrix
- **Rows** (both sides): P0.24, P1.00, P0.11, P1.04, P1.06
- **Columns (left)**: P0.02, P1.15, P1.13, P1.11, P0.10, P0.09
- **Columns (right)**: P0.09, P0.10, P1.11, P1.13, P1.15, P0.02 (reversed)

### Display (nice!view on both sides)
- **SPI Bus**: SPI0
- **MOSI**: P0.17
- **SCK**: P0.20
- **CS**: P1.01
- **MISO**: P0.25 (unused by display)

### Encoders (both sides)
- **A**: P0.31
- **B**: P0.29

**Note**: To modify pin assignments, edit the `.dtsi` and `.overlay` files in `config/boards/shields/souffle_v3_sweep/`

## Resources

- [ZMK Documentation](https://zmk.dev/docs)
- [Soufflé Keyboard GitHub](https://github.com/climent/SouffleKeyboard)
- [nice!nano Documentation](https://nicekeyboards.com/docs/nice-nano/)

## License

MIT