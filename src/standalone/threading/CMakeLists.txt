add_library(threading scheduler.cpp)
target_link_libraries(threading PRIVATE module synthesizer utils)