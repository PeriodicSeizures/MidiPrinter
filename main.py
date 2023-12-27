import sys
import mido
import pitches


def gc_tone(millis: float, freq: float):
    return f'M300 P{millis:.2f} S{freq:.5f}'
    #return f'M300 P{millis} S{max(freq, 31)}'


def gc_note(millis: float, note: int):
    val = pitches.pitchVals[note]
    if isinstance(val, list):
        return gc_tone(millis, val[0]) + ' ; ' + val[1]
    return gc_tone(millis, val)


def gc_pause(millis):
    return f'{gc_tone(millis, 31)} ; mute delay'

# https://mido.readthedocs.io/en/latest/midi_files.html#about-the-time-attribute
# the time attribute are the ticks since the last note (delta ticks)
def gc_midi(mid, **kwargs):
    # multiple notes can be played at once in midi
    # a stepper can only process 1 note at a time
    # so keep track of notes that are enabled in a map
    active_notes = 0

    # tempo multiplier will depend on ticks/beat and the current set midi temp
    # tempo_mul =

    commands = []

    print(f'# of Tracks: {len(mid.tracks)}')

    #tempo = [ t for m in mid.tracks[0]:  ][0]

    # steal tempo
    tempo = 500000 # mid.tracks[0]
    for m in mid.tracks[0]:
        if m.type == 'set_tempo':
            tempo = m.tempo
            break

    #

    # mid.tracks[1][0].type
    # speed_factor = 2, t = 0
    track_i = kwargs["track"] if "track" in kwargs else 0
    speed = kwargs["speed"] if "speed" in kwargs else 1
    for i, msg in enumerate(mid.tracks[track_i]):
        # the time for note on signifies the delay
        # a delay of zero means consecutive notes
        # a non-zero delay signifies a pause in between notes
        if msg.type == 'note_on':  # then find the corresponding note_off for the note
            #pause_t = msg.time
            pause_millis = mido.tick2second(msg.time, mid.ticks_per_beat, tempo) * 1000.0
            note: int = msg.note

            # skip parallel action (chords)
            if active_notes > 0:
                print('warning: parallel note detected (NYI)')
                continue

            active_notes += 1

            # pause for time
            if pause_millis > 0:
                commands.append(gc_pause(pause_millis * speed))

            for msg2 in mid.tracks[track_i][i:]:
                if msg2.type == 'note_off' and msg2.note == msg.note:
                    #play_t = msg2.time
                    #play_millis = get_note_millis(msg2.time, tempo)
                    play_millis = mido.tick2second(msg2.time, mid.ticks_per_beat, tempo) * 1000.0
                    # add the close
                    # get the 'time' for the note
                    # it signifies the time to play the note for

                    # determine if the action is simultaneous (time will be zero)
                    #if play_millis == 0:
                        # multi action
                        #pass

                    active_notes -= 1

                    #rel, mm_min = stepper1.process_note(note, play_t * speed)
                    commands.append(gc_note(play_millis * speed, note))
                    #gc += stepper1.gc_note(rel, mm_min)
                    break

        # elif msg.type == 'set_tempo':

    return '\n'.join(commands)


#midi = mido.MidiFile(sys.argv[1])


# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    midi = mido.MidiFile(sys.argv[1])
    print(gc_midi(midi, track=int(sys.argv[2]), speed=float(sys.argv[3])))
