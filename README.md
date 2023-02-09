# Chess
Custom chess engine, using SDL.

## How To Build
### Mac OS
This project uses SDL as the main windowing system and as a renderer. You can install it from https://github.com/libsdl-org/SDL.
* Once this is installed create a folder "deps" at the root of the project.
* Create the following directories inside "deps": include/SDL2.
* Place the SDL2 headers inside "deps/include/SDL2".
    * Alternatively, you can change line 14 in the .vscode/tasks.json file to point to your own SDL2 installation .
* You can then build the project using VS Code. (cmd + shift + B)

### Windows
* TODO

### Linux
* TODO

## Todo
- [x] Project refactor
- [ ] Text (label for the pieces)
- [ ] AI (AlphaBeta)
- [ ] Better graphics
- [ ] Setup script
- [ ] Implementing different rendering backends (Vulkan / Metal etc...)