project( ColVD )
cmake_minimum_required(VERSION 3.0)
include( ../IDCCommon.cmake )


include_directories( ./inc ../inc ../IDCLibs )


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

set(src_list
src/main.cpp
src/ColMaster.cpp
src/ColLogical.cpp
src/ColXtp.cpp)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

add_executable( ColVD ${src_list}  )      

target_link_libraries( ColVD ${LIB_LOG} ${LIB_XTPCLIENT} ${LIB_BASE} rt pthread )


