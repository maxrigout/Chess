# Chess
Custom chess engine, using SDL. It features an AI using the Minimax algorithm with AlphaBeta pruning. The AI's moves calculations are done on a separate thread.

## Dependencies
| Name | Url |
| ---- | --- |
| SDL2 | [https://github.com/libsdl-org/SDL](https://github.com/libsdl-org/SDL) |
| SDL2_ttf | [https://github.com/libsdl-org/SDL_ttf](https://github.com/libsdl-org/SDL_ttf) |
| SDL2_image | [https://github.com/libsdl-org/SDL_image](https://github.com/libsdl-org/SDL_image) |

* Once the dependencies are installed create the following directories at the root of the project:
    * deps/include
       * SDL2
       * SDL2_ttf
       * SDL2_image
    * deps/libraries
    * fonts
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
      |-- SDL2_image
      |-- Contains the SDL2_image header
      |
      |-- SDL2_ttf
      |-- Contains the SDL2_tff header
      |
   |-- includes
      | Contains all the 3rd party libraries used for linking
      |
 |-- resources
   |-- Contains all the resources needed to render the game
   |
   |-- fonts
      | Contains all the fonts
      |
   |-- graphics
      | Contains all the graphics
      |
 |-- src
   | Contains the source code for the project
   |
```
* Add a font to the fonts folder. (You might need to change the font that's loaded in src/Renderer2D/SDL/Renderer2D_SDL.cpp, line 25, wip)

## How To Build
### Mac OS (with VS Code)
* Copy the `SDL2`, `SDL2_ttf` and `SDL2_image` headers inside `deps/include/SDL2`, `deps/include/SDL2_ttf` and `deps/include/SDL2_image` respectively.
    * Alternatively, you can change line 14 in the .vscode/tasks.json file to point to your own SDL2/SDL2_ttf/SDL2_image installation.
* Open the project in VS Code.
* Go to Terminal > Run Build Task to build the project.

### Windows (with Visual Studio 2022)
* Copy the SDL2, SDL2_ttf and SDL2_image headers inside `deps/include/SDL2`, `deps/include/SDL2_ttf` and `deps/include/SDL2_image` respectively.
* Add the following .lib files to the deps/libraries folder:
    * SDL2.lib
    * SDL2main.lib
    * SDL2_ttf.lib
    * SDL2_image.lib (tbd)
* Open the .sln file.
* Build the project.
    * If the program crashes during execution due to missing dlls, then you'll need to copy the dlls to the root of the project.

### Linux
* TODO

## Todo
- [x] Project refactor
- [x] Renderer refactor
- [x] Text (label for the pieces)
- [x] AI (AlphaBeta)
- [ ] Better graphics (wip)
- [x] Better AI
- [ ] Window and input system
- [ ] Setup script
- [ ] Implementing different rendering backends (Vulkan / Metal etc...)

## Issues to resolve
- [x] The King's moves aren't properly calculated when using the AIPlayer.
   * it was actually a bug in how the king's moves were calculated.
- [ ] The AIPlayer is using the "real" board and the "real" opponent to do its calculations.
- [x] Cannot undo a "castle" move.
- [ ] The pieces are moved when the AIPlayer is testing moves.