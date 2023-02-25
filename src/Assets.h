#if defined RELEASE && __APPLE__
#define BOARD_TEXTURE_PATH "../graphics/board3.png"
#define PIECES_TEXTURE_PATH "../graphics/pieces4.png"
#define FONT_PATH "../fonts/FatBrush.ttf"
#else
#define BOARD_TEXTURE_PATH "resources/graphics/board3.png"
#define PIECES_TEXTURE_PATH "resources/graphics/pieces4.png"
#define FONT_PATH "resources/fonts/FatBrush.ttf"
#endif