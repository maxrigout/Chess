#if defined RELEASE && __APPLE__
#define BOARD_TEXTURE_PATH "../graphics/board3.png"
#define PIECES_TEXTURE_PATH "../graphics/pieces4.png"
#define FONT_PATH "../fonts/FatBrush.ttf"
#elif defined DEBUG && __APPLE__
#define BOARD_TEXTURE_PATH "/Users/max/dev/repos/Chess/resources/graphics/board3.png"
#define PIECES_TEXTURE_PATH "/Users/max/dev/repos/Chess/resources/graphics/pieces4.png"
#define AVAILABLE_CELLS_PATH "/Users/max/dev/repos/Chess/resources/graphics/Green.png"
#define HOVERED_CELLS_PATH "/Users/max/dev/repos/Chess/resources/graphics/Highlightedcell.png"
#define SELECTED_CELLS_PATH "/Users/max/dev/repos/Chess/resources/graphics/Activecell.png"
#define FONT_PATH "/Users/max/dev/repos/Chess/resources/fonts/FatBrush.ttf"
#else
#define BOARD_TEXTURE_PATH "resources/graphics/board3.png"
#define PIECES_TEXTURE_PATH "resources/graphics/pieces4.png"
#define FONT_PATH "resources/fonts/FatBrush.ttf"
#endif
