cmake_minimum_required(VERSION 3.10)
project(gif2ascii CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
find_package(SFML COMPONENTS graphics window system REQUIRED)

pkg_check_modules(AVFORMAT REQUIRED IMPORTED_TARGET libavformat)
pkg_check_modules(AVCODEC REQUIRED IMPORTED_TARGET libavcodec)
pkg_check_modules(SWSCALE REQUIRED IMPORTED_TARGET libswscale)
pkg_check_modules(AVUTIL REQUIRED IMPORTED_TARGET libavutil)

set(SOURCES
    src/main.cpp
    src/GifDecoder.cpp
    src/FrameScaler.cpp
    src/GifEncoder.cpp
    src/AsciiRenderer.cpp
)

add_executable(gif2ascii ${SOURCES})

target_include_directories(gif2ascii PRIVATE include)

target_link_libraries(gif2ascii PRIVATE
    PkgConfig::AVFORMAT
    PkgConfig::AVCODEC
    PkgConfig::SWSCALE
    PkgConfig::AVUTIL
    sfml-graphics
    sfml-window
    sfml-system
)
