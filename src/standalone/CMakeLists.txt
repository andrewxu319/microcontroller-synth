add_executable(MicrocontrollerSynth
    standalone.cpp
    file_io.cpp
    midi_listener.cpp
    sound_engine.cpp
)

add_subdirectory(threading)

target_link_libraries(
    MicrocontrollerSynth PRIVATE
    application
    midi
    synthesizer
    threading
    utils
)

target_include_directories(MicrocontrollerSynth
    PRIVATE
    ${PORTAUDIO_INCLUDE_DIRS}
    ${RTMIDI_INCLUDE_DIRS}
    ${AUDIOFILE_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}
)

target_link_libraries(MicrocontrollerSynth PRIVATE ${PORTAUDIO_LIBRARIES} ${RTMIDI_LIBRARIES} Tracy::TracyClient)