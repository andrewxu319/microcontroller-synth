add_library(envelope envelope.cpp)
target_link_libraries(envelope PRIVATE module utils)

add_library(portamento portamento.cpp)
target_link_libraries(portamento PRIVATE module utils)