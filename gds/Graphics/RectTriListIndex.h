#ifndef  __RECTTRILISTINDEX_H__
#define  __RECTTRILISTINDEX_H__


#include <d3d9.h>


#define NUM_MAX_2DRECTS 128

static const WORD s_RectTriListIndex[NUM_MAX_2DRECTS * 6] =
{
	  0,   1,   2,    0,   2,   3,
	  4,   5,   6,    4,   6,   7,
	  8,   9,  10,    8,  10,  11,
	 12,  13,  14,   12,  14,  15,
	 16,  17,  18,   16,  18,  19,
	 20,  21,  22,   20,  22,  23,
	 24,  25,  26,   24,  26,  27,
	 28,  29,  30,   28,  30,  31,
	 32,  33,  34,   32,  34,  35,
	 36,  37,  38,   36,  38,  39,
	 40,  41,  42,   40,  42,  43,
	 44,  45,  46,   44,  46,  47,
	 48,  49,  50,   48,  50,  51,
	 52,  53,  54,   52,  54,  55,
	 56,  57,  58,   56,  58,  59,
	 60,  61,  62,   60,  62,  63,
	 64,  65,  66,   64,  66,  67,
	 68,  69,  70,   68,  70,  71,
	 72,  73,  74,   72,  74,  75,
	 76,  77,  78,   76,  78,  79,
	 80,  81,  82,   80,  82,  83,
	 84,  85,  86,   84,  86,  87,
	 88,  89,  90,   88,  90,  91,
	 92,  93,  94,   92,  94,  95,
	 96,  97,  98,   96,  98,  99,
	100, 101, 102,  100, 102, 103,
	104, 105, 106,  104, 106, 107,
	108, 109, 110,  108, 110, 111,
	112, 113, 114,  112, 114, 115,
	116, 117, 118,  116, 118, 119,
	120, 121, 122,  120, 122, 123,
	124, 125, 126,  124, 126, 127,
	128, 129, 130,  128, 130, 131,
	132, 133, 134,  132, 134, 135,
	136, 137, 138,  136, 138, 139,
	140, 141, 142,  140, 142, 143,
	144, 145, 146,  144, 146, 147,
	148, 149, 150,  148, 150, 151,
	152, 153, 154,  152, 154, 155,
	156, 157, 158,  156, 158, 159,
	160, 161, 162,  160, 162, 163,
	164, 165, 166,  164, 166, 167,
	168, 169, 170,  168, 170, 171,
	172, 173, 174,  172, 174, 175,
	176, 177, 178,  176, 178, 179,
	180, 181, 182,  180, 182, 183,
	184, 185, 186,  184, 186, 187,
	188, 189, 190,  188, 190, 191,
	192, 193, 194,  192, 194, 195,
	196, 197, 198,  196, 198, 199,
	200, 201, 202,  200, 202, 203,
	204, 205, 206,  204, 206, 207,
	208, 209, 210,  208, 210, 211,
	212, 213, 214,  212, 214, 215,
	216, 217, 218,  216, 218, 219,
	220, 221, 222,  220, 222, 223,
	224, 225, 226,  224, 226, 227,
	228, 229, 230,  228, 230, 231,
	232, 233, 234,  232, 234, 235,
	236, 237, 238,  236, 238, 239,
	240, 241, 242,  240, 242, 243,
	244, 245, 246,  244, 246, 247,
	248, 249, 250,  248, 250, 251,
	252, 253, 254,  252, 254, 255,
	256, 257, 258,  256, 258, 259,
	260, 261, 262,  260, 262, 263,
	264, 265, 266,  264, 266, 267,
	268, 269, 270,  268, 270, 271,
	272, 273, 274,  272, 274, 275,
	276, 277, 278,  276, 278, 279,
	280, 281, 282,  280, 282, 283,
	284, 285, 286,  284, 286, 287,
	288, 289, 290,  288, 290, 291,
	292, 293, 294,  292, 294, 295,
	296, 297, 298,  296, 298, 299,
	300, 301, 302,  300, 302, 303,
	304, 305, 306,  304, 306, 307,
	308, 309, 310,  308, 310, 311,
	312, 313, 314,  312, 314, 315,
	316, 317, 318,  316, 318, 319,
	320, 321, 322,  320, 322, 323,
	324, 325, 326,  324, 326, 327,
	328, 329, 330,  328, 330, 331,
	332, 333, 334,  332, 334, 335,
	336, 337, 338,  336, 338, 339,
	340, 341, 342,  340, 342, 343,
	344, 345, 346,  344, 346, 347,
	348, 349, 350,  348, 350, 351,
	352, 353, 354,  352, 354, 355,
	356, 357, 358,  356, 358, 359,
	360, 361, 362,  360, 362, 363,
	364, 365, 366,  364, 366, 367,
	368, 369, 370,  368, 370, 371,
	372, 373, 374,  372, 374, 375,
	376, 377, 378,  376, 378, 379,
	380, 381, 382,  380, 382, 383,
	384, 385, 386,  384, 386, 387,
	388, 389, 390,  388, 390, 391,
	392, 393, 394,  392, 394, 395,
	396, 397, 398,  396, 398, 399,
	400, 401, 402,  400, 402, 403,
	404, 405, 406,  404, 406, 407,
	408, 409, 410,  408, 410, 411,
	412, 413, 414,  412, 414, 415,
	416, 417, 418,  416, 418, 419,
	420, 421, 422,  420, 422, 423,
	424, 425, 426,  424, 426, 427,
	428, 429, 430,  428, 430, 431,
	432, 433, 434,  432, 434, 435,
	436, 437, 438,  436, 438, 439,
	440, 441, 442,  440, 442, 443,
	444, 445, 446,  444, 446, 447,
	448, 449, 450,  448, 450, 451,
	452, 453, 454,  452, 454, 455,
	456, 457, 458,  456, 458, 459,
	460, 461, 462,  460, 462, 463,
	464, 465, 466,  464, 466, 467,
	468, 469, 470,  468, 470, 471,
	472, 473, 474,  472, 474, 475,
	476, 477, 478,  476, 478, 479,
	480, 481, 482,  480, 482, 483,
	484, 485, 486,  484, 486, 487,
	488, 489, 490,  488, 490, 491,
	492, 493, 494,  492, 494, 495,
	496, 497, 498,  496, 498, 499,
	500, 501, 502,  500, 502, 503,
	504, 505, 506,  504, 506, 507,
	508, 509, 510,  508, 510, 511
};



#endif  /*  __RECTTRILISTINDEX_H__  */