#include <unordered_map>
#include "MidiPrinter.h"
#include "MidiFile.h"
#include "Types.h"
#include "Stepper.h"

// locates a single note or series of synchronous notes
// async overlapping notes will be skipped
// returns any combined delay preceding the notes
static void findNoteBatch(smf::MidiEventList& track, int& i,
    std::vector<std::reference_wrapper<smf::MidiEvent>>& notes) {

    // we'll use the first note as the checker
    auto&& primer = track[i++];

    // Break out of non notes
    if (!primer.isNote())
        return;

    notes.push_back(primer);

    for (; i < track.size(); i++) {
        auto&& e = track[i];

        if (e.isNoteOn()) {

            // whether the note is in sync with the primer
            if (e.seconds == primer.seconds
                && e.getDurationInSeconds() == primer.getDurationInSeconds()) {
                // chord
                // TODO make the 3 a CONSTANT
                if (notes.size() < 3)
                    notes.push_back(e);
            }
            else {
                // skip async overlap
                // (this detects the inverse and breaks if there is no more overlapping)
                if (primer.seconds + primer.getDurationInSeconds() <= e.seconds)
                    break;
            }
        }
    }
}



MidiPrinter::MidiPrinter() {}

// http://www.music.mcgill.ca/~ich/classes/mumt306/StandardMIDIfileformat.html#BMA1_
void MidiPrinter::open(std::string filename) {
	smf::MidiFile midifile(filename);

    m_sounds.clear();

    if (midifile.isDeltaTicks())
        midifile.makeAbsoluteTicks();

    // linkNotePairs() works only for absolute
	midifile.doTimeAnalysis();
	midifile.linkNotePairs();

    int tracks = midifile.getTrackCount();
    for (int ti = 1; ti < tracks; ti++) {
        auto&& track = midifile[ti];

        midi_s_t lastSeconds = 0;
        midi_s_t lastDuration = 0;

        for (int ei = 0; ei < track.size();) {
            std::vector<std::reference_wrapper<smf::MidiEvent>> notes;
            {
                auto&& e = track[ei];
                findNoteBatch(track, ei, notes);
            }

            // testing single notes only for now
            if (!notes.empty()) {
                auto&& primer = notes[0].get();

                auto seconds = midi_s_t(primer.seconds);
                auto duration = midi_s_t(primer.getDurationInSeconds());
                auto diff = midi_s_t(seconds - (lastSeconds + lastDuration));
                // then delay
                if (duration < diff)
                    m_sounds.emplace_back(std::make_shared<Delay>(diff));

                lastSeconds = seconds;
                lastDuration = duration;

                if (notes.size() == 1) {
                    this->m_sounds.emplace_back(std::make_shared<Note>(
                        primer.getDurationInSeconds(), primer.getKeyNumber()));
                }
                else {
                    // gets keys instead of note events
                    std::vector<midi_key_t> keys;
                    for (auto n : notes) keys.push_back(n.get().getKeyNumber());

                    this->m_sounds.emplace_back(std::make_shared<Chord>(
                        primer.getDurationInSeconds(), keys));
                }
            }
        }
    }
}

std::string MidiPrinter::process() {
    //std::fill_n(m_steppers.begin(), 3, Stepper());

    // TODO tacky
    m_steppers.clear();
    m_steppers.push_back(Stepper());
    m_steppers.push_back(Stepper());
    m_steppers.push_back(Stepper());

    std::string result = "M117 Playing stepper song ; lcd\n" \
        "M107 ; disable part fan\n" \
        //"G28 ; autohome\n" \
        //"G0 Z10.0 F3000 ; raise gantry a bit\n" \
        //"G0 X100.0 F3000 ; move to center\n" 
        "G91 ; enable relative positioning\n" \
        "G4 P1000\n" \
        "\n" \
        "; delays/notes/chords ;\n";

    for (auto&& sound : m_sounds) {
         result += sound->gc(m_steppers) + std::string("\n");
    }

    result += "\n" \
        "G4 P1000\n" \
        "M84 X Y Z; disable steppers";

    return result;
}