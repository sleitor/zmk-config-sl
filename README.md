# ZMK Config for Soufflé v3 Sweep

This is a ZMK configuration for the Soufflé v3 Sweep keyboard with nice!nano v2 (nRF52840) and nice!view displays.

## Keyboard Specifications

- **Layout**: 6 columns × 4 rows + 5 thumb buttons + 1 palm key + 1 encoder button per side (31 keys per side, 62 total)
- **Controller**: nice!nano v2 (nRF52840)
- **Display**: nice!view (per side)
- **Encoders**: 2 (one per side)
- **Connectivity**: Bluetooth Low Energy

## Building Firmware

### Automatic Build (GitHub Actions)

1. Fork this repository to your GitHub account
2. Enable GitHub Actions in your fork (Settings → Actions → Allow all actions)
3. Push changes to trigger a build
4. Download firmware from the Actions tab

### Local Build

1. Install ZMK development environment: https://zmk.dev/docs/development/setup
2. Clone this repository
3. Build with:
   ```bash
   west build -d build/left -b nice_nano_v2 -- -DSHIELD=souffle_v3_sweep_left
   west build -d build/right -b nice_nano_v2 -- -DSHIELD=souffle_v3_sweep_right
   ```
4. Flash the `.uf2` files to your controllers

## Flashing

1. Connect the nice!nano via USB
2. Double-tap the reset button to enter bootloader mode
3. Drag and drop the `.uf2` file to the mounted drive
4. The controller will automatically restart with the new firmware

## Customizing

- Edit `boards/shields/souffle_v3_sweep/souffle_v3_sweep.keymap` to customize your key layout
- Modify `boards/shields/souffle_v3_sweep/souffle_v3_sweep.conf` for feature settings

## Pin Configuration

Based on Sofle pinout (Pro Micro compatible):
- **Rows**: C6, D7, E6, B4, B5
- **Columns**: F6, F7, B1, B3, B2, B6
- **Encoders**: F5 (A), F4 (B)

**Note**: If your Soufflé v3 Sweep PCB uses different pins, you'll need to update the `.dtsi` and `.overlay` files.

## Resources

- [ZMK Documentation](https://zmk.dev/docs)
- [Soufflé Keyboard GitHub](https://github.com/climent/SouffleKeyboard)
- [nice!nano Documentation](https://nicekeyboards.com/docs/nice-nano/)

## License

MIT