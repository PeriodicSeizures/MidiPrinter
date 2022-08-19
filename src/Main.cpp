#include <iostream>
#include "MidiPrinter.h"
#include "MidiFile.h"

void print(smf::MidiFile& midifile) {
    int tracks = midifile.getTrackCount();
    std::cout << "TPQ: " << midifile.getTicksPerQuarterNote() << "\n";
    if (tracks > 1) std::cout << "TRACKS: " << tracks << "\n";
    for (int track = 1; track < tracks; track++) {
        if (tracks > 1) std::cout << "\nTrack " << track << "\n";
        std::cout << "Tick\tSeconds\tDur\tMessage" << "\n";
        for (int event = 0; event < midifile[track].size(); event++) {
            std::cout << std::dec << midifile[track][event].tick;
            std::cout << '\t' << std::dec << midifile[track][event].seconds;
            std::cout << '\t';
            if (midifile[track][event].isNoteOn())
                std::cout << midifile[track][event].getDurationInSeconds();
            std::cout << '\t' << std::hex;
            for (int i = 0; i < midifile[track][event].size(); i++)
                std::cout << (int)midifile[track][event][i] << ' ';
            std::cout << "\n";
        }
    }
}

int mdain(int argc, char** args) {
    smf::MidiFile midifile;
    midifile.read("data/imperial-basic.mid");
    //midifile.read("data/nyan-cat-basic.mid");
    //midifile.read("data/test_sorting.mid");
    
    midifile.doTimeAnalysis();
    midifile.linkNotePairs();

    print(midifile);

    //midifile.de
    
    return 0;
}

int main(int argc, char** args) {

	MidiPrinter player;
	player.open("data/imperial-basic.mid");
    player.open("data/sweden.mid");
	//player.open("data/nyan-cat-basic.mid");

	std::cout << player.process();

	return 0;
}