set SDL2_VERSION=2.26.4
set SDL2_IMAGE_VERSION=2.6.3
set SDL2_TTF_VERSION=2.20.2

echo unzipping SDL...
cd tmp
tar -xf SDL2.zip
tar -xf SDL2_image.zip
tar -xf SDL2_ttf.zip
cd ..

echo installing dependencies

echo installing SDL2
call :installSDLInclude SDL2 %SDL2_VERSION%
call :installSDLLib SDL2 %SDL2_VERSION%

echo installing SDL2_image
call :installSDLInclude SDL2_image %SDL2_IMAGE_VERSION%
call :installSDLLib SDL2_image %SDL2_IMAGE_VERSION%

echo installing SDL2_ttf
call :installSDLInclude SDL2_ttf %SDL2_TTF_VERSION%
call :installSDLLib SDL2_ttf %SDL2_TTF_VERSION%

echo installing stb_image
copy tmp\stb_image.h deps\include\stb\stb_image.h
PAUSE

:installSDLInclude <name> <version>
xcopy tmp\%1-%2\include\ deps\include\%1\ /E/H/C/I

:installSDLLib <name> <version>
set name=%1
set version=%2
copy tmp\%name%-%version%\lib\x64\%name%.lib deps\lib\%name%.lib
if %name%==SDL2 copy tmp\%name%-%version%\lib\x64\%name%main.lib deps\lib\%name%main.lib
