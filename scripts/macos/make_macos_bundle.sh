INPUT_BIN=$1
OUTPUT_BUNDLE_NAME=$2

PROJECT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && cd ../.. && pwd )

cd $PROJECT_DIR
mkdir -p $OUTPUT_BUNDLE_NAME.app/Contents/MacOS
mkdir -p $OUTPUT_BUNDLE_NAME.app/Contents/Resources/graphics
mkdir -p $OUTPUT_BUNDLE_NAME.app/Contents/Resources/fonts
touch $OUTPUT_BUNDLE_NAME.app/Contents/Resources/fonts/font
touch $OUTPUT_BUNDLE_NAME.app/Contents/Resources/graphics/board
cp $INPUT_BIN $OUTPUT_BUNDLE_NAME.app/Contents/MacOS/$OUTPUT_BUNDLE_NAME
cp -R resources/ $OUTPUT_BUNDLE_NAME.app/Contents/Resources