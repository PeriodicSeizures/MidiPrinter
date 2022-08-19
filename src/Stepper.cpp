#include "Stepper.h"
#include <algorithm>

static char buf[16];

//Axis Stepper::axes = Axis::X;
//static Axis axes = Axis::X;
static Axis axes = 'X';

// TODO remove the auto incremental axis
Stepper::Stepper() : m_axis(axes++), m_pos(0), m_freqOffset(0) {
	//static_cast<Axis>(((char)axes)++)
	//auto a = ((char)axes);
	//a++;
	//axes = static_cast<Axis>(((char)axes)++);
	//if (this->m_axis == Axis::INVALID) {
	//	throw std::runtime_error("Max steppers allocated");
	//}
	if (this->m_axis > 'Z') {
		throw std::runtime_error("Max steppers allocated");
	}
}

char* Stepper::gc_frag(stepper_mm_min_t mm_min, midi_s_t duration) {
	// append the gcode line
	// the position is determined by the time and speed
	auto dist = distance(mm_min, duration);
	return gc_frag_d(mm_min, dist);
}

char* Stepper::gc_frag_d(stepper_mm_min_t mm_min, stepper_mm_t dist) {
	// gc = 'G1 X%5.5f F%5.5f' % (rel, mm_min)
	// now make rel signed or unsigned based on what is valid to move to
	if (m_pos - dist > 0)
		dist *= -1;
	m_pos += dist;

	std::snprintf(buf, sizeof(buf), "%c%.5f", m_axis, dist);
	return buf;
}

void Stepper::reset() {
	axes = 'X';
}