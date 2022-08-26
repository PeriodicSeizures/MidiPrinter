#include <stdexcept>
#include <cmath>
#include <sstream>
#include <string>
#include <format>
#include <iomanip>
#include "Types.h"
#include "Sound.h"
#include "Pitches.h"
#include "Stepper.h"

//static char buf[64];

Sound::Sound(midi_s_t duration) : m_duration(duration) {
	if (duration <= 0) //std::numeric_limits<midi_s_t>().epsilon())
		throw std::runtime_error("Duration must not be 0");
}
Delay::Delay(midi_s_t duration) : Sound(duration) {}
Note::Note(midi_s_t duration, midi_key_t note) : m_key(note), Sound(duration) {}
//Chord::Chord(midi_s_t duration, std::initializer_list<midi_note_t> notes) : m_notes(notes), Sound(duration) {}
Chord::Chord(midi_s_t duration, std::vector<midi_key_t> notes) : m_keys(notes), Sound(duration) {}

std::string Delay::gc(std::vector<Stepper> &steppers) {
	//if (!m_duration)
		//return nullptr;

	return std::format("G4 P{}", (unsigned int)(m_duration * 1000.0));

	//snprintf(buf, sizeof(buf), "G4 P%u ; dwell", (unsigned int) (m_duration * 1000.0));
	//return std::string(buf);
}

std::string Note::gc(std::vector<Stepper> &steppers) {
	auto &&stepper = steppers[0];

	auto mm_min = cvt(m_key);

	auto frag = stepper.gc_frag(mm_min, m_duration);

	return std::format("G0 {} F{:.5f}", frag, mm_min);

	//std::snprintf(buf, sizeof(buf), "G0 %s F%.5f", frag, mm_min);
	//return std::string(buf);
}

std::string Chord::gc(std::vector<Stepper> &steppers) {
	std::vector<stepper_mm_min_t> rates;

	// calculate the A --> B distance
	stepper_mm_t euclidian_dist = 0;
	for (auto key : m_keys) {
		auto mm_min = cvt(key);
		auto mm = distance(mm_min, m_duration);

		rates.push_back(mm_min);

		euclidian_dist += (mm * mm);
	}
	euclidian_dist = std::sqrt(euclidian_dist);

	std::vector<stepper_mm_t> adj_dists;
	auto adj_rate = extrapolate(euclidian_dist, rates, adj_dists);

	std::stringstream s;
	s << "G0";

	// iterate the newly calculated adjusted_dists
	for (int i = 0; i < adj_dists.size(); i++) {
		auto adj_dist = adj_dists[i];
		s << " " << steppers[i].gc_frag_d(rates[i], adj_dist);
	}

	s << std::format(" F{:.5f}", adj_rate);

	return s.str();
}