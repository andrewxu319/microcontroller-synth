add_library(rbj_filter
    allpass.cpp
    bandpass.cpp
    high_shelf.cpp
    highpass.cpp
    low_shelf.cpp
    lowpass.cpp
    notch.cpp    
)
target_link_libraries(rbj_filter PRIVATE filter module)