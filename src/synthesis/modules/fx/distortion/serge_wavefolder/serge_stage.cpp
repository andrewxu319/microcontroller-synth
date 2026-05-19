#include "serge_stage.h"

#include "utils/math.h"

#include <cmath>

using namespace synthesis;

SergeStage::SergeStage()
    : Fx(in_bufs), last_sample{}
{
}

void SergeStage::generate_buf() {
    math::vec_sub_float_s(audio_in_buf + 1, audio_in_buf, out_buf + 1, config::buffer_size - 1);
    out_buf[0] = audio_in_buf[0] - last_sample;

    if (std::abs(out_buf[0]) < anti_aliasing_thresh) {
        out_buf[0] = f_((audio_in_buf[0] + last_sample) / 2.0f);
    }
    else {
        out_buf[0] = (F_(audio_in_buf[0]) - F_(last_sample)) / out_buf[0];
    }

    for (size_t i{ 1 }; i < config::buffer_size; i++) {
        if (out_buf[i] < anti_aliasing_thresh) {
            out_buf[i] = f_((audio_in_buf[i] + audio_in_buf[i - 1]) / 2.0f);
        }
        else {
            out_buf[i] = (F_(audio_in_buf[i]) - F_(audio_in_buf[i - 1])) / out_buf[i];
        }
    }

    last_sample = audio_in_buf[config::buffer_size - 1];
}

float_s SergeStage::f_(float_s x) {
    int8_t lambda{ signbit(x) ? -1 : 1 };
    return x - 2 * lambda * eta * V_T * W_(R_1 * I_S / (eta * V_T) * std::exp(lambda * x / (eta * V_T)));
}

float_s SergeStage::F_(float_s x) {
    int8_t lambda{ signbit(x) ? -1 : 1 };
    float_s Psi_2{ W_(R_1 * I_S / (eta * V_T) * std::exp(lambda * x / (eta * V_T))) };
    return x * x / 2.0f - (eta * V_T) * (eta * V_T) * Psi_2 * (Psi_2 + 2.0f);
}

float_s SergeStage::W_(float_s x) {
    float_s w{};

    // initial guess
    if (x < 0.14546954290661823f) {
        // maybe do everything in double precision?
        float_s num{ 1.0f + 5.931375839364438f * x + 11.39220550532913f * static_cast<float_s>(std::pow(x, 2)) +  7.33888339911111f * static_cast<float_s>(std::pow(x, 3)) + 0.653449016991959f * static_cast<float_s>(std::pow(x, 4))};
        float_s den{ 1.0f + 6.931373689597704f * x + 16.82349461388016f * static_cast<float_s>(std::pow(x, 2)) + 16.43072324143226f * static_cast<float_s>(std::pow(x, 3)) + 5.115235195211697f * static_cast<float_s>(std::pow(x, 4))};
        w = x * num / den;
    }
    else if (x < 8.706658967856612f) {
        float_s num{ 1.0f + 2.4450530707265568f * x + 1.3436642259582265f * static_cast<float_s>(std::pow(x, 2)) + 0.14844005539759195f * static_cast<float_s>(std::pow(x, 3)) + 0.0008047501729130f * static_cast<float_s>(std::pow(x, 4))};
        float_s den{ 1.0f + 3.4447089864860025f * x + 3.2924898573719523f * static_cast<float_s>(std::pow(x, 2)) + 0.9164600188031222f * static_cast<float_s>(std::pow(x, 3))  + 0.05306864044833221f * static_cast<float_s>(std::pow(x, 4))};
        w = x * num / den;
    }
    else {
        float_s a{ std::log(x) };
        float_s b{ std::log(a) };    
        float_s ia{ 1.0f / a };
        w = a - b + (b * ia)  *  0.5f * b * (b - 2) * (ia * ia) + (1.0f / 6.0f) * (2.0f * b * b - 9.0f * b + 6.0f) * (ia * ia * ia);
    }

    // compute (20 iterations)
    for (uint8_t m{}; m < 20; m++) {
        float_s w1{ w + 1 };
        float_s z{ std::log(x) - std::log(w) - w };
        float_s q{ 2.0f * w1 * (w1 + (2.0f / 3.0f) * z) };
        float_s e{ (z / w1) * ((q - z) / (q - 2.0f * z)) };
        w *= 1.0f + e;

        if (std::abs(e) < lambert_W_thresh) {
            break;
        }
    }

    return w;
}
