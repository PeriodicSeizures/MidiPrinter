import math
import sys
import json
import mido
import argparse

import pitches


class StepperPlayer:
    available = ['X', 'Y', 'Z']

    def __init__(self):
        self.axis = StepperPlayer.available.pop(0)
        # also need to track position
        self.pos = 0

    # get the relative offset AND mm/min for a given note
    def process_note(self, note: int, ms: int) -> (float, float):
        mm_min = note2mm_min(note)
        # append the gcode line
        # the position is determined by the time and speed
        rel = (ms / 1000) * (mm_min / 60)
        # gc = 'G1 X%5.5f F%5.5f' % (rel, mm_min)
        # now make rel signed or unsigned based on what is valid to move to
        if self.pos - rel > 0:
            rel *= -1
        self.pos += rel
        return rel, mm_min

    # generate a gcode for combined stepper move
    # notes is a variadic template
    """
    @staticmethod
    def gen_parallel_notes(*notes):
        gc_moves = []
        total_mm_min = 0
        for note in notes:
            # convert single axis mm_min to all combined for pytha
            total_mm_min += pow(note['mm_min'], 2)
            gc_moves.append(note['axis'] + str(note['rel']))

        # convert the combined non-pytha movement to pytha
        total_mm_min = math.sqrt(total_mm_min)

        return "G1 " + " ".join(gc_moves) + (' F%5.5f' % total_mm_min)
    """

    def gc_note(self, rel: float, mm_min: float):
        return 'G1 %s%5.5f F%5.5f\n' % (self.axis, rel, mm_min)


# Convert a note index to mm / min
def note2mm_min(note: int) -> float:
    us_step = pitches.pitchVals[note]  # delay btw each step

    # convert to min / step
    min_step = us_step / 60000000
    # convert to min / mm
    min_mm = min_step * 25
    # invert to get mm / min
    mm_min = 1 / min_mm

    return mm_min


def gc_header():
    lcd = "Playing example song"

    return f'M117 {lcd} ; lcd\n' \
           'M107 ; disable fan\n' \
           'G28 ; home\n' \
           'G1 Z10.0 F3000 ; raise\n' \
           'G1 X100.0 F3000 ; center\n' \
           'G91 ; rel pos\n'  # relative positions


# determine which stepper to control
def gc_pause(ms):
    # moves are executed consecutively, one after another waiting for it to complete
    # to generate a wait, no notes must be played during the time
    # so only if no notes are being played, then perform a dwell command
    return f'G4 P{ms}\n'


# given ticks/beat and the set temp, determine the tempo multiplier for print head and pauses
# def calc_tempo_mul(ticks_beat: int, tempo_ticks: int):



# https://mido.readthedocs.io/en/latest/midi_files.html#about-the-time-attribute
# the time attribute are the ticks since the last note (delta ticks)
def gc_midi(mid, **kwargs):
    # multiple notes can be played at once in midi
    # a stepper can only process 1 note at a time
    # so keep track of notes that are enabled in a map
    active_notes = 0

    # tempo multiplier will depend on ticks/beat and the current set midi temp
    # tempo_mul =

    gc = gc_header()

    stepper1 = StepperPlayer()

    # mid.tracks[1][0].type
    # speed_factor = 2, t = 0
    track_i = kwargs["track"] if "track" in kwargs else 0
    speed = kwargs["speed"] if "speed" in kwargs else 1
    for i, msg in enumerate(mid.tracks[track_i]):
        # the time for note on signifies the delay
        # a delay of zero means consecutive notes
        # a non-zero delay signifies a pause in between notes
        if msg.type == 'note_on':  # then find the corresponding note_off for the note
            pause_t = msg.time
            note: int = msg.note

            # skip parallel action (chords)
            if active_notes > 0:
                continue

            active_notes += 1

            # pause for time
            if pause_t > 0:
                gc += gc_pause(pause_t * speed)

            for msg2 in mid.tracks[track_i][i:]:
                if msg2.type == 'note_off' and msg2.note == msg.note:
                    play_t = msg2.time
                    # add the close
                    # get the 'time' for the note
                    # it signifies the time to play the note for

                    # determine if the action is simultaneous (time will be zero)
                    if play_t == 0:
                        # multi action
                        pass

                    active_notes -= 1

                    rel, mm_min = stepper1.process_note(note, play_t * speed)
                    gc += stepper1.gc_note(rel, mm_min)
                    break

        # elif msg.type == 'set_tempo':


    return gc


def gen_test_e6():
    # index 64
    mm_min = note2mm_min(88)  # 64
    # move to a position over time mm_min
    gc = gc_header()
    gc += 'G1 X15.0 F%5.5f' % mm_min  # move 15 mm over mm_min duration
    return gc


midi = mido.MidiFile(sys.argv[1])
print(gc_midi(midi, track=int(sys.argv[2]), speed=float(sys.argv[3])))

# print(gen_test_e6())
