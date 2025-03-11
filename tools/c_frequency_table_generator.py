from math import *
from fractions import Fraction

MAX_LINE_LEN = 60
NUM_VALUES = 128
STARTING_LINE = "const int freq_notes[] = { "
LEFT_SPACES = 4


#def func_to_apply(x):
#    return 440 * (pow(2, note_num-69) / 12);
def func_to_apply(x):
    if (x < 128):
        current_freq = 440 * 2**((x - 69) / 12)
        return int(current_freq)



with open("c_freq_table.txt", "w") as f:

    val_to_commit = STARTING_LINE;
    for i in range(0, NUM_VALUES):

        if (i == NUM_VALUES):
            break;
        elif (len(val_to_commit) >= MAX_LINE_LEN):
            print(val_to_commit, file=f, end='')
            print("\n", file=f, end='')
            val_to_commit = ' ' * LEFT_SPACES;

        val_to_commit = val_to_commit + str(func_to_apply(i)) + ', ';

    print(val_to_commit[:-2], file=f, end='')
    print(" };", file=f, end='')

    f.close()
