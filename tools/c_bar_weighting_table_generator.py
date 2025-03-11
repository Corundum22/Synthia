NUM_BARS = 20
MIDI_NOTES = 128

LOW_FREQ = 31
HIGH_FREQ = 900

freq_bands = [LOW_FREQ * (HIGH_FREQ / LOW_FREQ) ** (i / (NUM_BARS - 1)) for i in range(NUM_BARS)]
freq_bands = [round(f) for f in freq_bands]

def midi_to_freq(note):
    return 440.0 * 2 ** ((note - 69) / 12)

freq_map = []
for note in range(MIDI_NOTES):
    freq = midi_to_freq(note)
    
    if freq < LOW_FREQ:
        bar1, bar2, weight1, weight2 = 0, 0, 255, 0
    elif freq > HIGH_FREQ:
        bar1, bar2, weight1, weight2 = 19, 19, 255, 0
    else:
        bar1 = max(0, min(NUM_BARS - 2, next((i for i, f in enumerate(freq_bands) if f > freq), NUM_BARS) - 1))
        bar2 = min(NUM_BARS - 1, bar1 + 1)
        
        range_width = freq_bands[bar2] - freq_bands[bar1]
        weight2 = int(((freq - freq_bands[bar1]) / range_width) * 255)
        weight1 = 255 - weight2
    
    freq_map.append((bar1, bar2, weight1, weight2))

with open("freq_map_output.txt", "w") as f:
    f.write("const freq_mapping freq_map[128] = {\n")
    for i, (b1, b2, w1, w2) in enumerate(freq_map):
        f.write(f"    {{ {b1}, {b2}, {w1}, {w2} }}" + ("," if i < MIDI_NOTES - 1 else "") + "\n")
    f.write("};\n")