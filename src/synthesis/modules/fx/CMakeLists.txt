add_subdirectory(filter)
add_subdirectory(components)
add_subdirectory(distortion)
add_subdirectory(reverb)

add_library(fx fx.cpp)
target_link_libraries(fx PRIVATE module utils)

add_library(chorus chorus.cpp)
target_link_libraries(chorus PRIVATE fx oscillator utils)

add_library(delay delay.cpp)
target_link_libraries(delay PRIVATE fx utils)

add_library(flanger flanger.cpp)
# target_link_libraries(flanger PRIVATE )

add_library(phaser phaser.cpp)
target_link_libraries(phaser PRIVATE fx phaser_allpass utils)
