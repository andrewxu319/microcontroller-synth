#include "serge_wavefolder.h"

#include "utils/math.h"

using namespace synthesis;

SergeWavefolder::SergeWavefolder()
    : Wavefolder{}, stages{}
{
    for (uint8_t i{}; i < stages.size(); i++) {
        stages[i] = std::move(std::make_unique<SergeStage>());
    }
}

void SergeWavefolder::init() {
    stages[0]->add_buf(out_buf, SergeWavefolder::BufType::AUDIO); // axpy'ed buffer is stored in out_buf
    for (uint8_t i{ 1 }; i < stages.size(); i++) {
        stages[i]->add_buf(stages[i - 1]->get_out_buf(), SergeWavefolder::BufType::AUDIO);
    }
}

void SergeWavefolder::generate_buf() {
    Wavefolder::generate_buf(); // store axpy output in out_buf

    for (uint8_t i{}; i < stages.size(); i++) {
        stages[i]->generate_buf();
    }

    memcpy(out_buf, stages.back()->get_out_buf(), config::buffer_size * sizeof(float_s));
}