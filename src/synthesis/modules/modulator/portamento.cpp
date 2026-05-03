#include "portamento.h"

#include "utils/config.h"

using namespace synthesis;

Portamento::Portamento()
	: time{}
{
	;
}

void Portamento::generate_buf() {
	;
}

void Portamento::change_note(uint8_t note) {
	printf("changing note\n");
}

void Portamento::set_time(double time_ms) {
	time = time_ms * (1000.0 / config::sample_rate);
}