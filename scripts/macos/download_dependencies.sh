SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

echo "downloading stb_image.h"
curl -o deps/include/stb/stb_image.h -L https://raw.githubusercontent.com/nothings/stb/master/stb_image.h