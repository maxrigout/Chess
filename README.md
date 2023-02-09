# Chess
custom chess engine, using an SDL Renderer

## How To Build
### Mac OS
This project uses SDL as the main windowing system and as a renderer. You can install it from `https://github.com/libsdl-org/SDL`
* Once this is installed create a folder "deps" at the root of the project.
* Create the following directories inside "deps": include/SDL2
* place the SDL2 headers inside "deps/include/SDL2"

* alternatively, you can change line 14 in the .vscode/tasks.json to point to your own SDL2 installation

### Windows
* TODO

### Linux
* TODO

## Todo
[ ] Text (label for the pieces)
[ ] AI (AlphaBeta)
[ ] Better graphics
[ ] Implementing different rendering backends (Vulkan / Metal etc...)