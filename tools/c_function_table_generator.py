from math import *
from fractions import Fraction

MAX_LINE_LEN = 60
NUM_VALUES = 256
STARTING_LINE = "uint_fast8_t triangle_array[] = { "
LEFT_SPACES = 4


def func_to_apply(x):
    return triangle(x)

orwell12_cents = [0.0, 157.14286, 271.42857, 385.71429, 428.57143,
             542.85714, 657.14286, 700.00000, 814.28571, 928.57143, 
             971.42857, 1085.71429]
mohajira = [0.0, 39.3, 193.9, 348.5, 387.8, 542.4, 580.6000, 697.0, 736.3, 890.9, 1122.6000, 1045.4]
rastplus_cents = [0.0, 147.8781, 197.7730, 348.4100, 361.8828, 499.4228, 552.3200, 705.6267, 859.9278, 899.7280, 1015.450, 1050.365099]
just_intonation = [0.0, 90.0, 204.0, 294.0, 408.0, 498.0, 558.0, 702.0, 792, 906, 996, 1110.0]
pythagorean = [0.0, 90.22500, 203.91000, 294.13500, 407.82000, 498.04500, 611.73001,
               701.95500, 792.18000, 905.86500, 996.09000, 1109.77500]

# this selects which tuning will be passed into tuning_denominator and tuning_numerator
tuning_cents = orwell12_cents


def tuning_denominator(x):
    x_rem = int(x / 12)
    x_mod = x % 12
    current_freq = 440 * 2**((x_rem - 69 + (tuning_cents[x_mod] / 100)) / 12)
    target_val = 44100 / (2**8 * current_freq)
    frac_res = Fraction(*target_val.as_integer_ratio()).limit_denominator(1200)
    return frac_res.numerator

def tuning_numerator(x):
    x_rem = int(x / 12)
    x_mod = x % 12
    current_freq = 440 * 2**((x_rem - 69 + (tuning_cents[x_mod] / 100)) / 12)
    target_val = 44100 / (2**8 * current_freq)
    frac_res = Fraction(*target_val.as_integer_ratio()).limit_denominator(1200)
    return frac_res.denominator

def triangle(x):
    if (x < 64):
        return 0x7f + (x * 2)
    elif (x < 64 * 3):
        x_new = x - 64;
        return 0xff - (x_new * 2)
    else:
        x_new = x - (64 * 3)
        return x_new * 2

def square(x):
    if (x < 128):
        return 0xff
    else:
        return 0x00

def sawtooth(x):
    return int(x)

def sinwave(x):
    return int(2**7 * (1 + sin(2 * pi * x / NUM_VALUES)))

#def func_to_apply(x):
#    if (x < 128):
#        current_freq = 440 * 2**((x - 69) / 12)
#        target_val = 44100 / (2**8 * current_freq)
#        frac_res = Fraction(*target_val.as_integer_ratio()).limit_denominator(100);
#        return frac_res.numerator #this will be applied as a numerator to the current time
#    else:
#        current_freq = 440 * 2**((x - 69 - 128) / 12)
#        target_val = 44100 / (2**8 * current_freq)
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
