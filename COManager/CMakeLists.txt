project( COMan )
cmake_minimum_required(VERSION 3.0)
include( ../IDCCommon.cmake )

include_directories( ./inc ../inc ../IDCLibs)

find_library(LIB_LOG
    IDCLog
    ../IDCLibs/IDCLog/build
)

find_library(LIB_BASE
    IDCBase
    ../IDCLibs/IDCBase/build
)

set(src_list
src/getopt.c
src/COMain.cpp
src/Master.cpp
src/MasterOD.cpp
src/COApp.cpp)

ADD_DEFINITIONS(-DNOT_USE_DYNAMIC_LOADING -DDEBUG_PRINTF)

add_executable( COMan ${src_list}  )      

target_link_libraries( COMan ${LIB_LOG} ${LIB_BASE} ${LIB_CO} rt pthread )
