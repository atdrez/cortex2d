cmake_minimum_required(VERSION 2.8)
project(cortex2d)
add_definitions(-DCT_SDL_BACKEND)

file(GLOB cortex2d_src
"src/*.cpp"
"src/3rdparty/*.cpp"
"src/3rdparty/alut/*.cpp"
"src/3rdparty/freetypegl/*.cpp"
"src/audio/*.cpp"
"src/filesystem/*.cpp"
"src/ios/*.cpp"
"src/utils/*.cpp")

file(GLOB cortex2d_includes
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
"/usr/include/SDL2/"
"/usr/include/libpng12/")


add_library(cortex2d SHARED ${cortex2d_src})
target_link_libraries(cortex2d SDL2 png12 GLESv2 openal freetype alut)

install(TARGETS cortex2d DESTINATION lib)

install(FILES ${cortex2d_includes} DESTINATION include)
