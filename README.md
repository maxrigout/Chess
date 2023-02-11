# Chess
Custom chess engine, using SDL.

## Dependencies
| Name | Url |
| ---- | --- |
| SDL2 | [https://github.com/libsdl-org/SDL]() |
| SDL2_ttf | [https://github.com/libsdl-org/SDL_ttf]() |
| ~~SDL2_image~~ | ~~[https://github.com/libsdl-org/SDL_image]()~~ |


## How To Build
### Mac OS
* Once the dependencies are installed create the following directories "deps/include" at the root of the project.
* Place the SDL2 and SDL2_ttf headers inside "deps/include/SDL2" and "deps/include/SDL2_ttf" respectively.
    * Alternatively, you can change line 14 in the .vscode/tasks.json file to point to your own SDL2/SDL2_ttf installation.
* Create a fonts folder.
* You should have the following structure:
```
 |-- .vscode
   | Contains the tasks and settings required to build the project with VS Code
   |
 |-- deps
   | Contains all the dependencies for this project
   |
   |-- include
      | Contains all the header files needed for the project
      |
      |-- SDL2
      |-- Contains the SDL2 headers
      |
      |-- SDL2_ttf
      |-- Contains the SDL2_tff header
   |-- fonts
      | Contains all the fonts
      |
   |-- src
    | Contains all the source code for the project

```
* Add a font to the fonts folder. (You might need to change the font that's loaded in src/Renderer2D/SDL/Renderer2D_SDL.cpp, line 25, wip)
* You can then build and run the project using VS Code.

### Windows
* TODO

### Linux
* TODO

## Todo
- [x] Project refactor
- [x] Renderer refactor
- [x] Text (label for the pieces)
- [ ] AI (AlphaBeta)
- [ ] Better graphics
- [ ] Window and input system
- [ ] Setup script
- [ ] Implementing different rendering backends (Vulkan / Metal etc...)