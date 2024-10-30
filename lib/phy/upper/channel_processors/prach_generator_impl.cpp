/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "prach_generator_impl.h"
#include "srsran/phy/support/complex_exponential_table.h"
#include "srsran/ran/prach/prach_cyclic_shifts.h"
#include "srsran/ran/prach/prach_preamble_information.h"
#include "srsran/srsvec/sc_prod.h"

using namespace srsran;

// Complex exponential look-up table for short preambles.
static const complex_exponential_table
    cexp_table_short(4 * prach_constants::SHORT_SEQUENCE_LENGTH,
                     std::sqrt(static_cast<float>(prach_constants::SHORT_SEQUENCE_LENGTH)));
// Complex exponential look-up table for long preambles.
static const complex_exponential_table
    cexp_table_long(4 * prach_constants::LONG_SEQUENCE_LENGTH,
                    std::sqrt(static_cast<float>(prach_constants::LONG_SEQUENCE_LENGTH)));

unsigned prach_generator_impl::get_sequence_number_long(unsigned root_sequence_index)
{
  static const std::array<uint16_t, LONG - 1> lut = {
      129, 710, 140, 699, 120, 719, 210, 629, 168, 671, 84,  755, 105, 734, 93,  746, 70,  769, 60,  779, 2,   837, 1,
      838, 56,  783, 112, 727, 148, 691, 80,  759, 42,  797, 40,  799, 35,  804, 73,  766, 146, 693, 31,  808, 28,  811,
      30,  809, 27,  812, 29,  810, 24,  815, 48,  791, 68,  771, 74,  765, 178, 661, 136, 703, 86,  753, 78,  761, 43,
      796, 39,  800, 20,  819, 21,  818, 95,  744, 202, 637, 190, 649, 181, 658, 137, 702, 125, 714, 151, 688, 217, 622,
      128, 711, 142, 697, 122, 717, 203, 636, 118, 721, 110, 729, 89,  750, 103, 736, 61,  778, 55,  784, 15,  824, 14,
      825, 12,  827, 23,  816, 34,  805, 37,  802, 46,  793, 207, 632, 179, 660, 145, 694, 130, 709, 223, 616, 228, 611,
      227, 612, 132, 707, 133, 706, 143, 696, 135, 704, 161, 678, 201, 638, 173, 666, 106, 733, 83,  756, 91,  748, 66,
      773, 53,  786, 10,  829, 9,   830, 7,   832, 8,   831, 16,  823, 47,  792, 64,  775, 57,  782, 104, 735, 101, 738,
      108, 731, 208, 631, 184, 655, 197, 642, 191, 648, 121, 718, 141, 698, 149, 690, 216, 623, 218, 621, 152, 687, 144,
      695, 134, 705, 138, 701, 199, 640, 162, 677, 176, 663, 119, 720, 158, 681, 164, 675, 174, 665, 171, 668, 170, 669,
      87,  752, 169, 670, 88,  751, 107, 732, 81,  758, 82,  757, 100, 739, 98,  741, 71,  768, 59,  780, 65,  774, 50,
      789, 49,  790, 26,  813, 17,  822, 13,  826, 6,   833, 5,   834, 33,  806, 51,  788, 75,  764, 99,  740, 96,  743,
      97,  742, 166, 673, 172, 667, 175, 664, 187, 652, 163, 676, 185, 654, 200, 639, 114, 725, 189, 650, 115, 724, 194,
      645, 195, 644, 192, 647, 182, 657, 157, 682, 156, 683, 211, 628, 154, 685, 123, 716, 139, 700, 212, 627, 153, 686,
      213, 626, 215, 624, 150, 689, 225, 614, 224, 615, 221, 618, 220, 619, 127, 712, 147, 692, 124, 715, 193, 646, 205,
      634, 206, 633, 116, 723, 160, 679, 186, 653, 167, 672, 79,  760, 85,  754, 77,  762, 92,  747, 58,  781, 62,  777,
      69,  770, 54,  785, 36,  803, 32,  807, 25,  814, 18,  821, 11,  828, 4,   835, 3,   836, 19,  820, 22,  817, 41,
      798, 38,  801, 44,  795, 52,  787, 45,  794, 63,  776, 67,  772, 72,  767, 76,  763, 94,  745, 102, 737, 90,  749,
      109, 730, 165, 674, 111, 728, 209, 630, 204, 635, 117, 722, 188, 651, 159, 680, 198, 641, 113, 726, 183, 656, 180,
      659, 177, 662, 196, 643, 155, 684, 214, 625, 126, 713, 131, 708, 219, 620, 222, 617, 226, 613, 230, 609, 232, 607,
      262, 577, 252, 587, 418, 421, 416, 423, 413, 426, 411, 428, 376, 463, 395, 444, 283, 556, 285, 554, 379, 460, 390,
      449, 363, 476, 384, 455, 388, 451, 386, 453, 361, 478, 387, 452, 360, 479, 310, 529, 354, 485, 328, 511, 315, 524,
      337, 502, 349, 490, 335, 504, 324, 515, 323, 516, 320, 519, 334, 505, 359, 480, 295, 544, 385, 454, 292, 547, 291,
      548, 381, 458, 399, 440, 380, 459, 397, 442, 369, 470, 377, 462, 410, 429, 407, 432, 281, 558, 414, 425, 247, 592,
      277, 562, 271, 568, 272, 567, 264, 575, 259, 580, 237, 602, 239, 600, 244, 595, 243, 596, 275, 564, 278, 561, 250,
      589, 246, 593, 417, 422, 248, 591, 394, 445, 393, 446, 370, 469, 365, 474, 300, 539, 299, 540, 364, 475, 362, 477,
      298, 541, 312, 527, 313, 526, 314, 525, 353, 486, 352, 487, 343, 496, 327, 512, 350, 489, 326, 513, 319, 520, 332,
      507, 333, 506, 348, 491, 347, 492, 322, 517, 330, 509, 338, 501, 341, 498, 340, 499, 342, 497, 301, 538, 366, 473,
      401, 438, 371, 468, 408, 431, 375, 464, 249, 590, 269, 570, 238, 601, 234, 605, 257, 582, 273, 566, 255, 584, 254,
      585, 245, 594, 251, 588, 412, 427, 372, 467, 282, 557, 403, 436, 396, 443, 392, 447, 391, 448, 382, 457, 389, 450,
      294, 545, 297, 542, 311, 528, 344, 495, 345, 494, 318, 521, 331, 508, 325, 514, 321, 518, 346, 493, 339, 500, 351,
      488, 306, 533, 289, 550, 400, 439, 378, 461, 374, 465, 415, 424, 270, 569, 241, 598, 231, 608, 260, 579, 268, 571,
      276, 563, 409, 430, 398, 441, 290, 549, 304, 535, 308, 531, 358, 481, 316, 523, 293, 546, 288, 551, 284, 555, 368,
      471, 253, 586, 256, 583, 263, 576, 242, 597, 274, 565, 402, 437, 383, 456, 357, 482, 329, 510, 317, 522, 307, 532,
      286, 553, 287, 552, 266, 573, 261, 578, 236, 603, 303, 536, 356, 483, 355, 484, 405, 434, 404, 435, 406, 433, 235,
      604, 267, 572, 302, 537, 309, 530, 265, 574, 233, 606, 367, 472, 296, 543, 336, 503, 305, 534, 373, 466, 280, 559,
      279, 560, 419, 420, 240, 599, 258, 581, 229, 610};

  return static_cast<unsigned>(lut[root_sequence_index % lut.size()]);
}

unsigned prach_generator_impl::get_sequence_number_short(unsigned root_sequence_index)
{
  static const std::array<uint8_t, SHORT - 1> lut = {
      1,   138, 2,   137, 3,   136, 4,   135, 5,   134, 6,   133, 7,   132, 8,   131, 9,   130, 10,  129, 11,  128, 12,
      127, 13,  126, 14,  125, 15,  124, 16,  123, 17,  122, 18,  121, 19,  120, 20,  119, 21,  118, 22,  117, 23,  116,
      24,  115, 25,  114, 26,  113, 27,  112, 28,  111, 29,  110, 30,  109, 31,  108, 32,  107, 33,  106, 34,  105, 35,
      104, 36,  103, 37,  102, 38,  101, 39,  100, 40,  99,  41,  98,  42,  97,  43,  96,  44,  95,  45,  94,  46,  93,
      47,  92,  48,  91,  49,  90,  50,  89,  51,  88,  52,  87,  53,  86,  54,  85,  55,  84,  56,  83,  57,  82,  58,
      81,  59,  80,  60,  79,  61,  78,  62,  77,  63,  76,  64,  75,  65,  74,  66,  73,  67,  72,  68,  71,  69,  70};

  return static_cast<unsigned>(lut[root_sequence_index % lut.size()]);
}

span<const cf_t> prach_generator_impl::generate_y_u_v_long(unsigned sequence_number, unsigned cyclic_shift)
{
  // Sequence compression factor look-up table for each sequence number.
  static const std::array<uint16_t, LONG> compression_factor_table = {
      0,   1,   420, 280, 210, 168, 140, 120, 105, 373, 84,  534, 70,  710, 60,  56,  472, 691, 606, 265, 42,  40,  267,
      73,  35,  537, 355, 404, 30,  434, 28,  406, 236, 178, 765, 24,  303, 771, 552, 796, 21,  573, 20,  800, 553, 578,
      456, 482, 437, 137, 688, 510, 597, 95,  202, 778, 15,  368, 217, 128, 14,  784, 203, 293, 118, 142, 89,  551, 802,
      304, 12,  130, 571, 23,  805, 179, 276, 316, 398, 308, 430, 694, 706, 465, 10,  306, 400, 704, 696, 66,  289, 461,
      228, 415, 241, 53,  638, 173, 488, 339, 344, 108, 255, 391, 718, 8,   467, 494, 101, 585, 389, 257, 427, 297, 184,
      518, 528, 545, 64,  698, 7,   735, 392, 191, 521, 443, 566, 621, 59,  826, 71,  269, 464, 757, 695, 752, 401, 49,
      152, 670, 6,   720, 65,  751, 705, 758, 431, 605, 822, 473, 509, 789, 138, 170, 158, 249, 199, 668, 154, 591, 215,
      370, 347, 664, 353, 539, 652, 628, 5,   700, 153, 682, 200, 97,  352, 676, 348, 602, 33,  75,  564, 445, 650, 541,
      114, 322, 627, 673, 540, 657, 446, 123, 319, 313, 506, 327, 244, 477, 589, 156, 172, 743, 54,  62,  547, 618, 615,
      381, 359, 558, 4,   672, 653, 323, 247, 160, 470, 58,  712, 567, 614, 634, 548, 380, 633, 619, 568, 377, 92,  425,
      259, 385, 264, 821, 692, 432, 32,  662, 349, 502, 423, 94,  787, 511, 196, 363, 515, 214, 680, 155, 641, 478, 283,
      388, 730, 102, 449, 111, 413, 230, 455, 794, 554, 453, 232, 19,  798, 22,  767, 131, 376, 613, 620, 713, 444, 659,
      76,  524, 335, 418, 3,   630, 360, 252, 452, 577, 795, 801, 772, 90,  379, 617, 635, 63,  722, 529, 411, 113, 656,
      651, 674, 354, 814, 36,  69,  828, 85,  440, 79,  410, 544, 723, 519, 193, 334, 562, 77,  442, 715, 192, 527, 724,
      185, 213, 593, 364, 332, 195, 596, 788, 689, 474, 326, 645, 314, 278, 422, 600, 350, 99,  496, 342, 341, 499, 100,
      732, 468, 162, 176, 238, 338, 741, 174, 164, 301, 26,  436, 792, 457, 208, 282, 588, 642, 245, 325, 508, 690, 823,
      57,  623, 161, 493, 733, 9,   756, 707, 270, 227, 748, 290, 223, 207, 481, 793, 579, 231, 576, 555, 253, 110, 583,
      103, 122, 649, 658, 565, 714, 522, 78,  532, 86,  136, 791, 483, 27,  810, 31,  604, 693, 759, 309, 296, 727, 258,
      610, 93,  599, 503, 279, 837, 2,   560, 336, 240, 746, 229, 581, 112, 543, 530, 80,  146, 235, 808, 29,  812, 356,
      48,  703, 753, 307, 761, 317, 125, 274, 181, 190, 717, 736, 256, 729, 586, 284, 263, 608, 260, 46,  358, 632, 616,
      549, 91,  612, 569, 132, 83,  830, 106, 346, 678, 216, 782, 16,  149, 331, 514, 594, 197, 251, 557, 631, 382, 47,
      403, 813, 538, 675, 665, 98,  501, 601, 663, 677, 371, 107, 739, 340, 498, 497, 343, 740, 489, 239, 417, 561, 525,
      194, 513, 365, 150, 51,  243, 644, 507, 475, 246, 626, 654, 115, 312, 647, 124, 397, 762, 277, 505, 646, 320, 116,
      295, 429, 760, 399, 754, 11,  770, 803, 25,  485, 165, 188, 183, 726, 428, 310, 117, 776, 204, 222, 460, 749, 67,
      38,  44,  262, 387, 587, 479, 209, 836, 421, 504, 315, 763, 180, 395, 126, 219, 226, 463, 708, 72,  817, 41,  820,
      607, 386, 285, 45,  384, 609, 426, 728, 390, 737, 109, 451, 556, 361, 198, 684, 159, 625, 324, 476, 643, 328, 52,
      745, 416, 337, 490, 177, 807, 407, 147, 18,  575, 454, 580, 414, 747, 462, 271, 220, 206, 459, 291, 205, 225, 272,
      127, 781, 369, 679, 592, 516, 186, 167, 835, 281, 480, 458, 224, 221, 292, 777, 785, 96,  667, 683, 250, 362, 595,
      512, 333, 526, 520, 716, 393, 182, 299, 166, 212, 517, 725, 298, 189, 394, 275, 764, 806, 237, 491, 163, 487, 742,
      639, 157, 686, 139, 834, 211, 187, 300, 486, 175, 492, 469, 624, 248, 685, 171, 640, 590, 681, 669, 701, 50,  330,
      366, 17,  234, 408, 81,  134, 88,  774, 119, 833, 169, 687, 790, 438, 87,  144, 82,  375, 570, 768, 13,  780, 218,
      273, 396, 318, 648, 447, 104, 832, 141, 775, 294, 311, 321, 655, 542, 412, 582, 450, 254, 738, 345, 372, 831, 121,
      448, 584, 731, 495, 500, 351, 666, 201, 786, 598, 424, 611, 378, 550, 773, 143, 135, 439, 533, 829, 374, 133, 145,
      409, 531, 441, 523, 563, 660, 34,  816, 268, 709, 827, 535, 37,  288, 750, 697, 721, 546, 636, 55,  825, 711, 622,
      471, 824, 61,  637, 744, 242, 329, 151, 702, 402, 357, 383, 261, 286, 39,  819, 266, 818, 43,  287, 68,  536, 815,
      74,  661, 603, 433, 811, 405, 809, 435, 484, 302, 804, 766, 572, 799, 797, 574, 233, 148, 367, 783, 779, 129, 769,
      305, 755, 466, 734, 719, 699, 671, 629, 559, 419, 838};

  // Sequence phase offset look-up table for each sequence number.
  static const std::array<uint16_t, LONG> index_offset = {
      0,    2937, 1,    421,  841,  1261, 1681, 2101, 2521, 2941, 5,    2103, 845,  2943, 1685, 2105, 2525, 1267, 9,
      429,  849,  1269, 11,   2109, 2529, 2949, 1691, 433,  853,  2951, 1693, 435,  2533, 1275, 1695, 437,  857,  1277,
      1697, 439,  2537, 1279, 21,   441,  2539, 1281, 1701, 2121, 2541, 2961, 25,   2123, 2543, 1285, 1705, 447,  2545,
      2965, 1707, 449,  869,  1289, 31,   2129, 2549, 1291, 1711, 2131, 2551, 1293, 1713, 455,  2553, 2973, 37,   457,
      877,  2975, 39,   459,  2557, 2977, 1719, 2139, 881,  2979, 1721, 463,  883,  1303, 45,   2143, 885,  2983, 1725,
      2145, 2565, 1307, 49,   2147, 889,  1309, 51,   471,  891,  2989, 53,   2151, 893,  2991, 55,   2153, 2573, 1315,
      57,   477,  2575, 2995, 1737, 479,  2577, 2997, 61,   2159, 2579, 1321, 1741, 2161, 2581, 3001, 1743, 2163, 2583,
      1325, 67,   2165, 907,  3005, 69,   2167, 909,  1329, 71,   2169, 2589, 1331, 73,   493,  913,  3011, 1753, 495,
      2593, 1335, 1755, 2175, 2595, 3015, 79,   2177, 2597, 3017, 81,   2179, 2599, 3019, 83,   503,  2601, 3021, 1763,
      505,  925,  3023, 87,   2185, 927,  3025, 1767, 509,  929,  3027, 91,   2189, 2609, 3029, 1771, 513,  933,  1353,
      1773, 515,  2613, 3033, 1775, 2195, 937,  1357, 99,   519,  2617, 1359, 101,  2199, 2619, 3039, 103,  2201, 943,
      1363, 105,  525,  945,  3043, 107,  2205, 2625, 1367, 1787, 529,  2627, 1369, 1789, 2209, 2629, 3049, 1791, 533,
      953,  3051, 1793, 535,  955,  1375, 1795, 537,  957,  3055, 119,  539,  2637, 3057, 121,  541,  961,  1381, 123,
      543,  963,  1383, 125,  2223, 965,  3063, 1805, 547,  2645, 3065, 129,  549,  2647, 3067, 131,  551,  971,  3069,
      133,  2231, 2651, 3071, 1813, 2233, 975,  1395, 137,  2235, 977,  3075, 139,  559,  2657, 3077, 141,  561,  2659,
      1401, 1821, 563,  2661, 3081, 1823, 2243, 985,  1405, 1825, 2245, 2665, 1407, 1827, 2247, 989,  1409, 151,  2249,
      2669, 3089, 1831, 2251, 2671, 3091, 155,  2253, 995,  3093, 1835, 577,  2675, 1417, 1837, 2257, 2677, 1419, 161,
      2259, 1001, 3099, 163,  583,  1003, 3101, 1843, 2263, 2683, 1425, 167,  587,  2685, 3105, 169,  2267, 2687, 1429,
      1849, 2269, 2689, 3109, 1851, 593,  2691, 3111, 1853, 595,  1015, 1435, 177,  2275, 2695, 3115, 1857, 599,  2697,
      3117, 1859, 601,  2699, 1441, 1861, 603,  2701, 1443, 185,  605,  2703, 1445, 1865, 2285, 2705, 3125, 189,  2287,
      1029, 1449, 191,  611,  2709, 1451, 193,  613,  2711, 3131, 195,  615,  2713, 1455, 197,  2295, 2715, 3135, 199,
      2297, 2717, 1459, 1879, 621,  1041, 1461, 203,  623,  1043, 1463, 1883, 625,  2723, 1465, 1885, 627,  1047, 1467,
      1887, 2307, 1049, 1469, 1889, 2309, 2729, 1471, 1891, 633,  2731, 1473, 1893, 2313, 2733, 3153, 1895, 2315, 2735,
      1477, 1897, 639,  1059, 3157, 221,  641,  1061, 3159, 1901, 643,  2741, 3161, 225,  645,  2743, 3163, 1905, 647,
      2745, 3165, 1907, 2327, 1069, 3167, 231,  651,  1071, 1491, 1911, 653,  2751, 3171, 1913, 655,  2753, 1495, 1915,
      657,  2755, 1497, 239,  659,  2757, 1499, 241,  661,  1081, 3179, 1921, 2341, 2761, 1503, 245,  665,  2763, 1505,
      247,  667,  1087, 1507, 1927, 669,  1089, 3187, 251,  671,  2769, 3189, 1931, 673,  1093, 1513, 255,  2353, 2773,
      3193, 257,  2355, 1097, 3195, 1937, 679,  1099, 1519, 1939, 681,  2779, 1521, 263,  2361, 1103, 3201, 265,  685,
      1105, 1525, 267,  687,  1107, 3205, 1947, 2367, 1109, 1529, 1949, 691,  1111, 1531, 1951, 2371, 1113, 1533, 275,
      695,  2793, 1535, 1955, 697,  2795, 3215, 279,  699,  2797, 3217, 281,  2379, 1121, 3219, 1961, 2381, 1123, 1543,
      285,  705,  1125, 3223, 287,  2385, 2805, 3225, 289,  709,  2807, 3227, 291,  711,  2809, 1551, 293,  2391, 1133,
      3231, 1973, 2393, 2813, 3233, 1975, 2395, 2815, 3235, 299,  719,  2817, 3237, 301,  2399, 2819, 1561, 1981, 2401,
      2821, 1563, 305,  2403, 2823, 1565, 307,  727,  1147, 1567, 1987, 729,  2827, 1569, 1989, 731,  1151, 3249, 313,
      2411, 2831, 3251, 1993, 2413, 1155, 3253, 317,  737,  1157, 3255, 1997, 739,  2837, 3257, 1999, 2419, 1161, 1581,
      323,  743,  2841, 1583, 2003, 2423, 2843, 1585, 327,  747,  1167, 3265, 329,  2427, 2847, 1589, 331,  2429, 1171,
      3269, 333,  2431, 2851, 1593, 335,  755,  2853, 3273, 337,  757,  1177, 3275, 339,  759,  1179, 3277, 341,  761,
      1181, 1601, 2021, 763,  2861, 1603, 345,  2443, 2863, 3283, 2025, 767,  1187, 3285, 2027, 2447, 1189, 3287, 351,
      2449, 1191, 3289, 2031, 773,  1193, 1613, 355,  775,  1195, 1615, 2035, 777,  1197, 3295, 359,  779,  2877, 1619,
      361,  781,  2879, 3299, 2041, 783,  1203, 3301, 365,  2463, 1205, 3303, 367,  2465, 2885, 3305, 2047, 2467, 1209,
      3307, 2049, 791,  1211, 1631, 373,  2471, 1213, 3311, 2053, 2473, 2893, 1635, 377,  2475, 1217, 1637, 379,  799,
      2897, 3317, 381,  2479, 2899, 3319, 383,  803,  2901, 1643, 2063, 805,  1225, 1645, 2065, 807,  1227, 3325, 2067,
      2487, 2907, 1649, 391,  811,  2909, 1651, 2071, 813,  1233, 3331, 395,  815,  1235, 1655, 2075, 817,  2915, 3335,
      2077, 819,  2917, 1659, 2079, 2499, 2919, 1661, 2081, 823,  2921, 1663, 405,  2503, 2923, 1665, 407,  827,  1247,
      3345, 2087, 2507, 2927, 3347, 2089, 831,  1251, 1671, 413,  2511, 1253, 3351, 415,  835,  1255, 1675, 2095, 2515,
      2935, 3355, 419};

  // Create view of the sequence.
  span<cf_t> y_u_v = span<cf_t>(sequence).first(LONG);

  // Sequence compression factor and offset.
  uint64_t factor = static_cast<uint64_t>(compression_factor_table[sequence_number]);
  uint64_t offset = static_cast<uint64_t>(index_offset[sequence_number]);

  // Zadoff-Chu root.
  uint64_t root = static_cast<uint64_t>(sequence_number);

  // Generate sequence and calculate the scaling of the sequence transform.
  for (uint64_t n = 0; n != LONG; ++n) {
    // Calculate sequence table index considering the compression factor of the sequence sample index.
    uint64_t y_u_v_index = ((root * factor) * n * (factor * n + 1UL));

    // Apply cyclic shift.
    y_u_v_index += ((2UL * cyclic_shift) * n);

    // Get value from the complex exponential table.
    y_u_v[n] = cexp_table_long[(y_u_v_index * 2 + offset) % cexp_table_long.size()];
  }

  return y_u_v;
}

span<const cf_t> prach_generator_impl::generate_y_u_v_short(unsigned sequence_number, unsigned cyclic_shift)
{
  // Sequence compression factor look-up table for each sequence number.
  static const std::array<uint16_t, SHORT> compression_factor_table = {
      0,   1,   70,  93,  35,  28,  116, 20, 87,  31,  14,  38, 58,  107, 10,  102, 113, 90, 85, 22,  7,  53,  19,  133,
      29,  89,  123, 103, 5,   24,  51,  9,  126, 59,  45,  4,  112, 124, 11,  82,  73,  78, 96, 97,  79, 34,  136, 71,
      84,  122, 114, 30,  131, 21,  121, 91, 72,  100, 12,  33, 95,  98,  74,  64,  63,  77, 99, 83,  92, 137, 2,   47,
      56,  40,  62,  76,  75,  65,  41,  44, 106, 127, 39,  67, 48,  18,  118, 8,   109, 25, 17, 55,  68, 3,   105, 60,
      42,  43,  61,  66,  57,  128, 15,  27, 135, 94,  80,  13, 130, 88,  115, 134, 36,  16, 50, 110, 6,  120, 86,  132,
      117, 54,  49,  26,  37,  129, 32,  81, 101, 125, 108, 52, 119, 23,  111, 104, 46,  69, 138};

  // Sequence phase offset look-up table for each sequence number.
  static const std::array<uint16_t, SHORT> index_offset = {
      0,   487, 1,   349, 419, 211, 3,   351, 421, 491, 5,   75,  145, 215, 285, 77,  147, 217, 9,  357,
      427, 497, 289, 81,  151, 499, 13,  361, 431, 223, 15,  363, 433, 225, 295, 87,  435, 227, 19, 89,
      437, 507, 299, 369, 439, 231, 23,  371, 441, 511, 25,  95,  443, 513, 27,  375, 445, 515, 29, 377,
      169, 517, 309, 379, 171, 519, 311, 103, 173, 243, 313, 383, 453, 245, 37,  385, 177, 247, 39, 387,
      179, 527, 41,  111, 181, 529, 43,  113, 461, 531, 45,  115, 185, 533, 325, 117, 187, 257, 49, 119,
      467, 537, 329, 121, 469, 261, 331, 123, 193, 541, 333, 125, 195, 543, 57,  405, 475, 267, 59, 129,
      199, 547, 339, 409, 479, 271, 341, 411, 481, 551, 65,  135, 205, 553, 345, 137, 207, 555, 69};

  // Create view of the sequence.
  span<cf_t> y_u_v = span<cf_t>(sequence).first(SHORT);

  // Sequence compression factor and offset.
  uint64_t factor = static_cast<uint64_t>(compression_factor_table[sequence_number]);
  uint64_t offset = static_cast<uint64_t>(index_offset[sequence_number]);

  // Zadoff-Chu root.
  uint64_t root = static_cast<uint64_t>(sequence_number);

  // Generate sequence and calculate the scaling of the sequence transform.
  for (uint64_t n = 0; n != SHORT; ++n) {
    // Calculate sequence table index considering the compression factor of the sequence sample index.
    uint64_t y_u_v_index = ((root * factor) * n * (factor * n + 1UL));

    // Apply cyclic shift.
    y_u_v_index += ((2UL * cyclic_shift) * n);

    // Get value from the complex exponential table.
    y_u_v[n] = cexp_table_short[(y_u_v_index * 2 + offset) % cexp_table_short.size()];
  }

  return y_u_v;
}

span<const cf_t> prach_generator_impl::generate(const prach_generator::configuration& config)
{
  srsran_assert(config.restricted_set == restricted_set_config::UNRESTRICTED, "Unrestricted sets are not implemented.");

  unsigned                 sequence_length = SHORT;
  prach_subcarrier_spacing ra_scs          = prach_subcarrier_spacing::kHz15;
  if (is_long_preamble(config.format)) {
    prach_preamble_information info = get_prach_preamble_long_info(config.format);
    ra_scs                          = info.scs;
    sequence_length                 = info.sequence_length;
  }

  unsigned N_cs = prach_cyclic_shifts_get(ra_scs, config.restricted_set, config.zero_correlation_zone);
  srsran_assert(N_cs != PRACH_CYCLIC_SHIFTS_RESERVED, "Configuration leads to a reserved number of cyclic shifts.");

  unsigned root_sequence_index = config.root_sequence_index + config.preamble_index;
  unsigned cyclic_shift        = 0;

  if (N_cs != 0) {
    unsigned nof_sequences_per_root = sequence_length / N_cs;
    root_sequence_index             = config.root_sequence_index + config.preamble_index / nof_sequences_per_root;
    cyclic_shift                    = (config.preamble_index % nof_sequences_per_root) * N_cs;
  }

  if (is_long_preamble(config.format)) {
    return generate_y_u_v_long(get_sequence_number_long(root_sequence_index), cyclic_shift);
  }

  return generate_y_u_v_short(get_sequence_number_short(root_sequence_index), cyclic_shift);
}
