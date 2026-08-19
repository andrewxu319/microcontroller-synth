add_subdirectory(utils)
add_subdirectory(midi)
add_subdirectory(synthesis)
add_subdirectory(standalone)

add_library(application application.cpp)
target_link_libraries(application PRIVATE
    synthesizer
    oscillator
    performed_oscillator
    noise_generator
    voice
    voice_manager
    rbj_filter
    delay
    phaser
    flanger
    chorus
    schroeder_reverb
    luff_reverb
    multichannel_diffuser
    soft_clip
    ideal_wavefolder
    serge_wavefolder
    envelope
    portamento
)