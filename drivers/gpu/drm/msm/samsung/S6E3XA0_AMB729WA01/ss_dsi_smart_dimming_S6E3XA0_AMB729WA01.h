/*
 * =================================================================
 *
 *       Filename:  smart_mtp_s6e3.h
 *
 *    Description:  Smart dimming algorithm implementation
 *
 *        Company:  Samsung Electronics
 *
 * ================================================================
 */
/*
<one line to give the program's name and a brief idea of what it does.>
Copyright (C) 2012, Samsung Electronics. All rights reserved.

 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef _SS_DSI_SMART_DIMMING_S6E3XA0_AMB729WA01_H_
#define _SS_DSI_SMART_DIMMING_S6E3XA0_AMB729WA01_H_

#include "ss_dsi_panel_common.h"
#include "ss_dsi_smart_dimming_common.h"

static int id1, id2, id3;

#define LUMINANCE_MAX 74
#define HMT_LUMINANCE_MAX 37
#define GAMMA_SET_MAX 34
#define GRAY_SCALE_MAX 256
#define RGB_COMPENSATION 27

/* BIT_SHIFT is used for right bit shfit */
#define BIT_SHIFT 22
#define BIT_SHFIT_MUL 4194304 // pow(2,BIT_SHIFT)

#define VREG0_REF_6P8 28521267	/* 6.8*4194304 */
#define VREG0_REF_6P5 27262976	/* 6.5*4194304 */
#define VREG0_REF_6P4 26843546	/* 6.4*4194304 */
#define VREG0_REF_6P3 26424115	/* 6.3*4194304 */
#define VREG0_REF_6P2 26004685	/* 6.2*4194304 */

#define VREG1_REF_6P6 27682406	/* 6.6*4194304 */
#define VREF_2P0 8388608 	/* 2*4194304 */
#define VREF_1P0 4194304 	/* 2*4194304 */

#define HBM_INTERPOLATION_STEP 40

enum {
	GAMMA_CURVE_1P9 = 0,
	GAMMA_CURVE_2P15,
	GAMMA_CURVE_2P2,
	GAMMA_CURVE_2P2_400CD,
};

/* TP */
enum {
	VT = 0,
	V0,
	V1,
	V7,
	V11,
	V23,
	V35,
	V51,
	V87,
	V151,
	V203,
	V255,
	V_MAX,
};

/* RGB */
enum {
	R = 0,
	G = 1,
	B = 2,
	RGB_MAX,
};

static char V_LIST[V_MAX][5] = {
	"VT",
	"V0",
	"V1",
	"V7",
	"V11",
	"V23",
	"V35",
	"V51",
	"V87",
	"V151",
	"V203",
	"V255",
};

struct illuminance_table {
	int lux;
	char gamma_setting[GAMMA_SET_MAX];
} __packed;

struct SMART_DIM {
	/* read C8h 1st ~ 35th */
	char MTP_ORIGN[GAMMA_SET_MAX];

	/* copy MTP_ORIGN -> MTP */
	int MTP[V_MAX][RGB_MAX];

	/* TP's gamma voltage */
	int RGB_OUTPUT[V_MAX][RGB_MAX];

	/* GRAY (0~255) */
	int GRAY[GRAY_SCALE_MAX][RGB_MAX];

	/* Because of AID funtion, below members are added*/
	int lux_table_max;
	int *plux_table;
	struct illuminance_table gen_table[LUMINANCE_MAX];

	int brightness_level;
	int ldi_revision;
	int vregout_voltage;
	int vref;
	char panel_revision;

	/* HBM interpolation */
	struct illuminance_table hbm_interpolation_table[HBM_INTERPOLATION_STEP+1];
	char *hbm_payload;
	int hbm_brightness_level;

	struct illuminance_table hmt_gen_table[HMT_LUMINANCE_MAX];
} __packed;

/* V0,V1,V7,V11,V23,V35,V51,V87,V151,V203,V255 */
static int INFLECTION_VOLTAGE_ARRAY[V_MAX] = {0, 0, 1, 7, 11, 23, 35, 51, 87, 151, 203, 255};

static int vt_coefficient[] = {
	0,  24,  48,  72,
	96,  120,  144,  168,
	192, 216, 276, 296,
	316, 336, 356, 372
};

static int v0_coefficient[] = {
	0, 12, 24, 36,
	48, 60, 72, 84,
	96, 108, 120, 132,
	144, 156, 168, 180
};

/* center (max) gamma value (Hex) */
static int center_gamma[V_MAX][RGB_MAX] = {
	{0x0, 0x0, 0x0},	/* VT */
	{0x4, 0x4, 0x4},	/* V0 */
	{0x80, 0x80, 0x80},	/* V1 */
	{0x80, 0x80, 0x80},	/* V7 */
	{0x80, 0x80, 0x80},	/* V11 */
	{0x80, 0x80, 0x80},	/* V23 */
	{0x80, 0x80, 0x80},	/* V35 */
	{0x80, 0x80, 0x80},	/* V51 */
	{0x80, 0x80, 0x80},	/* V87 */
	{0x80, 0x80, 0x80},	/* V151 */
	{0x80, 0x80, 0x80},	/* V203 */
	{0x200, 0x200, 0x200},	/* V255 */
};

/* fraction for gamma code */
static int fraction[V_MAX][2] = {
/* {numerator, denominator} */
	{0,  1200},		/* VT */
	{0,  1200},		/* V0 */
	{0,  256},		/* V1 */
	{0,  256},		/* V7 */
	{64, 320},		/* V11 */
	{64, 320},		/* V23 */
	{64, 320},		/* V35 */
	{64, 320},		/* V51 */
	{64, 320},		/* V87 */
	{64, 320},		/* V151 */
	{64, 320},		/* V203 */
	{177, 1200},		/* V255 */
};

static int hbm_interpolation_candela_table_revA[HBM_INTERPOLATION_STEP] = {425, 430, 433, 438, 443, 448, 452, 457, 460, 465,
										470, 475, 478, 483, 488, 493, 498, 501, 506, 510,
										515, 520, 523, 528, 533, 538, 542, 547, 550, 555,
										560, 565, 568, 573, 578, 583, 588, 591, 595, 600};

static unsigned int base_luminance_winner_revA[LUMINANCE_MAX][2] = {
	{2, 102},
	{3, 102},
	{4, 102},
	{5, 102},
	{6, 102},
	{7, 102},
	{8, 102},
	{9, 102},
	{10, 102},
	{11, 102},
	{12, 102},
	{13, 102},
	{14, 102},
	{15, 102},
	{16, 102},
	{17, 102},
	{19, 102},
	{20, 102},
	{21, 102},
	{22, 102},
	{24, 102},
	{25, 102},
	{27, 102},
	{29, 102},
	{30, 102},
	{32, 102},
	{34, 102},
	{37, 102},
	{39, 102},
	{41, 102},
	{44, 102},
	{47, 102},
	{50, 102},
	{53, 102},
	{56, 102},
	{60, 107},
	{64, 114},
	{68, 120},
	{72, 127},
	{77, 135},
	{82, 144},
	{87, 152},
	{93, 163},
	{98, 172},
	{105, 183},
	{111, 193},
	{119, 207},
	{126, 215},
	{134, 229},
	{143, 241},
	{152, 254},
	{162, 267},
	{172, 281},
	{183, 288},
	{195, 288},
	{207, 288},
	{220, 288},
	{234, 288},
	{249, 299},
	{265, 314},
	{282, 329},
	{300, 349},
	{316, 363},
	{333, 379},
	{350, 391},
	{357, 400},
	{365, 405},
	{372, 405},
	{380, 405},
	{387, 405},
	{395, 405},
	{403, 408},
	{412, 415},
	{420, 420}
};

static int gradation_offset_winner_revA[LUMINANCE_MAX][9] = {
/*	V255 V203 V151 V87 V51 V35 V23 V11 V7 */
	{0,	4,	11,	25,	35,	40,	42,	45,	45},
	{0,	3,	8,	19,	27,	32,	36,	39,	39},
	{0,	3,	4,	14,	22,	26,	30,	33,	33},
	{0,	3,	5,	11,	19,	24,	27,	30,	30},
	{0,	2,	5,	11,	18,	23,	25,	29,	29},
	{0,	1,	4,	9,	16,	21,	23,	27,	27},
	{0,	2,	4,	10,	15,	20,	23,	27,	27},
	{0,	2,	3,	9,	15,	19,	22,	26,	26},
	{0,	1,	3,	7,	13,	17,	20,	25,	25},
	{0,	1,	3,	8,	13,	16,	19,	25,	25},
	{0,	2,	3,	8,	12,	16,	19,	24,	24},
	{0,	1,	2,	6,	11,	14,	16,	22,	22},
	{0,	1,	2,	6,	11,	14,	17,	22,	22},
	{0,	1,	2,	5,	10,	13,	15,	20,	20},
	{0,	1,	2,	6,	10,	13,	15,	20,	20},
	{0,	1,	2,	4,	9,	12,	14,	19,	19},
	{0,	1,	2,	5,	8,	11,	13,	18,	18},
	{0,	0,	1,	4,	7,	10,	13,	18,	19},
	{0,	0,	1,	3,	7,	10,	12,	17,	17},
	{0,	0,	1,	3,	7,	9,	11,	17,	17},
	{0,	0,	0,	3,	6,	8,	10,	15,	15},
	{0,	1,	1,	3,	6,	8,	10,	15,	15},
	{0,	0,	0,	2,	5,	8,	9,	14,	14},
	{0,	0,	-1,	1,	5,	7,	9,	14,	14},
	{0,	0,	0,	1,	5,	7,	9,	14,	14},
	{0,	0,	0,	2,	4,	5,	7,	12,	12},
	{0,	0,	0,	1,	4,	6,	8,	13,	13},
	{0,	0,	0,	0,	3,	5,	6,	11,	11},
	{0,	0,	0,	0,	3,	4,	6,	11,	11},
	{0,	0,	0,	0,	2,	4,	6,	11,	11},
	{0,	0,	-1,	0,	2,	3,	4,	9,	9},
	{0,	0,	-1,	-1,	1,	2,	4,	8,	8},
	{0,	0,	-1,	-1,	1,	2,	4,	8,	8},
	{0,	-1,	-1,	-1,	1,	1,	2,	7,	7},
	{0,	0,	-1,	-1,	1,	1,	3,	7,	7},
	{0,	-1,	-1,	-1,	1,	0,	3,	7,	7},
	{0,	-1,	-1,	-1,	1,	0,	2,	5,	5},
	{0,	0,	-1,	0,	1,	0,	2,	6,	6},
	{0,	0,	-2,	0,	0,	0,	2,	6,	6},
	{0,	0,	-2,	-1,	0,	0,	3,	6,	6},
	{0,	0,	-1,	-1,	0,	-1,	2,	6,	6},
	{0,	0,	-1,	0,	1,	0,	3,	6,	6},
	{0,	-1,	-1,	0,	1,	0,	2,	6,	6},
	{0,	0,	0,	0,	1,	0,	2,	6,	6},
	{0,	-1,	0,	1,	1,	-1,	1,	6,	6},
	{0,	-1,	0,	1,	0,	-1,	2,	6,	6},
	{0,	-1,	0,	0,	1,	0,	1,	5,	5},
	{0,	-1,	0,	0,	1,	1,	2,	5,	5},
	{0,	-3,	0,	0,	0,	1,	3,	6,	6},
	{0,	-2,	0,	1,	0,	2,	2,	5,	5},
	{0,	-2,	0,	0,	0,	1,	2,	5,	5},
	{0,	-1,	1,	1,	0,	1,	2,	5,	6},
	{0,	-1,	0,	0,	0,	1,	2,	4,	6},
	{0,	0,	1,	1,	0,	1,	2,	4,	5},
	{0,	-1,	1,	0,	0,	0,	1,	3,	4},
	{0,	-1,	-1,	0,	-1,	0,	1,	2,	3},
	{0,	-1,	-1,	0,	-1,	-1,	0,	2,	3},
	{0,	-3,	-3,	-1,	-2,	-1,	0,	1,	2},
	{0,	-1,	0,	-1,	-1,	-1,	0,	1,	2},
	{0,	0,	-1,	-1,	-1,	-1,	0,	1,	1},
	{0,	0,	0,	-1,	-1,	-1,	-1,	0,	1},
	{0,	-1,	0,	-2,	-2,	-1,	0,	1,	2},
	{0,	-1,	0,	-1,	-2,	-1,	-1,	0,	1},
	{0,	0,	0,	-1,	-1,	-1,	0,	1,	1},
	{0,	-1,	-1,	-2,	-1,	-1,	-1,	1,	1},
	{0,	-1,	0,	-1,	-1,	-1,	-1,	0,	1},
	{0,	0,	-1,	-2,	-1,	-1,	-1,	0,	1},
	{0,	-1,	0,	-1,	-1,	-1,	-1,	-1,	0},
	{0,	-1,	-1,	-2,	-2,	-2,	-1,	-1,	0},
	{0,	-1,	-1,	-2,	-2,	-1,	-1,	-1,	1},
	{0,	-2,	-1,	-3,	-2,	-1,	-2,	-1,	1},
	{0,	-1,	-1,	-2,	-2,	-2,	-2,	-2,	0},
	{0,	-1,	-2,	-3,	-3,	-1,	-1,	-1,	0},
	{0,	0,	0,	0,	0,	0,	0,	0,	0}
};

static int rgb_offset_winner_revA[LUMINANCE_MAX][RGB_COMPENSATION] = {
/*	R255 G255 B255 R203 G203 B203 R151 G151 B151
	R87 G87 B87 R51 G51 B51 R35 G35 B35
	R23 G23 B23 R11 G11 B11
*/	
	{-13,	0,	-9,	-3,	0,	-4,	-7,	0,	-7,	-16,	-1,	-20,	-10,	1,	-19,	-5,	0,	-17,	-1,	2,	-9,	1,	0,	-15,	-6,	0,	-7},
	{-9,	0,	-6,	-2,	0,	-2,	-6,	0,	-6,	-14,	0,	-14,	-14,	0,	-22,	-9,	-1,	-19,	-6,	-1,	-14,	1,	-2,	-15,	-6,	0,	-7},
	{-6,	0,	-3,	-2,	0,	-2,	-6,	0,	-6,	-12,	1,	-12,	-15,	1,	-20,	-9,	2,	-19,	-10,	-1,	-18,	3,	0,	-14,	-6,	0,	-7},
	{-6,	0,	-3,	0,	0,	-1,	-4,	0,	-3,	-13,	0,	-13,	-19,	-2,	-22,	-11,	1,	-20,	-7,	-1,	-14,	1,	1,	-18,	-6,	0,	-7},
	{-4,	0,	-1,	-2,	0,	-3,	-3,	0,	-2,	-12,	-1,	-13,	-16,	-1,	-18,	-12,	-1,	-19,	-6,	0,	-22,	-3,	-2,	-11,	-6,	0,	-7},
	{-4,	0,	-2,	-2,	0,	-3,	-3,	0,	-2,	-9,	1,	-10,	-16,	0,	-18,	-11,	-1,	-20,	-6,	1,	-19,	-6,	1,	-14,	-5,	0,	-7},
	{-4,	0,	-2,	-1,	0,	-2,	-3,	0,	-3,	-13,	-1,	-12,	-13,	1,	-18,	-13,	-2,	-21,	-3,	-3,	-20,	-5,	0,	-14,	-5,	0,	-7},
	{-4,	0,	-2,	-1,	0,	-2,	-2,	1,	1,	-10,	-1,	-11,	-15,	-1,	-19,	-12,	0,	-17,	-6,	-3,	-28,	-3,	1,	-12,	-4,	0,	-9},
	{-3,	0,	-1,	-1,	0,	-2,	-2,	0,	-2,	-9,	1,	-8,	-11,	1,	-17,	-11,	0,	-21,	-6,	0,	-22,	-4,	0,	-15,	-5,	0,	-7},
	{-2,	0,	-1,	-2,	0,	-2,	-2,	0,	-2,	-9,	0,	-9,	-13,	0,	-17,	-8,	3,	-17,	-6,	0,	-23,	-9,	-7,	-15,	-4,	0,	-6},
	{-2,	0,	0,	-1,	0,	-2,	-3,	0,	-3,	-9,	0,	-8,	-10,	2,	-15,	-11,	-2,	-20,	-8,	0,	-25,	-3,	-1,	-14,	-4,	0,	-6},
	{-2,	0,	0,	-1,	0,	-2,	-1,	0,	-2,	-9,	0,	-9,	-10,	1,	-14,	-6,	3,	-16,	-2,	6,	-18,	-11,	-4,	-23,	-2,	0,	-6},
	{-2,	0,	0,	-1,	0,	-2,	-1,	0,	-2,	-9,	0,	-8,	-9,	0,	-14,	-11,	1,	-21,	-6,	1,	-22,	-4,	-1,	-17,	-4,	0,	-6},
	{-2,	0,	0,	-1,	0,	-2,	-1,	0,	-1,	-8,	0,	-8,	-9,	1,	-14,	-8,	3,	-17,	-4,	4,	-22,	-4,	0,	-17,	-4,	0,	-6},
	{-2,	0,	0,	-1,	0,	-2,	-1,	0,	-2,	-8,	0,	-7,	-9,	0,	-14,	-10,	1,	-20,	-3,	5,	-20,	-4,	0,	-19,	-4,	0,	-6},
	{-2,	0,	0,	-1,	0,	-2,	0,	0,	-1,	-7,	1,	-7,	-9,	1,	-12,	-9,	0,	-20,	-4,	3,	-21,	-5,	-2,	-20,	-3,	0,	-6},
	{-1,	0,	0,	-1,	0,	-1,	-1,	0,	-2,	-6,	0,	-6,	-9,	1,	-11,	-9,	0,	-19,	-8,	4,	-25,	-3,	1,	-18,	-3,	0,	-6},
	{-1,	0,	0,	0,	0,	-1,	-1,	0,	-1,	-7,	0,	-7,	-8,	2,	-10,	-9,	2,	-19,	-12,	-3,	-30,	-5,	0,	-19,	-2,	0,	-6},
	{-1,	0,	0,	0,	0,	-1,	-1,	0,	-1,	-7,	0,	-6,	-9,	0,	-11,	-10,	1,	-19,	-8,	1,	-25,	-7,	-1,	-23,	0,	0,	-6},
	{-2,	0,	0,	0,	0,	-1,	-1,	0,	-1,	-6,	0,	-6,	-9,	0,	-10,	-8,	3,	-18,	-9,	1,	-27,	-11,	-8,	-24,	-1,	0,	-6},
	{-1,	0,	0,	0,	0,	-1,	0,	0,	0,	-6,	0,	-6,	-8,	0,	-10,	-8,	3,	-15,	-13,	2,	-31,	0,	1,	-16,	-1,	0,	-6},
	{-1,	0,	0,	0,	0,	0,	-1,	0,	-1,	-5,	0,	-5,	-9,	-1,	-11,	-10,	2,	-16,	-11,	0,	-28,	-2,	2,	-20,	0,	0,	-6},
	{-1,	0,	0,	-1,	0,	-1,	1,	0,	0,	-4,	0,	-4,	-8,	0,	-10,	-14,	-3,	-20,	-8,	4,	-26,	-1,	5,	-19,	0,	0,	-6},
	{0,	0,	1,	-1,	0,	-1,	1,	1,	1,	-5,	0,	-5,	-10,	-2,	-11,	-12,	0,	-17,	-10,	1,	-25,	-5,	-1,	-23,	7,	0,	-10},
	{-1,	0,	1,	0,	0,	-1,	0,	-1,	-1,	-4,	0,	-4,	-9,	-2,	-10,	-13,	0,	-17,	-11,	0,	-25,	-4,	-2,	-25,	1,	0,	-6},
	{0,	0,	1,	0,	0,	-1,	0,	0,	0,	-3,	-2,	-3,	-4,	3,	-6,	-12,	3,	-16,	-7,	2,	-19,	-4,	-2,	-30,	1,	0,	-6},
	{0,	0,	1,	0,	0,	-1,	0,	0,	0,	-3,	0,	-3,	-9,	-3,	-10,	-10,	2,	-14,	-15,	-3,	-30,	-8,	-1,	-30,	1,	0,	-5},
	{-1,	0,	0,	0,	0,	0,	1,	0,	0,	-3,	0,	-3,	-7,	0,	-7,	-10,	-1,	-14,	-8,	3,	-21,	-8,	0,	-29,	2,	0,	-8},
	{-1,	0,	1,	0,	0,	-1,	1,	0,	0,	-3,	0,	-3,	-6,	0,	-6,	-12,	-1,	-15,	-11,	-1,	-23,	-8,	-1,	-30,	2,	0,	-5},
	{0,	0,	1,	0,	0,	-1,	0,	0,	0,	-2,	0,	-2,	-7,	0,	-7,	-12,	-3,	-16,	-12,	-2,	-23,	-12,	-2,	-34,	2,	0,	-3},
	{0,	0,	1,	0,	0,	-1,	0,	0,	1,	-2,	0,	-2,	-6,	0,	-5,	-9,	-1,	-13,	-5,	4,	-17,	-11,	-1,	-31,	3,	0,	-12},
	{-1,	0,	0,	1,	0,	0,	-1,	0,	0,	-2,	0,	-2,	-4,	0,	-5,	-7,	0,	-10,	-11,	-2,	-22,	-5,	5,	-27,	3,	0,	-10},
	{0,	0,	1,	-1,	0,	-1,	1,	0,	1,	-2,	0,	-2,	-5,	-1,	-5,	-9,	-2,	-12,	-12,	-3,	-23,	-7,	3,	-30,	4,	0,	-4},
	{-1,	0,	0,	0,	0,	0,	0,	0,	0,	-2,	0,	-2,	-4,	1,	-4,	-5,	1,	-8,	-5,	4,	-15,	-15,	-3,	-34,	0,	0,	-16},
	{0,	0,	1,	-1,	0,	-1,	1,	0,	1,	-1,	0,	-1,	-3,	0,	-3,	-7,	-1,	-9,	-11,	-3,	-18,	-9,	3,	-27,	4,	0,	-15},
	{0,	0,	2,	1,	0,	0,	0,	0,	0,	-1,	0,	-1,	-3,	0,	-4,	-4,	1,	-6,	-10,	-2,	-15,	-12,	0,	-31,	0,	0,	-13},
	{-1,	0,	2,	1,	0,	0,	-1,	0,	0,	-1,	0,	-1,	-3,	0,	-3,	-1,	3,	-3,	-3,	4,	-10,	-9,	2,	-28,	0,	0,	-13},
	{0,	0,	1,	-1,	0,	-1,	1,	0,	1,	-2,	0,	-1,	-3,	0,	-3,	-4,	1,	-7,	-6,	1,	-12,	-11,	1,	-29,	0,	0,	-13},
	{0,	0,	2,	0,	0,	-1,	0,	0,	0,	-1,	0,	-1,	-3,	0,	-2,	-3,	3,	-5,	-4,	1,	-10,	-14,	-1,	-31,	-1,	0,	-14},
	{0,	0,	2,	0,	0,	-1,	1,	0,	0,	-2,	0,	-1,	-2,	0,	-3,	-8,	-2,	-9,	-5,	0,	-10,	-9,	4,	-27,	-1,	0,	-14},
	{0,	0,	1,	-1,	0,	0,	0,	0,	0,	-1,	0,	-1,	-2,	0,	-2,	-4,	2,	-5,	-7,	-2,	-12,	-10,	3,	-28,	-2,	0,	-15},
	{0,	0,	2,	0,	0,	0,	0,	0,	0,	-1,	0,	0,	-3,	0,	-3,	-6,	0,	-7,	-4,	-1,	-8,	-11,	4,	-29,	-1,	0,	-14},
	{0,	0,	1,	0,	0,	-1,	0,	0,	0,	0,	0,	-1,	-2,	0,	-2,	-6,	0,	-6,	-6,	-1,	-9,	-14,	0,	-30,	-1,	0,	-13},
	{-1,	0,	1,	0,	0,	-1,	0,	0,	0,	-1,	0,	-1,	-2,	0,	-2,	-4,	0,	-5,	-7,	-1,	-9,	-13,	0,	-29,	-3,	-1,	-17},
	{0,	0,	2,	0,	0,	0,	1,	0,	1,	-2,	0,	-1,	-1,	0,	-1,	-4,	0,	-6,	-6,	0,	-7,	-16,	-2,	-33,	-2,	0,	-15},
	{0,	0,	1,	0,	0,	0,	1,	0,	0,	0,	0,	0,	-3,	0,	-3,	-4,	0,	-5,	-9,	-3,	-11,	-9,	3,	-25,	-2,	0,	-14},
	{0,	0,	2,	0,	0,	0,	0,	0,	0,	-1,	0,	0,	-2,	0,	-2,	-3,	0,	-4,	-3,	3,	-5,	-14,	0,	-29,	-2,	0,	-14},
	{0,	0,	2,	-1,	0,	-1,	1,	0,	0,	-1,	0,	-1,	-1,	0,	-1,	-2,	1,	-3,	-3,	1,	-4,	-15,	0,	-26,	-1,	1,	-17},
	{0,	0,	3,	0,	0,	-1,	-1,	0,	0,	1,	0,	1,	-2,	0,	-2,	-4,	0,	-4,	-5,	-2,	-8,	-15,	-1,	-24,	-2,	0,	-17},
	{1,	0,	3,	0,	0,	-1,	0,	0,	0,	-1,	0,	-1,	-1,	0,	0,	-3,	0,	-3,	-4,	-1,	-6,	-8,	4,	-19,	-4,	-1,	-17},
	{1,	0,	3,	-1,	0,	-1,	1,	0,	1,	0,	0,	-1,	-1,	0,	0,	-2,	0,	-2,	-3,	0,	-4,	-12,	0,	-22,	-7,	0,	-15},
	{1,	0,	3,	-2,	0,	-2,	1,	0,	1,	0,	0,	0,	-1,	0,	-1,	-3,	0,	-2,	-7,	-3,	-7,	-15,	-3,	-23,	-2,	3,	-19},
	{0,	0,	2,	-1,	0,	-1,	0,	0,	1,	1,	0,	0,	-2,	0,	-1,	-2,	0,	-2,	-5,	-2,	-5,	-9,	2,	-16,	-8,	-1,	-25},
	{1,	0,	2,	-1,	0,	-1,	0,	0,	0,	1,	0,	0,	-3,	-1,	-2,	-2,	0,	-2,	-3,	1,	-4,	-11,	0,	-18,	-2,	4,	-19},
	{1,	0,	2,	-2,	0,	-1,	0,	0,	1,	0,	0,	0,	0,	0,	0,	-2,	0,	-2,	0,	3,	0,	-12,	-1,	-17,	-4,	2,	-19},
	{0,	0,	2,	-1,	0,	-1,	0,	0,	0,	0,	0,	1,	-1,	0,	-1,	-1,	0,	-1,	-2,	0,	-2,	-5,	4,	-10,	-2,	1,	-16},
	{1,	0,	2,	-2,	0,	-1,	0,	0,	0,	1,	0,	1,	0,	0,	0,	-1,	0,	-1,	-1,	0,	-1,	-10,	-1,	-15,	-4,	0,	-17},
	{0,	0,	1,	0,	0,	0,	1,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	-1,	-2,	-1,	-8,	1,	-12,	-4,	0,	-15},
	{-1,	0,	0,	-1,	0,	0,	0,	0,	0,	1,	0,	0,	1,	0,	1,	-1,	0,	-1,	-1,	-2,	-1,	-6,	3,	-9,	-4,	0,	-12},
	{0,	0,	0,	0,	0,	0,	0,	0,	1,	0,	0,	-1,	0,	0,	1,	-1,	0,	0,	0,	0,	0,	-7,	1,	-9,	-4,	0,	-12},
	{0,	0,	0,	0,	0,	0,	0,	0,	0,	1,	0,	1,	-1,	0,	0,	0,	0,	1,	1,	0,	1,	-7,	0,	-9,	-4,	0,	-10},
	{-2,	0,	-1,	1,	0,	0,	0,	0,	0,	1,	0,	1,	0,	0,	0,	-1,	0,	0,	-1,	-1,	-1,	-11,	-4,	-12,	-4,	0,	-10},
	{-1,	0,	1,	0,	0,	-1,	0,	0,	1,	0,	0,	-1,	-1,	0,	0,	1,	1,	2,	0,	0,	0,	-3,	2,	-4,	-7,	-2,	-12},
	{1,	0,	0,	-1,	0,	-1,	0,	0,	1,	0,	0,	0,	0,	0,	0,	0,	0,	1,	-2,	-2,	-3,	-6,	-1,	-6,	-5,	1,	-9},
	{0,	0,	0,	0,	0,	0,	0,	0,	0,	1,	0,	1,	-1,	0,	0,	0,	0,	0,	-3,	-3,	-3,	-5,	-2,	-5,	-7,	0,	-10},
	{0,	0,	0,	0,	0,	0,	-1,	0,	0,	1,	0,	0,	-1,	0,	0,	1,	0,	1,	0,	0,	0,	-7,	-3,	-6,	-6,	0,	-10},
	{0,	0,	0,	-1,	0,	0,	1,	0,	1,	0,	0,	0,	0,	0,	0,	0,	0,	1,	0,	0,	0,	-6,	-3,	-6,	-6,	0,	-9},
	{-1,	0,	-1,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	1,	0,	0,	0,	0,	0,	0,	1,	2,	0,	-2,	0,	-3},
	{-1,	0,	-1,	0,	0,	0,	0,	0,	0,	1,	0,	1,	-1,	0,	0,	1,	0,	1,	-1,	-1,	-1,	-2,	0,	-2,	-1,	0,	-2},
	{-1,	0,	-1,	0,	0,	0,	0,	0,	0,	1,	1,	1,	0,	0,	0,	-1,	-1,	0,	0,	-1,	0,	1,	2,	0,	-5,	-2,	-6},
	{-2,	0,	-2,	1,	0,	0,	-1,	0,	0,	2,	0,	1,	-1,	0,	0,	0,	0,	1,	2,	3,	3,	-5,	-4,	-5,	-3,	-1,	-4},
	{-1,	0,	-1,	1,	0,	0,	-1,	0,	0,	1,	0,	1,	-1,	0,	0,	2,	2,	2,	3,	1,	2,	2,	1,	3,	-2,	0,	-1},
	{0,	0,	-1,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	1,	0,	0,	1,	-1,	-1,	-1,	-1,	-1,	-1,	-6,	-3,	-6},
	{0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0}
};

/************
 *	  HMT   *
 ************/
static unsigned int base_luminance_reverse_hmt_single[][2] = {
	{10,	42},
	{11,	47},
	{12,	51},
	{13,	56},
	{14,	60},
	{15,	64},
	{16,	68},
	{17,	74},
	{19,	82},
	{20,	86},
	{21,	89},
	{22,	94},
	{23,	97},
	{25,	106},
	{27,	114},
	{29,	121},
	{31,	130},
	{33,	137},
	{35,	145},
	{37,	154},
	{39,	162},
	{41,	168},
	{44,	180},
	{47,	191},
	{50,	201},
	{53,	213},
	{56,	224},
	{60,	239},
	{64,	251},
	{68,	265},
	{72,	277},
	{77,	212},
	{82,	223},
	{87,	236},
	{93,	249},
	{99,	263},
	{105,	279},
};

static int gradation_offset_reverse_hmt_single[][9] = {
/*	V255 V203 V151 V87 V51 V35 V23 V11 V7 */
	{0,	2,	2,	3,	5,	6,	6,	9,	8},
	{0,	1,	2,	2,	5,	6,	7,	9,	8},
	{0,	1,	2,	2,	4,	6,	6,	8,	8},
	{0,	1,	1,	3,	4,	5,	6,	9,	9},
	{0,	0,	2,	2,	5,	6,	6,	8,	9},
	{0,	0,	2,	2,	4,	5,	6,	9,	9},
	{0,	0,	2,	2,	4,	5,	6,	9,	9},
	{0,	0,	2,	2,	3,	5,	6,	9,	9},
	{0,	1,	2,	2,	4,	5,	5,	8,	8},
	{0,	2,	2,	2,	4,	5,	6,	8,	8},
	{0,	2,	2,	2,	3,	5,	6,	8,	8},
	{0,	2,	1,	2,	4,	4,	5,	8,	8},
	{0,	2,	2,	2,	4,	5,	5,	8,	8},
	{0,	1,	0,	1,	3,	5,	6,	8,	8},
	{0,	3,	1,	2,	3,	4,	5,	8,	9},
	{0,	3,	2,	2,	3,	5,	5,	8,	9},
	{0,	4,	2,	3,	3,	5,	5,	7,	8},
	{0,	4,	3,	3,	4,	5,	5,	8,	8},
	{0,	3,	4,	3,	4,	5,	5,	8,	8},
	{0,	3,	3,	2,	3,	4,	4,	7,	8},
	{0,	3,	4,	3,	3,	5,	5,	7,	8},
	{0,	3,	4,	3,	3,	5,	5,	8,	9},
	{0,	2,	4,	3,	3,	5,	5,	8,	8},
	{0,	2,	5,	3,	3,	5,	5,	8,	9},
	{0,	3,	5,	4,	3,	4,	4,	7,	8},
	{0,	3,	5,	4,	3,	5,	5,	7,	8},
	{0,	4,	7,	5,	4,	5,	4,	7,	8},
	{0,	5,	6,	4,	4,	4,	5,	8,	8},
	{0,	4,	6,	5,	3,	4,	4,	7,	8},
	{0,	5,	7,	5,	4,	5,	5,	7,	8},
	{0,	6,	9,	6,	4,	4,	4,	7,	9},
	{0,	2,	4,	2,	1,	3,	3,	4,	4},
	{0,	3,	4,	3,	1,	2,	2,	4,	4},
	{0,	3,	5,	3,	1,	2,	2,	4,	4},
	{0,	4,	5,	3,	1,	1,	1,	3,	4},
	{0,	5,	6,	3,	2,	2,	2,	4,	5},
	{0,	4,	5,	3,	1,	1,	1,	4,	5},
};

static int rgb_offset_reverse_hmt_single[][RGB_COMPENSATION] = {
/*	R255 G255 B255 R203 G203 B203 R151 G151 B151
	R87 G87 B87 R51 G51 B51 R35 G35 B35
	R23 G23 B23 R11 G11 B11
*/
	{0,	0,	1,	0,	0,	0,	-1,	0,	-1,	-2,	2,	-4,	-4,	3,	-7,	-1,	3,	-6,	-4,	4,	-8,	0,	3,	-8,	-7,	1,	-3},
	{0,	0,	2,	-1,	0,	0,	-1,	0,	-1,	-2,	2,	-4,	-4,	2,	-6,	-2,	2,	-6,	-3,	3,	-6,	0,	4,	-10,	-12,	0,	-2},
	{0,	0,	2,	0,	0,	0,	0,	0,	-1,	-2,	2,	-4,	-3,	3,	-6,	-3,	2,	-6,	-3,	2,	-6,	0,	4,	-9,	0,	0,	-2},
	{0,	0,	1,	0,	0,	0,	-1,	0,	0,	-3,	1,	-4,	-4,	3,	-7,	-3,	3,	-7,	-3,	2,	-6,	1,	3,	-7,	-7,	1,	-4},
	{0,	0,	2,	0,	0,	0,	-1,	0,	-1,	-2,	1,	-3,	-3,	2,	-6,	-4,	3,	-7,	-2,	3,	-8,	1,	4,	-9,	-3,	0,	0},
	{0,	0,	1,	0,	0,	0,	0,	0,	0,	-2,	1,	-4,	-4,	2,	-6,	-2,	3,	-6,	-2,	3,	-7,	0,	3,	-8,	-3,	1,	-4},
	{1,	0,	2,	0,	0,	0,	0,	0,	0,	-3,	1,	-4,	-4,	2,	-6,	-2,	3,	-6,	-2,	3,	-6,	0,	4,	-9,	-3,	1,	-3},
	{0,	0,	1,	-1,	0,	0,	0,	0,	0,	-1,	1,	-2,	-4,	2,	-6,	-2,	3,	-6,	-2,	3,	-7,	0,	3,	-6,	-1,	2,	-4},
	{0,	0,	1,	-1,	0,	0,	0,	0,	0,	-2,	1,	-3,	-3,	2,	-5,	-4,	2,	-6,	-4,	3,	-8,	0,	3,	-8,	-3,	2,	-5},
	{1,	0,	1,	0,	0,	0,	1,	0,	0,	-2,	1,	-3,	-4,	2,	-5,	-3,	3,	-6,	-1,	3,	-6,	-2,	3,	-8,	-2,	2,	-4},
	{0,	0,	1,	0,	0,	0,	0,	0,	0,	-2,	1,	-2,	-2,	2,	-4,	-3,	3,	-6,	-2,	3,	-6,	-3,	4,	-9,	-1,	2,	-4},
	{1,	0,	2,	0,	0,	0,	0,	0,	0,	-2,	1,	-2,	-3,	2,	-4,	-4,	3,	-7,	-1,	3,	-6,	-2,	4,	-8,	-2,	2,	-4},
	{1,	0,	1,	0,	0,	0,	0,	0,	0,	-2,	1,	-3,	-3,	1,	-4,	-3,	3,	-6,	-4,	3,	-8,	-2,	4,	-10,	-5,	2,	-5},
	{1,	0,	1,	0,	0,	0,	0,	0,	-1,	-1,	1,	-2,	-3,	1,	-4,	-4,	2,	-6,	-3,	3,	-7,	-1,	4,	-10,	0,	2,	-4},
	{0,	0,	2,	0,	0,	0,	0,	0,	0,	-2,	0,	-2,	-3,	1,	-4,	-4,	2,	-6,	-2,	2,	-6,	-4,	4,	-10,	0,	1,	-2},
	{1,	0,	1,	0,	0,	0,	0,	0,	0,	-2,	0,	-2,	-3,	1,	-4,	-4,	3,	-6,	-3,	3,	-6,	-1,	4,	-8,	-1,	1,	-4},
	{0,	0,	1,	0,	0,	0,	0,	0,	0,	-2,	1,	-2,	-2,	1,	-3,	-2,	2,	-4,	-3,	3,	-6,	-5,	5,	-12,	0,	0,	-1},
	{0,	0,	1,	0,	0,	0,	0,	0,	0,	-2,	0,	-2,	-3,	1,	-4,	-3,	2,	-4,	-3,	3,	-6,	-4,	5,	-10,	-2,	1,	-4},
	{1,	0,	2,	1,	0,	0,	0,	0,	0,	-2,	0,	-2,	-3,	1,	-4,	-3,	2,	-5,	-4,	3,	-8,	-6,	5,	-10,	-1,	2,	-4},
	{1,	0,	2,	0,	0,	0,	0,	0,	0,	-1,	0,	-1,	-1,	1,	-3,	-4,	2,	-5,	-4,	3,	-7,	-5,	5,	-10,	-2,	2,	-5},
	{0,	0,	1,	0,	0,	1,	0,	0,	0,	0,	0,	-2,	-4,	1,	-4,	-2,	2,	-4,	-3,	3,	-6,	-4,	5,	-10,	-2,	2,	-4},
	{1,	0,	2,	0,	0,	0,	0,	0,	0,	-2,	0,	-2,	-3,	1,	-3,	-3,	2,	-4,	-5,	3,	-8,	-5,	4,	-9,	-1,	1,	-3},
	{2,	0,	2,	0,	0,	0,	0,	0,	0,	-2,	0,	-2,	-2,	1,	-3,	-3,	1,	-4,	-4,	3,	-6,	-5,	4,	-9,	-1,	2,	-5},
	{2,	0,	3,	0,	0,	0,	-1,	0,	-1,	-1,	0,	-1,	-2,	1,	-3,	-2,	1,	-3,	-5,	3,	-7,	-4,	4,	-10,	-2,	1,	-4},
	{3,	0,	4,	0,	0,	0,	0,	0,	0,	-1,	0,	-1,	-2,	1,	-3,	-3,	2,	-4,	-4,	3,	-7,	-4,	4,	-9,	-2,	2,	-4},
	{3,	0,	3,	0,	0,	0,	-1,	0,	0,	0,	0,	0,	-2,	1,	-4,	-3,	1,	-4,	-4,	2,	-6,	-5,	5,	-10,	0,	1,	-3},
	{3,	0,	3,	0,	0,	0,	-2,	0,	-1,	-1,	0,	0,	-3,	1,	-3,	-1,	2,	-4,	-4,	3,	-6,	-4,	4,	-9,	-1,	1,	-4},
	{3,	0,	3,	0,	0,	0,	-1,	0,	0,	-1,	0,	-1,	-2,	1,	-3,	-3,	1,	-4,	-5,	2,	-6,	-4,	4,	-10,	-2,	2,	-5},
	{4,	0,	4,	0,	0,	0,	-1,	0,	-1,	-1,	0,	-1,	-2,	1,	-3,	-2,	1,	-4,	-4,	3,	-6,	-5,	4,	-10,	0,	2,	-5},
	{2,	0,	4,	0,	0,	0,	0,	0,	0,	0,	0,	-1,	-2,	1,	-3,	-3,	1,	-4,	-4,	2,	-6,	-5,	5,	-10,	-1,	2,	-5},
	{3,	0,	4,	0,	0,	0,	0,	0,	0,	-2,	0,	-2,	-2,	1,	-2,	-3,	1,	-4,	-4,	2,	-6,	-5,	4,	-10,	-1,	1,	-4},
	{3,	0,	3,	0,	0,	0,	0,	0,	0,	-1,	0,	-1,	-1,	0,	-2,	-2,	0,	-2,	-4,	2,	-5,	-4,	4,	-8,	0,	2,	-5},
	{3,	0,	3,	0,	0,	0,	-1,	0,	0,	0,	0,	0,	-1,	0,	-1,	-2,	0,	-2,	-2,	2,	-4,	-4,	4,	-8,	0,	2,	-4},
	{3,	0,	3,	0,	0,	0,	0,	0,	0,	-1,	0,	-1,	-2,	0,	-2,	-1,	0,	-2,	-2,	2,	-4,	-4,	4,	-8,	-1,	2,	-5},
	{4,	0,	4,	0,	0,	0,	0,	0,	0,	-1,	0,	-1,	-2,	1,	-2,	-1,	1,	-2,	-3,	2,	-4,	-3,	4,	-8,	-1,	2,	-4},
	{2,	0,	4,	0,	0,	0,	0,	0,	0,	-1,	0,	0,	-1,	0,	-2,	-1,	0,	-2,	-2,	1,	-4,	-5,	3,	-8,	-3,	2,	-6},
	{3,	0,	4,	0,	0,	0,	0,	0,	0,	0,	0,	0,	-1,	1,	-2,	-1,	0,	-2,	-2,	1,	-3,	-4,	3,	-8,	-2,	2,	-5},
};
#endif
