PROJECT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && cd ../.. && pwd )

rm $PROJECT_DIR/obj/main.o
rm $PROJECT_DIR/test

echo "linking tests..."
g++ $PROJECT_DIR/obj/*.o $PROJECT_DIR/obj/test/*.o -o $PROJECT_DIR/test -F/Library/Frameworks -framework SDL2 -framework SDL2_ttf -framework SDL2_image -pthread
echo "done linking tests."