echo downloading dependencies...
echo downloading SDL2.zip (version 2.26.4) to tmp/SDL2.zip
curl -o tmp/SDL2.zip -L https://github.com/libsdl-org/SDL/releases/download/release-2.26.4/SDL2-devel-2.26.4-VC.zip

echo downloading SDL2_image.zip (version 2.6.3) to tmp/SDL2_image.zip
curl -o tmp/SDL2_image.zip -L https://github.com/libsdl-org/SDL_image/releases/download/release-2.6.3/SDL2_image-devel-2.6.3-VC.zip

echo downloading SDL2_ttf.zip (version 2.20.2) to tmp/SDL2_ttf.zip
curl -o tmp/SDL2_ttf.zip -L https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.20.2/SDL2_ttf-devel-2.20.2-VC.zip

echo downloading stb_image.h (latest) to tmp/stb_image.h
curl -o tmp/stb_image.h -L https://raw.githubusercontent.com/nothings/stb/master/stb_image.h