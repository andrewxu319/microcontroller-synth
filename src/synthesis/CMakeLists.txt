add_subdirectory(modules)

add_library(synthesizer synthesizer.cpp)
target_link_libraries(synthesizer PUBLIC module master voice_manager midi)