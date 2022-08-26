#include <stdexcept>
#include <cmath>
#include <vector>
#include "Types.h"
#include "Pitches.h"

stepper_mm_t distance(stepper_mm_min_t mm_min, midi_s_t duration) {
	return duration * (mm_min / 60.0);
}

// convert a midi_note_t to stepper_mm_min_t
// several conversions are performed in order:
//  note --> frequency --> microsteps --> min/step --> mm/min
stepper_mm_min_t cvt(midi_key_t note) {
	if (note > sizeof(pitches))
		throw std::runtime_error("out of range midinote");

	auto us_step = pitches[note];
	if (!us_step) {
		return 0;
	}

	auto min_step = (float)us_step / (float)60000000.0f; // converts to minutes per step
	auto min_mm = min_step * 25.0f; // converts to minutes per mm
	auto mm_min = 1.0f / min_mm; // reciprocal

	return mm_min;
}


// accepts a distance defined by 2 points in 2d space and initial stepper note rates
// returns the calculated point a to b rate, and assigns the out_vec the adjusted stepper relative displacements
stepper_mm_min_t extrapolate(stepper_mm_t euclidian_dist, // desired total distance to travel along straight line
	const std::vector<stepper_mm_min_t> &rates, 
	std::vector<stepper_mm_t>& adjusted_dist) {

	// this streamlines the distance algorithm by summing the square of all plane rates (axes)
	// then square rooting the result
	stepper_mm_min_t euclidian_rate = 0;
	for (auto r : rates) {
		euclidian_rate += (r * r);
	}
	euclidian_rate = std::sqrt(euclidian_rate); // get the euclidian distance between the points in space

	for (auto r : rates) {
		adjusted_dist.push_back(euclidian_dist * (r / euclidian_rate));
	}

	return euclidian_rate;
}