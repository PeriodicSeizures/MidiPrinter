#pragma once

#include <stdint.h>
#include <vector>

//enum class Axis : char {
//	X = 'X',
//	Y = 'Y',
//	Z = 'Z',
//	//E = 'E', // extruder isnt feasable because theres messy filament
//	INVALID // = CHAR_MAX
//};

using Axis = char;

using midi_ms_t = uint32_t;		// duration
using midi_s_t = double;
using midi_key_t = uint8_t;	// index into pitches (same as midi)
using midi_vel_t = uint8_t;
using stepper_us_t = uint16_t;
using stepper_mm_min_t = double;		// rate of motion
using stepper_mm_t = double;			// length

// convert a midi_key_t to stepper_mm_min_t
// several conversions are performed in order:
//  note --> frequency --> microsteps --> min/step --> mm/min
stepper_mm_min_t cvt(midi_key_t note);

// get the distance covered by a speed performed over a duration
stepper_mm_t distance(stepper_mm_min_t mm_min, midi_s_t duration);

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
void extrapolate(stepper_mm_t dist, stepper_mm_min_t h_rate, stepper_mm_min_t v_rate,
	stepper_mm_t& h_dist, stepper_mm_t& v_dist, stepper_mm_min_t& rate);

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
void extrapolate(stepper_mm_t dist, stepper_mm_min_t h_rate, stepper_mm_min_t v_rate, stepper_mm_min_t r_rate,
	stepper_mm_t& h_dist, stepper_mm_t& v_dist, stepper_mm_t& r_dist, stepper_mm_min_t& rate);


// accepts a distance defined by 2 points in 2d space and initial stepper note rates
// returns the calculated point a to b rate, and assigns the out_vec the adjusted stepper relative displacements
stepper_mm_min_t extrapolate(stepper_mm_t euclidian_dist, const std::vector<stepper_mm_min_t> &rates, std::vector<stepper_mm_t>& adjusted_dist);