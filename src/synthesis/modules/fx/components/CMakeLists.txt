add_library(delay_line delay_line.cpp)
target_link_libraries(delay_line PRIVATE fx utils)

add_library(multichannel_diffuser multichannel_diffuser.cpp)
target_link_libraries(multichannel_diffuser PRIVATE module delay_line mixer utils)

add_library(phaser_allpass phaser_allpass.cpp)
target_link_libraries(phaser_allpass PRIVATE rbj_filter)

add_library(schroeder_allpass schroeder_allpass.cpp)
target_link_libraries(schroeder_allpass PRIVATE fx delay_line mixer utils)