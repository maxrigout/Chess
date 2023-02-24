PROJECT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && cd ../.. && pwd )
cd $PROJECT_DIR

FRAMEWORK_NAME=$1
DOWNLOAD_URL=$2

DOWNLOAD_LOCATION=$PROJECT_DIR/$FRAMEWORK_NAME.dmg

echo "Downloading $FRAMEWORK_NAME framework"
curl -o $DOWNLOAD_LOCATION -L $DOWNLOAD_URL
echo "Mounting the image"
sudo hdiutil attach $FRAMEWORK_NAME.dmg
echo "Copying the framework"
cp -R /Volumes/$FRAMEWORK_NAME/$FRAMEWORK_NAME.framework deps/frameworks
echo "Copying the headers"
cp -R $PROJECT_DIR/deps/frameworks/$FRAMEWORK_NAME.framework/Versions/Current/Headers/ deps/include/$FRAMEWORK_NAME
# cleanup
echo "Cleanup"
sudo hdiutil detach /Volumes/$FRAMEWORK_NAME
echo "removing dmg file"
rm $DOWNLOAD_LOCATION
echo "done with $FRAMEWORK_NAME"