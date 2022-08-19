#pragma once

#include <stdexcept>
#include <algorithm>
#include <string>
#include <vector>
#include "Types.h"

class Stepper {
	//static Axis axes;

	const Axis m_axis;
	stepper_mm_t m_pos;
	stepper_us_t m_freqOffset;

public:
	Stepper();

	// converts
	char* gc_frag(stepper_mm_min_t mm_min, midi_s_t duration);
	char* gc_frag_d(stepper_mm_min_t mm_min, stepper_mm_t dist);

	static void reset();
};
