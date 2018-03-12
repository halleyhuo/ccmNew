project( GeneratorVD )
cmake_minimum_required(VERSION 3.0)
include( ../IDCCommon.cmake )

include_directories(../inc ../IDCLibs/ ./inc)

find_library(LIB_LOG
    IDCLog
    ../IDCLibs/IDCLog/build
)

find_library(LIB_BASE
    IDCBase
    ../IDCLibs/IDCBase/build
)

find_library(LIB_XTPCLIENT
    XTPClientIf
    ../IDCLibs/IDCXTP/build
)

set( sources
src/GenMain.cpp
src/GenXtp.cpp
src/GenStateM.cpp
src/HeatUnitCal.cpp
)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

add_executable( GeneratorVD.out ${sources}  )

target_link_libraries( GeneratorVD.out ${LIB_LOG} ${LIB_XTPCLIENT} ${LIB_BASE} rt pthread )

