# !/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

$SCRIPT_DIR/install_framework.sh SDL2 https://github.com/libsdl-org/SDL/releases/download/release-2.26.3/SDL2-2.26.3.dmg
$SCRIPT_DIR/install_framework.sh SDL2_ttf https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.20.2/SDL2_ttf-2.20.2.dmg
$SCRIPT_DIR/install_framework.sh SDL2_image https://github.com/libsdl-org/SDL_image/releases/download/release-2.6.3/SDL2_image-2.6.3.dmg