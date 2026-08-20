cmake_minimum_required(VERSION 3.10)
project(gif2ascii CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
find_package(SFML QUIET COMPONENTS Graphics Window System)
if(SFML_FOUND)
    #Windows rolling release
    message(STATUS "Found SFML 3+")
    set(SFML_LIBS SFML::Graphics SFML::Window SFML::System)
else()
    #Stable Linux
    find_package(SFML REQUIRED COMPONENTS graphics window system)
    message(STATUS "Found SFML 2")
    set(SFML_LIBS sfml-graphics sfml-window sfml-system)
endif()

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
    ${SFML_LIBS}
)
