# Chess
[![CI](https://github.com/maxrigout/Chess/actions/workflows/main.yml/badge.svg)](https://github.com/maxrigout/Chess/actions/workflows/main.yml)\
Custom chess engine, using SDL. It features an AI using the Minimax algorithm with AlphaBeta pruning. The AI's moves calculations are done on a separate thread.
You can read more about the AI here:\
Minimax: https://en.wikipedia.org/wiki/Minimax
Alpha-Beta-Pruning: https://en.wikipedia.org/wiki/Alpha%E2%80%93beta_pruning

## Dependencies
| Name | Url |
| ---- | --- |
| SDL2 | [https://github.com/libsdl-org/SDL](https://github.com/libsdl-org/SDL) |
| SDL2_ttf | [https://github.com/libsdl-org/SDL_ttf](https://github.com/libsdl-org/SDL_ttf) |
| SDL2_image | [https://github.com/libsdl-org/SDL_image](https://github.com/libsdl-org/SDL_image) |

* run the scripts/setup.sh file.
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
   |-- libraries
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
* Add a font to the resources/fonts folder. (You might need to change the font that's loaded in src/Assets.h, line 25, wip)
* Add graphics to the resources/graphics folder. You'll need an image for the board and for the pieces. You can configure how the images are interpreted in the "loadGraphics" member function of the Game class (Game.cpp line 129). For the board image, you'll need to change the variables at lines 78, 79 and 80.

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

## Running the Tests
### Mac OS
   * run the scripts/test.sh file

## Todo
- [x] Project refactor (02-08-2023)
- [x] Renderer refactor (02-10-2023)
- [x] Text (label for the pieces) (02-10-2023)
- [x] AI (AlphaBeta) (02-12-2023)
- [x] Better graphics (02-20-2023)
- [x] Better AI (02-17-2023)
- [x] Window and input system (03-05-2023)
- [x] Setup script (02-22-2023)
- [ ] Implementing different rendering backends (Vulkan / Metal etc...)
- [ ] Use better testing framework. ([catch2](https://github.com/catchorg/Catch2) or [googletest](https://github.com/google/googletest))
- [ ] Game configurator (ie configure the AI difficulty, human vs human etc..)
- [ ] GUI
- [ ] Ability to reset a game

## Issues to resolve
- [x] The King's moves aren't properly calculated when using the AIPlayer. (02-18-2023)
   * it was actually a bug in how the king's moves were calculated.
- [x] The AIPlayer is using the "real" board and the "real" opponent to do its calculations. (02-25-2023)
   * made a copy of the board
- [x] Cannot undo a "castle" move. (02-19-2023)
   * created a new move system.
- [x] The pieces are moved when the AIPlayer is testing moves. (side effect of the AIPlayer using the real board) (02-25-2023)
   * solved when coping the board
- [x] AI does not take a piece when it can. (02-22-2023)
- [ ] Moves aren't properly undone when the AI Plays. (happens with the knights sometimes)
- [ ] Issues with the undo system... meaning that I have to create a new copy of the board for each move I want to test
- [x] Some piece's first move state isn't reset properly. (affects pawns and kings) (02-22-2023)
   * when a captured move was added, it was using the default value (false) instead of using the piece's first move state.
