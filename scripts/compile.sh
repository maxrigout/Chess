DEFINE=$1
PROJECT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && cd .. && pwd )

if [ -z $DEFINE ]; then
	DEFINE=
else
	DEFINE=-D$DEFINE
fi

mkdir $PROJECT_DIR/obj

echo "compiling main.cpp"
clang++ -Wall -std=c++17 -I$PROJECT_DIR/deps/include -I$PROJECT_DIR/src $DEFINE $PROJECT_DIR/src/main.cpp -c -o $PROJECT_DIR/obj/main.o
echo "compiling Logger.cpp"
clang++ -Wall -std=c++17 -I$PROJECT_DIR/deps/include -I$PROJECT_DIR/src $DEFINE $PROJECT_DIR/src/Logger.cpp -c -o $PROJECT_DIR/obj/Logger.o
echo "compiling Renderer2D_SDL.cpp"
clang++ -Wall -std=c++17 -I$PROJECT_DIR/deps/include -I$PROJECT_DIR/src $DEFINE $PROJECT_DIR/src/Renderer2D/SDL/Renderer2D_SDL.cpp -c -o $PROJECT_DIR/obj/Renderer2D_SDL.o
echo "compiling Board.cpp"
clang++ -Wall -std=c++17 -I$PROJECT_DIR/deps/include -I$PROJECT_DIR/src $DEFINE $PROJECT_DIR/src/Chess/Board.cpp -c -o $PROJECT_DIR/obj/Board.o
echo "compiling Cell.cpp"
clang++ -Wall -std=c++17 -I$PROJECT_DIR/deps/include -I$PROJECT_DIR/src $DEFINE $PROJECT_DIR/src/Chess/Cell.cpp -c -o $PROJECT_DIR/obj/Cell.o
echo "compiling Common.cpp"
clang++ -Wall -std=c++17 -I$PROJECT_DIR/deps/include -I$PROJECT_DIR/src $DEFINE $PROJECT_DIR/src/Chess/Common.cpp -c -o $PROJECT_DIR/obj/Common.o
echo "compiling Game.cpp"
clang++ -Wall -std=c++17 -I$PROJECT_DIR/deps/include -I$PROJECT_DIR/src $DEFINE $PROJECT_DIR/src/Chess/Game.cpp -c -o $PROJECT_DIR/obj/Game.o
echo "compiling Piece.cpp"
clang++ -Wall -std=c++17 -I$PROJECT_DIR/deps/include -I$PROJECT_DIR/src $DEFINE $PROJECT_DIR/src/Chess/Piece.cpp -c -o $PROJECT_DIR/obj/Piece.o
echo "compiling Player.cpp"
clang++ -Wall -std=c++17 -I$PROJECT_DIR/deps/include -I$PROJECT_DIR/src $DEFINE $PROJECT_DIR/src/Chess/Player.cpp -c -o $PROJECT_DIR/obj/Player.o
echo "compiling AIPlayer.cpp"
clang++ -Wall -std=c++17 -I$PROJECT_DIR/deps/include -I$PROJECT_DIR/src $DEFINE $PROJECT_DIR/src/Chess/Players/AIPlayer.cpp -c -o $PROJECT_DIR/obj/AIPlayer.o
echo "compiling HumanPlayer.cpp"
clang++ -Wall -std=c++17 -I$PROJECT_DIR/deps/include -I$PROJECT_DIR/src $DEFINE $PROJECT_DIR/src/Chess/Players/HumanPlayer.cpp -c -o $PROJECT_DIR/obj/HumanPlayer.o
echo "compiling Bishop.cpp"
clang++ -Wall -std=c++17 -I$PROJECT_DIR/deps/include -I$PROJECT_DIR/src $DEFINE $PROJECT_DIR/src/Chess/Pieces/Bishop.cpp -c -o $PROJECT_DIR/obj/Bishop.o
echo "compiling FastPiece.cpp"
clang++ -Wall -std=c++17 -I$PROJECT_DIR/deps/include -I$PROJECT_DIR/src $DEFINE $PROJECT_DIR/src/Chess/Pieces/FastPiece.cpp -c -o $PROJECT_DIR/obj/FastPiece.o
echo "compiling SlowPiece.cpp"
clang++ -Wall -std=c++17 -I$PROJECT_DIR/deps/include -I$PROJECT_DIR/src $DEFINE $PROJECT_DIR/src/Chess/Pieces/SlowPiece.cpp -c -o $PROJECT_DIR/obj/SlowPiece.o
echo "compiling King.cpp"
clang++ -Wall -std=c++17 -I$PROJECT_DIR/deps/include -I$PROJECT_DIR/src $DEFINE $PROJECT_DIR/src/Chess/Pieces/King.cpp -c -o $PROJECT_DIR/obj/King.o
echo "compiling Pawn.cpp"
clang++ -Wall -std=c++17 -I$PROJECT_DIR/deps/include -I$PROJECT_DIR/src $DEFINE $PROJECT_DIR/src/Chess/Pieces/Pawn.cpp -c -o $PROJECT_DIR/obj/Pawn.o
echo "compiling Rook.cpp"
clang++ -Wall -std=c++17 -I$PROJECT_DIR/deps/include -I$PROJECT_DIR/src $DEFINE $PROJECT_DIR/src/Chess/Pieces/Rook.cpp -c -o $PROJECT_DIR/obj/Rook.o