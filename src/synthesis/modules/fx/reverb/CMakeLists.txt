add_library(reverb reverb.cpp)
target_link_libraries(reverb PRIVATE fx utils PUBLIC Eigen3::Eigen)

add_library(luff_reverb luff.cpp)
target_link_libraries(luff_reverb PRIVATE reverb multichannel_diffuser delay_line PUBLIC Eigen3::Eigen)

add_library(schroeder_reverb schroeder.cpp)
target_link_libraries(schroeder_reverb PRIVATE reverb delay_line schroeder_allpass mixer utils PUBLIC Eigen3::Eigen)
