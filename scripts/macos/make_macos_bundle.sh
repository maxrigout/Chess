INPUT_BIN=$1
OUTPUT_BUNDLE_NAME=$2

PROJECT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && cd ../.. && pwd )

cd $PROJECT_DIR
mkdir -p $OUTPUT_BUNDLE_NAME.app/Contents/MacOS
mkdir -p $OUTPUT_BUNDLE_NAME.app/Contents/Resources
cp $INPUT_BIN $OUTPUT_BUNDLE_NAME.app/Contents/MacOS/$OUTPUT_BUNDLE_NAME
chmod +x $OUTPUT_BUNDLE_NAME.app/Contents/MacOS/$OUTPUT_BUNDLE_NAME
cp -R resources/ $OUTPUT_BUNDLE_NAME.app/Contents/
cp scripts/macos/rsc/Info.plist chess.app/Contents/Info.plist
cp scripts/macos/rsc/chess.icns chess.app/Contents/Resources/chess.icns