cmake_minimum_required(VERSION 3.19)

Project(MicrocontrollerSynth LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(PORTAUDIO REQUIRED portaudio-2.0)
pkg_check_modules(RTMIDI REQUIRED rtmidi)
find_package(Eigen3 REQUIRED)
if(Eigen3_FOUND)
    message(STATUS "Eigen3 found: ${Eigen3_VERSION}")
else()
    message(STATUS "Eigen3 not found")
endif()
set(AUDIOFILE_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/include/AudioFile")

include_directories(${CMAKE_SOURCE_DIR}/src)

# avx flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-march=native)
elseif(MSVC)
    add_compile_options(/arch:AVX2) 
endif()

add_subdirectory(tracy)
add_subdirectory(src)
