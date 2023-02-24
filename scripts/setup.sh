BASE_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && cd .. && pwd )

mkdir -p $BASE_DIR/deps/include
cd $BASE_DIR/deps
mkdir {include,libraries,frameworks}
cd include
mkdir {SDL2,SDL2_image,SDL2_ttf}

cd $BASE_DIR
mkdir resources
cd resources
mkdir {fonts,graphics}