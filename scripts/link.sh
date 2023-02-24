BASE_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && cd .. && pwd )
echo "linking..."
clang++ $BASE_DIR/obj/*.o -o $BASE_DIR/out -F$BASE_DIR/deps/frameworks -framework SDL2 -framework SDL2_ttf -framework SDL2_image
echo "done linking."