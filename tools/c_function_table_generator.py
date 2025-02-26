from math import *
from fractions import Fraction

MAX_LINE_LEN = 60
NUM_VALUES = 64
STARTING_LINE = "uint_fast8_t test_pattern[SEQ_LEN] = { "
LEFT_SPACES = 4


def func_to_apply(x):
    return (x * 7) % 127
#def func_to_apply(x):
#    return int((2**16 / 2) * (1 + sin(2 * pi * x / NUM_VALUES)))
#def func_to_apply(x):
#    if (x < 128):
#        current_freq = 440 * 2**((x - 69) / 12)
#        target_val = 40000000 / (2**8 * current_freq)
#        frac_res = Fraction(*target_val.as_integer_ratio()).limit_denominator(100);
#        return frac_res.numerator #this will be applied as a numerator to the current time
#    else:
#        current_freq = 440 * 2**((x - 69 - 128) / 12)
#        target_val = 40000000 / (2**8 * current_freq)
#        frac_res = Fraction(*target_val.as_integer_ratio()).limit_denominator(100);
#        return frac_res.denominator #this will be applied as a denominator to the current time



with open("c_table.txt", "w") as f:

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
