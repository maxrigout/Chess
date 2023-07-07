SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
COMPILER_DIR=~/dev/VulkanSDK/1.3.250.1/macOS/bin
SHADER_DIR=shaders/glsl
VERTEX_SRC=$1.vert
FRAGMENT_SRC=$1.frag
OUT_DIR=${SHADER_DIR}/compiled
COMPILER=glslc

GLSLC=${COMPILER_DIR}/${COMPILER}
VERTEX_INPUT=${SCRIPT_DIR}/${SHADER_DIR}/${VERTEX_SRC}
VERTEX_OUTPUT=${SCRIPT_DIR}/${OUT_DIR}/${VERTEX_SRC}.spv
FRAGMENT_INPUT=${SCRIPT_DIR}/${SHADER_DIR}/${FRAGMENT_SRC}
FRAGMENT_OUTPUT=${SCRIPT_DIR}/${OUT_DIR}/${FRAGMENT_SRC}.spv

echo "compiling ${VERTEX_SRC}"
${GLSLC} ${VERTEX_INPUT} -o ${VERTEX_OUTPUT}
echo "compiling ${FRAGMENT_SRC}"
${GLSLC} ${FRAGMENT_INPUT} -o ${FRAGMENT_OUTPUT}
echo "done compiling"