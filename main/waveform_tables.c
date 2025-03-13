#include <stdint.h>
#include "waveform_tables.h"

uint_fast16_t sin_array[] = { 32768, 33572, 34375, 35178, 35979, 
    36779, 37576, 38370, 39160, 39947, 40729, 41507, 42280, 
    43046, 43807, 44561, 45307, 46046, 46778, 47500, 48214, 
    48919, 49614, 50298, 50972, 51636, 52287, 52927, 53555, 
    54171, 54773, 55362, 55938, 56500, 57047, 57580, 58098, 
    58600, 59087, 59558, 60013, 60452, 60874, 61279, 61666, 
    62037, 62389, 62724, 63041, 63340, 63620, 63882, 64125, 
    64349, 64553, 64739, 64906, 65053, 65181, 65289, 65378, 
    65447, 65496, 65526, 65535, 65526, 65496, 65447, 65378, 
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

uint_fast16_t sawtooth_array[] = { 0, 256, 512, 768, 1024, 
    1280, 1536, 1792, 2048, 2304, 2560, 2816, 3072, 3328, 3584, 
    3840, 4096, 4352, 4608, 4864, 5120, 5376, 5632, 5888, 6144, 
    6400, 6656, 6912, 7168, 7424, 7680, 7936, 8192, 8448, 8704, 
    8960, 9216, 9472, 9728, 9984, 10240, 10496, 10752, 11008, 
    11264, 11520, 11776, 12032, 12288, 12544, 12800, 13056, 
    13312, 13568, 13824, 14080, 14336, 14592, 14848, 15104, 
    15360, 15616, 15872, 16128, 16384, 16640, 16896, 17152, 
    17408, 17664, 17920, 18176, 18432, 18688, 18944, 19200, 
    19456, 19712, 19968, 20224, 20480, 20736, 20992, 21248, 
    21504, 21760, 22016, 22272, 22528, 22784, 23040, 23296, 
    23552, 23808, 24064, 24320, 24576, 24832, 25088, 25344, 
    25600, 25856, 26112, 26368, 26624, 26880, 27136, 27392, 
    27648, 27904, 28160, 28416, 28672, 28928, 29184, 29440, 
    29696, 29952, 30208, 30464, 30720, 30976, 31232, 31488, 
    31744, 32000, 32256, 32512, 32768, 33024, 33280, 33536, 
    33792, 34048, 34304, 34560, 34816, 35072, 35328, 35584, 
    35840, 36096, 36352, 36608, 36864, 37120, 37376, 37632, 
    37888, 38144, 38400, 38656, 38912, 39168, 39424, 39680, 
    39936, 40192, 40448, 40704, 40960, 41216, 41472, 41728, 
    41984, 42240, 42496, 42752, 43008, 43264, 43520, 43776, 
    44032, 44288, 44544, 44800, 45056, 45312, 45568, 45824, 
    46080, 46336, 46592, 46848, 47104, 47360, 47616, 47872, 
    48128, 48384, 48640, 48896, 49152, 49408, 49664, 49920, 
    50176, 50432, 50688, 50944, 51200, 51456, 51712, 51968, 
    52224, 52480, 52736, 52992, 53248, 53504, 53760, 54016, 
    54272, 54528, 54784, 55040, 55296, 55552, 55808, 56064, 
    56320, 56576, 56832, 57088, 57344, 57600, 57856, 58112, 
    58368, 58624, 58880, 59136, 59392, 59648, 59904, 60160, 
    60416, 60672, 60928, 61184, 61440, 61696, 61952, 62208, 
    62464, 62720, 62976, 63232, 63488, 63744, 64000, 64256, 
    64512, 64768, 65024, 65280 };

uint_fast16_t square_array[] = { 65535, 65535, 65535, 
    65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 
    65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 
    65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 
    65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 
    65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 
    65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 
    65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 
    65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 
    65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 
    65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 
    65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 
    65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 
    65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 
    65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 
    65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 
    65535, 65535, 65535, 65535, 65535, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0 };

uint_fast16_t triangle_array[] = { 32768, 33024, 33280, 
    33536, 33792, 34048, 34304, 34560, 34816, 35072, 35328, 
    35584, 35840, 36096, 36352, 36608, 36864, 37120, 37376, 
    37632, 37888, 38144, 38400, 38656, 38912, 39168, 39424, 
    39680, 39936, 40192, 40448, 40704, 40960, 41216, 41472, 
    41728, 41984, 42240, 42496, 42752, 43008, 43264, 43520, 
    43776, 44032, 44288, 44544, 44800, 45056, 45312, 45568, 
    45824, 46080, 46336, 46592, 46848, 47104, 47360, 47616, 
    47872, 48128, 48384, 48640, 48896, 65535, 65023, 64511, 
    63999, 63487, 62975, 62463, 61951, 61439, 60927, 60415, 
    59903, 59391, 58879, 58367, 57855, 57343, 56831, 56319, 
    55807, 55295, 54783, 54271, 53759, 53247, 52735, 52223, 
    51711, 51199, 50687, 50175, 49663, 49151, 48639, 48127, 
    47615, 47103, 46591, 46079, 45567, 45055, 44543, 44031, 
    43519, 43007, 42495, 41983, 41471, 40959, 40447, 39935, 
    39423, 38911, 38399, 37887, 37375, 36863, 36351, 35839, 
    35327, 34815, 34303, 33791, 33279, 32767, 32255, 31743, 
    31231, 30719, 30207, 29695, 29183, 28671, 28159, 27647, 
    27135, 26623, 26111, 25599, 25087, 24575, 24063, 23551, 
    23039, 22527, 22015, 21503, 20991, 20479, 19967, 19455, 
    18943, 18431, 17919, 17407, 16895, 16383, 15871, 15359, 
    14847, 14335, 13823, 13311, 12799, 12287, 11775, 11263, 
    10751, 10239, 9727, 9215, 8703, 8191, 7679, 7167, 6655, 
    6143, 5631, 5119, 4607, 4095, 3583, 3071, 2559, 2047, 1535, 
    1023, 511, 0, 512, 1024, 1536, 2048, 2560, 3072, 3584, 4096, 
    4608, 5120, 5632, 6144, 6656, 7168, 7680, 8192, 8704, 9216, 
    9728, 10240, 10752, 11264, 11776, 12288, 12800, 13312, 13824, 
    14336, 14848, 15360, 15872, 16384, 16896, 17408, 17920, 
    18432, 18944, 19456, 19968, 20480, 20992, 21504, 22016, 
    22528, 23040, 23552, 24064, 24576, 25088, 25600, 26112, 
    26624, 27136, 27648, 28160, 28672, 29184, 29696, 30208, 
    30720, 31232, 31744, 32256 };

uint32_t ratio_denom[] = { 1201, 1770, 657, 691, 786, 1247, 1326, 
    225, 1261, 664, 745, 1105, 748, 885, 657, 691, 393, 734, 
    663, 225, 73, 332, 473, 173, 374, 353, 413, 443, 393, 367, 
    257, 225, 73, 166, 68, 173, 187, 179, 176, 206, 209, 73, 
    149, 167, 73, 83, 34, 113, 54, 87, 88, 103, 23, 73, 27, 
    29, 73, 65, 17, 30, 27, 23, 44, 31, 23, 37, 27, 29, 39, 
    9, 17, 15, 27, 23, 22, 18, 23, 18, 17, 20, 17, 9, 17, 15, 
    13, 7, 11, 9, 3, 9, 5, 10, 8, 9, 6, 2, 7, 7, 3, 5, 3, 5, 
    5, 5, 4, 2, 3, 1, 3, 3, 3, 1, 3, 2, 2, 2, 2, 1, 2, 1, 2, 
    1, 1, 1, 1, 1, 1, 1};

uint32_t ratio_num[] = { 57, 89, 35, 39, 47, 79, 89, 16, 95, 
    53, 63, 99, 71, 89, 70, 78, 47, 93, 89, 32, 11, 53, 80, 
    31, 71, 71, 88, 100, 94, 93, 69, 64, 22, 53, 23, 62, 71, 
    72, 75, 93, 100, 37, 80, 95, 44, 53, 23, 81, 41, 70, 75, 
    93, 22, 74, 29, 33, 88, 83, 23, 43, 41, 37, 75, 56, 44, 
    75, 58, 66, 94, 23, 46, 43, 82, 74, 75, 65, 88, 73, 73, 
    91, 82, 46, 92, 86, 79, 45, 75, 65, 23, 73, 43, 91, 77, 
    92, 65, 23, 85, 90, 41, 72, 46, 81, 86, 91, 77, 41, 65, 
    23, 73, 77, 82, 29, 92, 65, 69, 73, 77, 41, 87, 46, 97, 
    51, 55, 58, 61, 65, 69, 73 };
