NUM_BARS = 20
LOW_PASS_MAX = 256

LOW_FREQ = 31
HIGH_FREQ = 900

freq_bands = [LOW_FREQ * (HIGH_FREQ / LOW_FREQ) ** (i / (NUM_BARS - 1)) for i in range(NUM_BARS)]
freq_bands = [round(f) for f in freq_bands]

def midi_to_freq(note):
    return 440.0 * 2 ** ((note - 69) / 12)

lp_map = []
for low_pass_val in range(LOW_PASS_MAX):

    vin = low_pass_val/LOW_PASS_MAX * 3.3

    f_cutoff = max(8675*(vin - 1), 0)

    #freq = midi_to_freq(note)
    
    
    if f_cutoff < LOW_FREQ:
        bar_val = 0
    elif f_cutoff > HIGH_FREQ:
        bar_val = int((((NUM_BARS - 1) + (f_cutoff/midi_to_freq(135))))/NUM_BARS * 65535)
    else:

        bar1 = max(0, min(NUM_BARS - 2, next((i for i, f in enumerate(freq_bands) if f > f_cutoff), NUM_BARS) - 1))
        bar2 = min(NUM_BARS - 1, bar1 + 1)
        
        range_width = freq_bands[bar2] - freq_bands[bar1]
        weight2 = int(((f_cutoff - freq_bands[bar1]) / range_width) * 255)
        weight1 = 255 - weight2


        bar_val = (bar1 * NUM_BARS/65535 + weight2) * 255

    
    lp_map.append(bar_val)

with open("low_pass_output.txt", "w") as f:
    f.write("const uint_16t low_pass_map[128] = {\n")
    for i, num in enumerate(lp_map):
        f.write(f"    {num}" + ("," if i < LOW_PASS_MAX - 1 else "") + "\n")
    f.write("};\n")