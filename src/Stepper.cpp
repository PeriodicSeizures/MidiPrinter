#include "Stepper.h"
#include <algorithm>

static char buf[16];

Stepper::Stepper(Axis axis) 
	: m_axis(axis), m_pos(0), m_freqOffset(0) {}

char* Stepper::gc_frag(stepper_mm_min_t mm_min, midi_s_t duration) {
	// append the gcode line
	// the position is determined by the time and speed
	auto dist = distance(mm_min, duration);
	return gc_frag_d(mm_min, dist);
}

char* Stepper::gc_frag_d(stepper_mm_min_t mm_min, stepper_mm_t dist) {
	// gc = 'G1 X%5.5f F%5.5f' % (rel, mm_min)
	// now make rel signed or unsigned based on what is valid to move to
#ifdef STEPPERS_CENTER
	if (m_pos - dist > 0)
		dist *= -1;
#else
	if (std::abs(m_pos - dist) > 20) // a bit bigger buffer for smoother and practical
		dist *= -1;
#endif
	m_pos += dist;

	std::snprintf(buf, sizeof(buf), "%c%.5f", m_axis, dist);
	return buf;
}
