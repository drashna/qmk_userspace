# Drashna's Keymaps

[![Build QMK firmware](https://github.com/drashna/qmk_userspace/actions/workflows/build_binaries.yaml/badge.svg?branch=master)](https://github.com/drashna/qmk_userspace/actions/workflows/build_binaries.yaml)

For instructions on how to setup see [main readme](https://github.com/qmk/qmk_userspace/blob/main/README.md).

For quick setup, mostly just run `qmk config user.overlay_dir="$(realpath .)"`

## Overview

This is my personal userspace file.  Most of my code exists here, as it's heavily shared.

* [Callback (keymap+misc)](docs/callbacks.md)
* [Keycode Handling](docs/keyrecords.md)
  * [Diablo Tap Dancing](docs/tap_dance.md)
  * [Keymap Wrappers](docs/wrappers.md)
  * [Secret Macros](docs/secrets.md)
  * [Custom Keycodes](docs/keycodes.md)
  * [Unicode Input](docs/unicode.md)
* [OLED Display](docs/oled.md)
* [Pointing Devices](docs/pointing.md)
* [RGB Customization](docs/rgb.md)
* [Split Transport](docs/split.md)
