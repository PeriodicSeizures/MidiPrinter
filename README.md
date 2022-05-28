# MidiPrinter
play music with ender 3 steppers

## Usage
executable.py my-file.mid track speed

args: ender3_music.py ./nyan-cat.mid 1 4

I havn't been able to correctly set the tempo automatically, so youll have to that yourself with the custom speed arg, so something like nyan cat doesnt finish playing in 2 seconds.

I was planning multiple stepper support, but too much for now. Linear moves with the gcode interface require pytha and stuff, and having to keep track of notes is awkward, especially considering how move commands are blocking and notes could be played during a move.

Because of this, only simple midis work at the moment. I added a small detection to tell whether notes are being played at parallel (those will be skipped).
