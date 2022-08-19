# MidiPrinter
play music with ender 3 steppers

## Overview
There are 2 versions of this project, one originally written in python, and one later written in c++.

The Python one supports only single notes (played with one stepper), and plays in regards to the midi ticks and a speed argument.

The c++ version supports single notes and chords, but still cannot play notes which otherwise overlap with each other (limitations of the default ender3 firmware). The chord feature is highly experimental (enabled anyways) and needs more tweaking. Sweden sounds like this in the beginning <>.

The 2 different versions are on their own respective branches.
