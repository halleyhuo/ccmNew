
# Common include for all projects

SET(TOOLCHAIN_DIR "/opt/feeling/usr/local/gcc-4.6.2-glibc-2.13-linaro-multilib-2011.12/fsl-linaro-toolchain/bin")
set( CMAKE_BUILD_TYPE "release" )
set( CMAKE_CXX_COMPILER_FULLPATH "${TOOLCHAIN_DIR}/arm-none-linux-gnueabi-gcc")
set( CMAKE_CXX_COMPILER "${TOOLCHAIN_DIR}/arm-none-linux-gnueabi-g++" )
