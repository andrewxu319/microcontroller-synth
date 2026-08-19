add_library(utils math.cpp timer.cpp)

target_include_directories(utils PRIVATE ${PORTAUDIO_INCLUDE_DIRS})
target_link_libraries(utils PRIVATE ${PORTAUDIO_LIBRARIES})