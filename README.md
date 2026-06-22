This is a modular MIDI synthesizer that can run either as a standalone Windows application or on a Teensy microcontroller.

## Features
General
- Mono (for now)
- Unlimited polyphony
- MIDI-controlled
- Most parameters can be modulated in real time by the output of any module
- Any arbitrary arrangement of modules & is possible, with automatical signal generation ordering

Oscillators
- Sine, sawtooth, square, triangle oscillators
- White noise
- Custom waveforms (.wav file required)
- Performed (with MIDI input) and LFO-type oscillators

Performance
- Mono legato and portamento

Effects
- ADSR
- LFOs
- Filters (RBJ): high/low/allpass, bandpass, notch, high/low shelf
- Chorus
- Flanger
- Phaser
- Delay
- Reverb: Schroeder, [Luff](https://signalsmith-audio.co.uk/writing/2021/lets-write-a-reverb/#diffusion-a-multi-channel-diffuser-choosing-delay-times)
- Distortion: arctan soft clip, linear ("ideal") wavefolder, [Serge wavefolder](https://www.mdpi.com/2076-3417/7/12/1328)

## Codebase
- AVX2 and AVX512 for vector arithmetic
- Eigen (Windows) & CMSIS-DSP (Teensy) for linear algebra
- TeensyAudio for Teensy audio output (not used for the signal processing itself)
- RtMidi for Windows MIDI input
- PortAudio for Windows audio output

## Building
Requires C++20.

For desktop: build with MicrocontrollerSynth.sln or with something else.

For Teensy: build with PlatformIO and see PlatformIO.ini. Use the `TEENSY` flag if not using PlatformIO. MIDI input pins are configurable.

## Usage
All configurations are found in `src/utils/config.h`.

To set up the synthesizer, edit `src/applications.cpp` to set up all modules, map midi CC, and route their inputs & outputs (in this step they can be made modular and made to be modulated by other modules).

MIDI keyboard required. For Windows, just plug it into USB, and the application will automatically detect the first input device.

To use a custom oscillator, place the `.wav` file containing one period of the waveform in `resources/waveforms/{bitrate}_bit/{sample_rate}/`. In `src/applications.cpp`, create an `Oscillator` object with the extensionless filename as the argument. There is a `.wav` generator script at `other/wav_generator.ipynb`.

<!-- selecting audio output -->

## Hardware Setup
My setup (very simple): Teensy 4.1, Teensy Audio Shield, a USB module routing into the audio shield's USB ports (will replace), audio output device plugged into the audio shield's headphone jack.

Plan to add physical knobs & sliders, maybe also a keyboard.