cmake_minimum_required(VERSION 2.8)
project(cortex2d)
add_definitions(-DCT_SDL_BACKEND)
add_definitions(-DCT_OPENAL_AUDIO)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
find_package(SDL2 REQUIRED)
find_package(FreeType REQUIRED)
find_package(PNG12 REQUIRED)

file(GLOB cortex2d_src
"src/*.cpp"
"src/3rdparty/*.cpp"
"src/3rdparty/alut/*.cpp"
"src/3rdparty/freetypegl/*.cpp"
"src/audio/*.cpp"
"src/audio/openal/*.cpp"
"src/filesystem/*.cpp"
"src/ios/*.cpp"
"src/utils/*.c"
"src/utils/*.cpp"
"src/3rdparty/alut/*.c"
"src/3rdparty/freetypegl/*.c")

file(GLOB cortex2d_includes
"include/Cortex2D"
"src/*.h"
"src/3rdparty/*.h"
"src/3rdparty/alut/*.h"
"src/3rdparty/freetypegl/*.h"
"src/audio/*.h"
"src/filesystem/*.h"
"src/ios/*.h"
"src/utils/*.h")

include_directories(
"src/"
"src/3rdparty/"
"src/3rdparty/alut/"
"src/3rdparty/freetypegl/"
"src/audio/"
"src/filesystem/"
"src/ios/"
"src/utils/"
${SDL2_INCLUDE_DIR}
${PNG12_INCLUDE}
${FREETYPE_INCLUDE_DIR_freetype2})



add_library(cortex2d SHARED ${cortex2d_src})
target_link_libraries(cortex2d ${SDL2_LIBRARY} ${PNG12_LIBRARIES} GLESv2 openal ${FREETYPE_LIBRARY} alut)

install(TARGETS cortex2d DESTINATION lib)

install(FILES ${cortex2d_includes} DESTINATION include)
