#pragma once

#include <vector>
#include <string>
#include "Types.h"
#include "Stepper.h"

//class Stepper;

class Sound {
protected:
	midi_s_t m_duration;

public:
	Sound(midi_s_t duration);

	virtual std::string gc(std::vector<Stepper> &steppers) = 0;
};

class Delay : public Sound {
public:
	Delay(midi_s_t duration);

	std::string gc(std::vector<Stepper> &steppers) override;
};

class Note : public Sound {
	midi_key_t m_key;

public:
	Note(midi_s_t duration, midi_key_t key);

	std::string gc(std::vector<Stepper> &steppers) override;
};

class Chord : public Sound {
	std::vector<midi_key_t> m_keys;

public:
	//Chord(midi_s_t duration, std::initializer_list<midi_note_t> notes); //vector<midi_note_t> notes
	Chord(midi_s_t duration, std::vector<midi_key_t> keys); //vector<midi_note_t> notes

	std::string gc(std::vector<Stepper> &steppers) override;
};
