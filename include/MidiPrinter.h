#pragma once

#include <string>
#include <memory>
#include "Types.h"
#include "Sound.h"
#include "Stepper.h"
#include <array>

class MidiPrinter {

	// what notes?
	// will accept a buffer of singles/chords, with durations

	//Stepper steppers[3];
	//std::array<Stepper, 3> steppers;
	std::vector<Stepper> m_steppers;
	std::vector<std::shared_ptr<Sound>> m_sounds;

public:
	MidiPrinter();

	void open(std::string filename);
	std::string process();

};
