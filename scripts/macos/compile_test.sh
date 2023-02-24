PROJECT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && cd ../.. && pwd )

rm -r $PROJECT_DIR/obj/test
mkdir -p $PROJECT_DIR/obj/test

echo "compiling main.cpp"
clang++ -Wall -std=c++17 -I$PROJECT_DIR/deps/include -I$PROJECT_DIR/src -Itests $PROJECT_DIR/tests/main.cpp -c -o $PROJECT_DIR/obj/test/main.o
echo "compiling TestLogger.cpp"
clang++ -Wall -std=c++17 -I$PROJECT_DIR/deps/include -I$PROJECT_DIR/src $PROJECT_DIR/tests/TestLogger.cpp -c -o $PROJECT_DIR/obj/test/TestLogger.o
echo "compiling TestTimer.cpp"
clang++ -Wall -std=c++17 -I$PROJECT_DIR/deps/include -I$PROJECT_DIR/src $PROJECT_DIR/tests/TestTimer.cpp -c -o $PROJECT_DIR/obj/test/TestTimer.o
echo "compiling AIPlayer_test.cpp"
clang++ -Wall -std=c++17 -I$PROJECT_DIR/deps/include -I$PROJECT_DIR/src $PROJECT_DIR/tests/AIPlayer_test.cpp -c -o $PROJECT_DIR/obj/test/AIPlayer_test.o