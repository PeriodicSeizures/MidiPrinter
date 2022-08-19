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
		throw std::runtime_error("bad note");

	auto us_step = pitches[note];
	if (!us_step) {
		return 0;
	}

	auto min_step = (float)us_step / (float)60000000.0f; // converts to minutes per step
	auto min_mm = min_step * 25.0f; // converts to minutes per mm
	auto mm_min = 1.0f / min_mm; // reciprocal

	return mm_min;
}

// 1 note
//static void extrapolate(stepper_mm_t dist, stepper_mm_min_t h_rate,
	//stepper_mm_t *h_dist)


// TODO reduce this to 1 looped function?
// 
// 2 note chord
//  dist = duration to move
//  h_rate = horizontal rate of motion
//  v_rate = vertical rate of motion
// ---
//  out h_dist = resultant horizontal offset
//  out v_dist = resultant vertical offset
//  out rate = resultant diagonal rate
[[deprecated("Use extrapolate(stepper_mm_t, vec, vec) instead")]]
void extrapolate(stepper_mm_t dist, stepper_mm_min_t h_rate, stepper_mm_min_t v_rate,
	stepper_mm_t& h_dist, stepper_mm_t& v_dist, stepper_mm_min_t& rate) {
	rate = std::sqrt(h_rate * h_rate + v_rate * v_rate);

	h_dist = dist * (h_rate / rate); // using proportions instead of trig
	v_dist = dist * (v_rate / rate);
}

// 3 note chord
//  dist = duration to move
//  h_rate = horizontal rate of motion
//  v_rate = vertical rate of motion
//  r_rate = raiser rate of motion (up/down)
// ---
//  out h_dist = resultant horizontal offset
//  out v_dist = resultant vertical offset
//  out r_dist = resultant raiser offset (up/down)
//  out rate = resultant diagonal rate
[[deprecated("Use extrapolate(stepper_mm_t, vec, vec) instead")]]
void extrapolate(stepper_mm_t dist, stepper_mm_min_t h_rate, stepper_mm_min_t v_rate, stepper_mm_min_t r_rate,
	stepper_mm_t& h_dist, stepper_mm_t& v_dist, stepper_mm_t& r_dist, stepper_mm_min_t& rate) {
	rate = std::sqrt(h_rate * h_rate + v_rate * v_rate + r_rate * r_rate);

	h_dist = dist * (h_rate / rate); // using proportions instead of trig
	v_dist = dist * (v_rate / rate);
	r_dist = dist * (r_rate / rate);
}

// accepts a distance defined by 2 points in 2d space and initial stepper note rates
// returns the calculated point a to b rate, and assigns the out_vec the adjusted stepper relative displacements
stepper_mm_min_t extrapolate(stepper_mm_t euclidian_dist, 
	const std::vector<stepper_mm_min_t> &rates, 
	std::vector<stepper_mm_t>& adjusted_dist) {

	stepper_mm_min_t rate = 0;
	for (auto r : rates) {
		rate += (r * r);
	}
	rate = std::sqrt(rate);

	for (auto r : rates) {
		adjusted_dist.push_back(euclidian_dist * (r / rate));
	}

	//for (int i = 0; i < rates.size(); i++) {
	//	adjusted_dist[i] = euclidian_dist * (rates[i] / rate);
	//}

	return rate;
}