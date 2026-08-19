add_subdirectory(serge_wavefolder)

add_library(soft_clip soft_clip.cpp)
target_link_libraries(soft_clip PRIVATE fx utils)

add_library(wavefolder wavefolder.cpp)
target_link_libraries(wavefolder PRIVATE fx utils)

add_library(ideal_wavefolder ideal_wavefolder.cpp)
target_link_libraries(ideal_wavefolder PRIVATE wavefolder utils)