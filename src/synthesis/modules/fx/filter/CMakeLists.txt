add_subdirectory(RBJ)

add_library(filter filter.cpp)
target_link_libraries(filter PRIVATE fx utils)