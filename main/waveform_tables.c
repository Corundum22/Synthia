#include <stdint.h>
#include "waveform_tables.h"

const uint32_t sin_array[] = { 32768, 33572, 34375, 35178, 35979, 
    36779, 37576, 38370, 39160, 39947, 40729, 41507, 42280, 
    43046, 43807, 44561, 45307, 46046, 46778, 47500, 48214, 
    48919, 49614, 50298, 50972, 51636, 52287, 52927, 53555, 
    54171, 54773, 55362, 55938, 56500, 57047, 57580, 58098, 
    58600, 59087, 59558, 60013, 60452, 60874, 61279, 61666, 
    62037, 62389, 62724, 63041, 63340, 63620, 63882, 64125, 
    64349, 64553, 64739, 64906, 65053, 65181, 65289, 65378, 
    65447, 65496, 65526, 65536, 65526, 65496, 65447, 65378, 
    65289, 65181, 65053, 64906, 64739, 64553, 64349, 64125, 
    63882, 63620, 63340, 63041, 62724, 62389, 62037, 61666, 
    61279, 60874, 60452, 60013, 59558, 59087, 58600, 58098, 
    57580, 57047, 56500, 55938, 55362, 54773, 54171, 53555, 
    52927, 52287, 51636, 50972, 50298, 49614, 48919, 48214, 
    47500, 46778, 46046, 45307, 44561, 43807, 43046, 42280, 
    41507, 40729, 39947, 39160, 38370, 37576, 36779, 35979, 
    35178, 34375, 33572, 32768, 31963, 31160, 30357, 29556, 
    28756, 27959, 27165, 26375, 25588, 24806, 24028, 23255, 
    22489, 21728, 20974, 20228, 19489, 18757, 18035, 17321, 
    16616, 15921, 15237, 14563, 13899, 13248, 12608, 11980, 
    11364, 10762, 10173, 9597, 9035, 8488, 7955, 7437, 6935, 
    6448, 5977, 5522, 5083, 4661, 4256, 3869, 3498, 3146, 2811, 
    2494, 2195, 1915, 1653, 1410, 1186, 982, 796, 629, 482, 
    354, 246, 157, 88, 39, 9, 0, 9, 39, 88, 157, 246, 354, 482, 
    629, 796, 982, 1186, 1410, 1653, 1915, 2195, 2494, 2811, 
    3146, 3498, 3869, 4256, 4661, 5083, 5522, 5977, 6448, 6935, 
    7437, 7955, 8488, 9035, 9597, 10173, 10762, 11364, 11980, 
    12608, 13248, 13899, 14563, 15237, 15921, 16616, 17321, 
    18035, 18757, 19489, 20228, 20974, 21728, 22489, 23255, 
    24028, 24806, 25588, 26375, 27165, 27959, 28756, 29556, 
    30357, 31160, 31963 };

const uint32_t sawtooth_array[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 
    12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 
    26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 
    40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 
    54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 
    68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 
    82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 
    96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 
    108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 
    120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 
    132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 
    144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 
    156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 
    168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 
    180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 
    192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 
    204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 
    216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 
    228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 
    240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 
    252, 253, 254, 255 };

const uint32_t ratio_denom[] = { 745340, 1641517, 1413176, 369624, 
    788769, 1360142, 1337858, 51021, 1191896, 125000, 482663, 
    536564, 372670, 333715, 706588, 184812, 477812, 680071, 
    668929, 51021, 595948, 62500, 482663, 268282, 186335, 333715, 
    353294, 92406, 238906, 289925, 337843, 51021, 297974, 31250, 
    34859, 134141, 186335, 160093, 176647, 46203, 119453, 100221, 
    23649, 51021, 148987, 15625, 34859, 99973, 107501, 86811, 
    69169, 46203, 72051, 47426, 23649, 51021, 27841, 15625, 
    34859, 17084, 47181, 36641, 38309, 46203, 23701, 23713, 
    23649, 39063, 27841, 15625, 31172, 8542, 13139, 25085, 7715, 
    24357, 23701, 14541, 16681, 18535, 16366, 15625, 15586, 
    4271, 13139, 5778, 7715, 3641, 11732, 4586, 3484, 8271, 
    8183, 8434, 7793, 4271, 5599, 2889, 3791, 3641, 5866, 2293, 
    1742, 1993, 1646, 4217, 3687, 3836, 1829, 2889, 1962, 2103, 
    2933, 2293, 871, 1993, 823, 799, 2053, 1918, 1829, 1744, 
    981, 769, 1348, 769, 871, 710 };

const uint32_t ratio_num[] = { 39, 91, 83, 23, 52, 95, 99, 
    4, 99, 11, 45, 53, 39, 37, 83, 23, 63, 95, 99, 8, 99, 11, 
    90, 53, 39, 74, 83, 23, 63, 81, 100, 16, 99, 11, 13, 53, 
    78, 71, 83, 23, 63, 56, 14, 32, 99, 11, 26, 79, 90, 77, 
    65, 46, 76, 53, 28, 64, 37, 22, 52, 27, 79, 65, 72, 92, 
    50, 53, 56, 98, 74, 44, 93, 27, 44, 89, 29, 97, 100, 65, 
    79, 93, 87, 88, 93, 27, 88, 41, 58, 29, 99, 41, 33, 83, 
    87, 95, 93, 54, 75, 41, 57, 58, 99, 41, 33, 40, 35, 95, 
    88, 97, 49, 82, 59, 67, 99, 82, 33, 80, 35, 36, 98, 97, 
    98, 99, 59, 49, 91, 55, 66, 57 };
