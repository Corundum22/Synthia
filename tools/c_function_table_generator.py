from math import *

MAX_LINE_LEN = 60
NUM_VALUES = 256
STARTING_LINE = "uint8_t the_array[] = { "
LEFT_SPACES = 4

def func_to_apply(x):
    return int(127.5 * (1 + sin(2 * pi * x / NUM_VALUES)))

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
