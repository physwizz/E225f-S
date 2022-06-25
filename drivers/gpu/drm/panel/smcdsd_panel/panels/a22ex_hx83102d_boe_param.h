/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __HX83102D_BOE_PARAM_H__
#define __HX83102D_BOE_PARAM_H__

#include <linux/types.h>
#include <linux/kernel.h>

#define LCD_TYPE_VENDOR		"BOE"

#define EXTEND_BRIGHTNESS	306
#define UI_MAX_BRIGHTNESS	255
#define UI_DEFAULT_BRIGHTNESS	128

#define LDI_REG_BRIGHTNESS	0x51
#define LDI_REG_ID		0x04
#define LDI_LEN_BRIGHTNESS	((u16)ARRAY_SIZE(SEQ_HX83102D_BOE_BRIGHTNESS))

/* len is read length */
#define LDI_LEN_ID		3

#define TYPE_WRITE		I2C_SMBUS_WRITE
#define TYPE_DELAY		U8_MAX

static u8 LM36274_INIT[] = {
	TYPE_WRITE, 0x0C, 0x2C,
	TYPE_WRITE, 0x0D, 0x26,
	TYPE_WRITE, 0x0E, 0x26,
	TYPE_WRITE, 0x09, 0xBE,
	TYPE_WRITE, 0x02, 0x69,
	TYPE_WRITE, 0x03, 0x0F,
	TYPE_WRITE, 0x11, 0x75,
	TYPE_WRITE, 0x04, 0x07,
	TYPE_WRITE, 0x05, 0xFF,
	TYPE_WRITE, 0x10, 0x67,
	TYPE_WRITE, 0x08, 0x13,
};

static u8 LM36274_EXIT[] = {
	TYPE_WRITE, 0x09, 0xBC,
	TYPE_DELAY, 3, 0,
	TYPE_WRITE, 0x09, 0xB8,
};

struct lcd_seq_info {
	unsigned char	*cmd;
	unsigned int	len;
	unsigned int	sleep;
};

static unsigned char SEQ_HX83102D_BOE_SLEEP_OUT[] = { 0x11 };
static unsigned char SEQ_HX83102D_BOE_SLEEP_IN[] = { 0x10 };
static unsigned char SEQ_HX83102D_BOE_DISPLAY_OFF[] = { 0x28 };
static unsigned char SEQ_HX83102D_BOE_DISPLAY_ON[] = { 0x29 };

static unsigned char SEQ_HX83102D_BOE_BRIGHTNESS[] = {
	0x51,
	0x00, 0x00,
};

static unsigned char SEQ_HX83102D_BOE_CU_01[] = {
	0xB9,
	0x83, 0x10, 0x2D,
};

static unsigned char SEQ_HX83102D_BOE_CU_02[] = {
	0xB1,
	0x22, 0x44, 0x31, 0x31, 0x22, 0x34, 0x2F, 0x57, 0x08, 0x08,
	0x08, 0x67, 0x67, 0x71
};

static unsigned char SEQ_HX83102D_BOE_CU_03[] = {
	0xB2,
	0x00, 0x00, 0x06, 0x18, 0x00, 0x0E, 0xFE, 0x40, 0x00, 0x00,
	0x00, 0x00, 0xF4, 0xA0,
};

static unsigned char SEQ_HX83102D_BOE_CU_04[] = {
	0xB4,
	0x19, 0x59, 0x19, 0x59, 0x19, 0x59, 0x19, 0x59, 0x06, 0xFF,
	0x06, 0x20, 0x00, 0xFF,
};

static unsigned char SEQ_HX83102D_BOE_CU_05[] = {
	0xCC,
	0x02,
};

static unsigned char SEQ_HX83102D_BOE_CU_06[] = {
	0xD3,
	0x00, 0x00, 0x3C, 0x03, 0x00, 0x08, 0x00, 0x37, 0x00, 0x33,
	0x33, 0x08, 0x08, 0x00, 0x00, 0x32, 0x10, 0x06, 0x00, 0x06,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x12, 0x00, 0x00,
};

static unsigned char SEQ_HX83102D_BOE_CU_07[] = {
	0xD5,
	0x18, 0x18, 0x18, 0x18, 0x39, 0x39, 0x18, 0x18, 0x20, 0x21,
	0x22, 0x23, 0x19, 0x19, 0x19, 0x19, 0x04, 0x05, 0x06, 0x07,
	0x00, 0x01, 0x02, 0x03, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
	0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
	0x18, 0x18, 0x18, 0x18,
};

static unsigned char SEQ_HX83102D_BOE_CU_08[] = {
	0xD6,
	0x18, 0x18, 0x19, 0x19, 0x39, 0x39, 0x18, 0x18, 0x23, 0x22,
	0x21, 0x20, 0x18, 0x18, 0x19, 0x19, 0x03, 0x02, 0x01, 0x00,
	0x07, 0x06, 0x05, 0x04, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
	0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
	0x18, 0x18, 0x18, 0x18,
};

static unsigned char SEQ_HX83102D_BOE_CU_09[] = {
	0xE7,
	0xFF, 0x0F, 0x00, 0x00,
};

static unsigned char SEQ_HX83102D_BOE_CU_10[] = {
	0xBD,
	0x01,
};

static unsigned char SEQ_HX83102D_BOE_CU_11[] = {
	0xE7,
	0x01,
};

static unsigned char SEQ_HX83102D_BOE_CU_12[] = {
	0xBD,
	0x00,
};

static unsigned char SEQ_HX83102D_BOE_CU_13[] = {
	0xBD,
	0x02,
};

static unsigned char SEQ_HX83102D_BOE_CU_14[] = {
	0xD8,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xF0,
};

static unsigned char SEQ_HX83102D_BOE_CU_15[] = {
	0xBD,
	0x03,
};

static unsigned char SEQ_HX83102D_BOE_CU_16[] = {
	0xD8,
	0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xA0, 0xAA, 0xAA, 0xAA, 0xAA,
	0xAA, 0xA0, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xA0, 0xAA, 0xAA,
	0xAA, 0xAA, 0xAA, 0xA0,
};

static unsigned char SEQ_HX83102D_BOE_CU_17[] = {
	0xBD,
	0x00,
};

static unsigned char SEQ_HX83102D_BOE_CU_18[] = {
	0xE0,
	0x00, 0x01, 0x06, 0x0A, 0x0F, 0x11, 0x27, 0x2F, 0x39, 0x39,
	0x5C, 0x64, 0x72, 0x82, 0x87, 0x96, 0xA3, 0xC4, 0xC6, 0x63,
	0x6C, 0x77, 0x7F, 0x00, 0x01, 0x06, 0x0A, 0x0F, 0x11, 0x27,
	0x2F, 0x39, 0x39, 0x5C, 0x6E, 0x7A, 0x98, 0x9B, 0xAA, 0xB7,
	0xC4, 0xC6, 0x63, 0x6C, 0x77, 0x7F,
};

static unsigned char SEQ_HX83102D_BOE_CU_19[] = {
	0xBA,
	0x70, 0x23, 0xA8, 0x8B, 0xB2, 0x80, 0x80, 0x01, 0x10, 0x00,
	0x00, 0x00, 0x0D, 0x3D, 0x02, 0x77, 0x04, 0x01, 0x00,
};

static unsigned char SEQ_HX83102D_BOE_CU_20[] = {
	0xBD,
	0x01,
};

static unsigned char SEQ_HX83102D_BOE_CU_21[] = {
	0xCB,
	0x01,
};

static unsigned char SEQ_HX83102D_BOE_CU_22[] = {
	0xBD,
	0x00,
};

static unsigned char SEQ_HX83102D_BOE_CU_23[] = {
	0xCB,
	0x00, 0x53, 0x00, 0x02, 0x6C,
};

static unsigned char SEQ_HX83102D_BOE_CU_24[] = {
	0xBF,
	0xFC, 0x00, 0x24, 0x9E, 0xF6, 0x00, 0x5D,
};

static unsigned char SEQ_HX83102D_BOE_CU_25[] = {
	0xBD,
	0x02,
};

static unsigned char SEQ_HX83102D_BOE_CU_26[] = {
	0xB4,
	0x42, 0x00, 0x33, 0x00, 0x33, 0x88, 0xB3, 0x00,
};

static unsigned char SEQ_HX83102D_BOE_CU_27[] = {
	0xBD,
	0x00,
};

static unsigned char SEQ_HX83102D_BOE_CU_28[] = {
	0xD1,
	0x20, 0x01,
};

static unsigned char SEQ_HX83102D_BOE_CU_29[] = {
	0xBD,
	0x02,
};

static unsigned char SEQ_HX83102D_BOE_CU_30[] = {
	0xB1,
	0x7F, 0x03, 0xFF,
};

static unsigned char SEQ_HX83102D_BOE_CU_31[] = {
	0xBD,
	0x00,
};

static unsigned char SEQ_HX83102D_BOE_CU_32[] = {
	0xBD,
	0x01,
};

static unsigned char SEQ_HX83102D_BOE_CU_33[] = {
	0xD3,
	0x01, 0x00, 0x39,
};

static unsigned char SEQ_HX83102D_BOE_CU_34[] = {
	0xBD,
	0x00,
};

static unsigned char SEQ_HX83102D_BOE_CU_35[] = {
	0xC9,
	0x04, 0x65, 0x90, 0x01,
};

static unsigned char SEQ_HX83102D_BOE_CU_36[] = {
	0x51,
	0x00, 0x00,
};

static unsigned char SEQ_HX83102D_BOE_CU_37[] = {
	0x53,
	0x24,
};

static unsigned char SEQ_HX83102D_BOE_CU_38[] = {
	0xBB,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x80,
};

static struct lcd_seq_info LCD_SEQ_INIT_CU_1[] = {
	{SEQ_HX83102D_BOE_CU_01, ARRAY_SIZE(SEQ_HX83102D_BOE_CU_01), 0, },
	{SEQ_HX83102D_BOE_CU_02, ARRAY_SIZE(SEQ_HX83102D_BOE_CU_02), 0, },
	{SEQ_HX83102D_BOE_CU_03, ARRAY_SIZE(SEQ_HX83102D_BOE_CU_03), 0, },
	{SEQ_HX83102D_BOE_CU_04, ARRAY_SIZE(SEQ_HX83102D_BOE_CU_04), 0, },
	{SEQ_HX83102D_BOE_CU_05, ARRAY_SIZE(SEQ_HX83102D_BOE_CU_05), 0, },
	{SEQ_HX83102D_BOE_CU_06, ARRAY_SIZE(SEQ_HX83102D_BOE_CU_06), 0, },
	{SEQ_HX83102D_BOE_CU_07, ARRAY_SIZE(SEQ_HX83102D_BOE_CU_07), 0, },
	{SEQ_HX83102D_BOE_CU_08, ARRAY_SIZE(SEQ_HX83102D_BOE_CU_08), 0, },
	{SEQ_HX83102D_BOE_CU_09, ARRAY_SIZE(SEQ_HX83102D_BOE_CU_09), 0, },
	{SEQ_HX83102D_BOE_CU_10, ARRAY_SIZE(SEQ_HX83102D_BOE_CU_10), 0, },
	{SEQ_HX83102D_BOE_CU_11, ARRAY_SIZE(SEQ_HX83102D_BOE_CU_11), 0, },
	{SEQ_HX83102D_BOE_CU_12, ARRAY_SIZE(SEQ_HX83102D_BOE_CU_12), 0, },
	{SEQ_HX83102D_BOE_CU_13, ARRAY_SIZE(SEQ_HX83102D_BOE_CU_13), 0, },
	{SEQ_HX83102D_BOE_CU_14, ARRAY_SIZE(SEQ_HX83102D_BOE_CU_14), 0, },
	{SEQ_HX83102D_BOE_CU_15, ARRAY_SIZE(SEQ_HX83102D_BOE_CU_15), 0, },
	{SEQ_HX83102D_BOE_CU_16, ARRAY_SIZE(SEQ_HX83102D_BOE_CU_16), 0, },
	{SEQ_HX83102D_BOE_CU_17, ARRAY_SIZE(SEQ_HX83102D_BOE_CU_17), 0, },
	{SEQ_HX83102D_BOE_CU_18, ARRAY_SIZE(SEQ_HX83102D_BOE_CU_18), 0, },
	{SEQ_HX83102D_BOE_CU_19, ARRAY_SIZE(SEQ_HX83102D_BOE_CU_19), 0, },
	{SEQ_HX83102D_BOE_CU_20, ARRAY_SIZE(SEQ_HX83102D_BOE_CU_20), 0, },
	{SEQ_HX83102D_BOE_CU_21, ARRAY_SIZE(SEQ_HX83102D_BOE_CU_21), 0, },
	{SEQ_HX83102D_BOE_CU_22, ARRAY_SIZE(SEQ_HX83102D_BOE_CU_22), 0, },
	{SEQ_HX83102D_BOE_CU_23, ARRAY_SIZE(SEQ_HX83102D_BOE_CU_23), 0, },
	{SEQ_HX83102D_BOE_CU_24, ARRAY_SIZE(SEQ_HX83102D_BOE_CU_24), 0, },
	{SEQ_HX83102D_BOE_CU_25, ARRAY_SIZE(SEQ_HX83102D_BOE_CU_25), 0, },
	{SEQ_HX83102D_BOE_CU_26, ARRAY_SIZE(SEQ_HX83102D_BOE_CU_26), 0, },
	{SEQ_HX83102D_BOE_CU_27, ARRAY_SIZE(SEQ_HX83102D_BOE_CU_27), 0, },
	{SEQ_HX83102D_BOE_CU_28, ARRAY_SIZE(SEQ_HX83102D_BOE_CU_28), 0, },
	{SEQ_HX83102D_BOE_CU_29, ARRAY_SIZE(SEQ_HX83102D_BOE_CU_29), 0, },
	{SEQ_HX83102D_BOE_CU_30, ARRAY_SIZE(SEQ_HX83102D_BOE_CU_30), 0, },
	{SEQ_HX83102D_BOE_CU_31, ARRAY_SIZE(SEQ_HX83102D_BOE_CU_31), 0, },
	{SEQ_HX83102D_BOE_CU_32, ARRAY_SIZE(SEQ_HX83102D_BOE_CU_32), 0, },
	{SEQ_HX83102D_BOE_CU_33, ARRAY_SIZE(SEQ_HX83102D_BOE_CU_33), 0, },
	{SEQ_HX83102D_BOE_CU_34, ARRAY_SIZE(SEQ_HX83102D_BOE_CU_34), 0, },
	{SEQ_HX83102D_BOE_CU_35, ARRAY_SIZE(SEQ_HX83102D_BOE_CU_35), 0, },
	{SEQ_HX83102D_BOE_CU_36, ARRAY_SIZE(SEQ_HX83102D_BOE_CU_36), 0, },
	{SEQ_HX83102D_BOE_CU_37, ARRAY_SIZE(SEQ_HX83102D_BOE_CU_37), 0, },
	{SEQ_HX83102D_BOE_CU_38, ARRAY_SIZE(SEQ_HX83102D_BOE_CU_38), 0, },
};

static unsigned char SEQ_HX83102D_BOE_AL_01[] = {
	0xB9,
	0x83, 0x10, 0x2D,
};

static unsigned char SEQ_HX83102D_BOE_AL_02[] = {
	0xB1,
	0x22, 0x44, 0x31, 0x31, 0x22, 0x34, 0x2F, 0x57, 0x08, 0x08,
	0x08, 0x67, 0x67, 0x71
};

static unsigned char SEQ_HX83102D_BOE_AL_03[] = {
	0xB2,
	0x00, 0x00, 0x06, 0x18, 0x00, 0x0E, 0xFE, 0x40, 0x00, 0x00,
	0x00, 0x00, 0xF4, 0xA0,
};

static unsigned char SEQ_HX83102D_BOE_AL_04[] = {
	0xB4,
	0x19, 0x59, 0x19, 0x59, 0x19, 0x59, 0x19, 0x59, 0x05, 0xFF,
	0x03, 0x00, 0x00, 0xFF,
};

static unsigned char SEQ_HX83102D_BOE_AL_05[] = {
	0xCC,
	0x02,
};

static unsigned char SEQ_HX83102D_BOE_AL_06[] = {
	0xC0,
	0x33, 0x33, 0x00, 0x00, 0x19, 0x42, 0x00, 0x08, 0x00, 0x1A,
	0x1B,
};

static unsigned char SEQ_HX83102D_BOE_AL_07[] = {
	0xC7,
	0x00, 0xC2, 0x00, 0x10, 0x42, 0xC0,
};

static unsigned char SEQ_HX83102D_BOE_AL_08[] = {
	0xD3,
	0x06, 0x02, 0x3C, 0x03, 0x00, 0x08, 0x00, 0x37, 0x00, 0x33,
	0x33, 0x08, 0x08, 0x00, 0x00, 0x32, 0x10, 0x06, 0x00, 0x06,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x12, 0x00, 0x00,
};

static unsigned char SEQ_HX83102D_BOE_AL_09[] = {
	0xD5,
	0x18, 0x18, 0x18, 0x18, 0x39, 0x39, 0x18, 0x18, 0x20, 0x21,
	0x22, 0x23, 0x19, 0x19, 0x19, 0x19, 0x04, 0x05, 0x06, 0x07,
	0x00, 0x01, 0x02, 0x03, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
	0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
	0x18, 0x18, 0x18, 0x18,
};

static unsigned char SEQ_HX83102D_BOE_AL_10[] = {
	0xD6,
	0x18, 0x18, 0x19, 0x19, 0x39, 0x39, 0x18, 0x18, 0x23, 0x22,
	0x21, 0x20, 0x18, 0x18, 0x19, 0x19, 0x03, 0x02, 0x01, 0x00,
	0x07, 0x06, 0x05, 0x04, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
	0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
	0x18, 0x18, 0x18, 0x18,
};

static unsigned char SEQ_HX83102D_BOE_AL_11[] = {
	0xE7,
	0xFF, 0x0F, 0x00, 0x00,
};

static unsigned char SEQ_HX83102D_BOE_AL_12[] = {
	0xBD,
	0x01,
};

static unsigned char SEQ_HX83102D_BOE_AL_13[] = {
	0xE7,
	0x01,
};

static unsigned char SEQ_HX83102D_BOE_AL_14[] = {
	0xBD,
	0x00,
};

static unsigned char SEQ_HX83102D_BOE_AL_15[] = {
	0xBD,
	0x02,
};

static unsigned char SEQ_HX83102D_BOE_AL_16[] = {
	0xD8,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xF0,
};

static unsigned char SEQ_HX83102D_BOE_AL_17[] = {
	0xBD,
	0x03,
};

static unsigned char SEQ_HX83102D_BOE_AL_18[] = {
	0xD8,
	0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xA0, 0xAA, 0xAA, 0xAA, 0xAA,
	0xAA, 0xA0, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xA0, 0xAA, 0xAA,
	0xAA, 0xAA, 0xAA, 0xA0,
};

static unsigned char SEQ_HX83102D_BOE_AL_19[] = {
	0xBD,
	0x00,
};

static unsigned char SEQ_HX83102D_BOE_AL_20[] = {
	0xE0,
	0x00, 0x01, 0x06, 0x09, 0x0E, 0x13, 0x23, 0x2D, 0x36, 0x35,
	0x56, 0x5E, 0x6C, 0x80, 0x82, 0x95, 0xA0, 0xBF, 0xC2, 0x62,
	0x6B, 0x77, 0x7F, 0x00, 0x01, 0x06, 0x09, 0x0E, 0x13, 0x23,
	0x2D, 0x36, 0x35, 0x56, 0x66, 0x74, 0x8C, 0x92, 0xA5, 0xB0,
	0xBF, 0xC2, 0x62, 0x6B, 0x77, 0x7F,
};

static unsigned char SEQ_HX83102D_BOE_AL_21[] = {
	0xBA,
	0x70, 0x23, 0xA8, 0x8B, 0xB2, 0x80, 0x80, 0x01, 0x10, 0x00,
	0x00, 0x00, 0x0D, 0x3D, 0x02, 0x77, 0x04, 0x01, 0x00,
};

static unsigned char SEQ_HX83102D_BOE_AL_22[] = {
	0xBD,
	0x01,
};

static unsigned char SEQ_HX83102D_BOE_AL_23[] = {
	0xCB,
	0x01,
};

static unsigned char SEQ_HX83102D_BOE_AL_24[] = {
	0xBD,
	0x00,
};

static unsigned char SEQ_HX83102D_BOE_AL_25[] = {
	0xCB,
	0x00, 0x53, 0x00, 0x02, 0x6C,
};

static unsigned char SEQ_HX83102D_BOE_AL_26[] = {
	0xBF,
	0xFC, 0x00, 0x04, 0x9E, 0xF6, 0x00, 0x4D,
};

static unsigned char SEQ_HX83102D_BOE_AL_27[] = {
	0xBD,
	0x02,
};

static unsigned char SEQ_HX83102D_BOE_AL_28[] = {
	0xB4,
	0x42, 0x00, 0xB3, 0x00, 0x33, 0xBB, 0xB3, 0x00,
};

static unsigned char SEQ_HX83102D_BOE_AL_29[] = {
	0xBD,
	0x00,
};

static unsigned char SEQ_HX83102D_BOE_AL_30[] = {
	0xD1,
	0x20, 0x01,
};

static unsigned char SEQ_HX83102D_BOE_AL_31[] = {
	0xBD,
	0x02,
};

static unsigned char SEQ_HX83102D_BOE_AL_32[] = {
	0xB1,
	0x7F, 0x03, 0xFF,
};

static unsigned char SEQ_HX83102D_BOE_AL_33[] = {
	0xBD,
	0x00,
};

static unsigned char SEQ_HX83102D_BOE_AL_34[] = {
	0xBD,
	0x01,
};

static unsigned char SEQ_HX83102D_BOE_AL_35[] = {
	0xD3,
	0x01, 0x00, 0x39,
};

static unsigned char SEQ_HX83102D_BOE_AL_36[] = {
	0xBD,
	0x00,
};

static unsigned char SEQ_HX83102D_BOE_AL_37[] = {
	0xC9,
	0x04, 0x65, 0x90, 0x01,
};

static unsigned char SEQ_HX83102D_BOE_AL_38[] = {
	0x51,
	0x00, 0x00,
};

static unsigned char SEQ_HX83102D_BOE_AL_39[] = {
	0x53,
	0x24,
};

static unsigned char SEQ_HX83102D_BOE_AL_40[] = {
	0xBB,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x80,
};

static struct lcd_seq_info LCD_SEQ_INIT_AL_1[] = {
	{SEQ_HX83102D_BOE_AL_01, ARRAY_SIZE(SEQ_HX83102D_BOE_AL_01), 0, },
	{SEQ_HX83102D_BOE_AL_02, ARRAY_SIZE(SEQ_HX83102D_BOE_AL_02), 0, },
	{SEQ_HX83102D_BOE_AL_03, ARRAY_SIZE(SEQ_HX83102D_BOE_AL_03), 0, },
	{SEQ_HX83102D_BOE_AL_04, ARRAY_SIZE(SEQ_HX83102D_BOE_AL_04), 0, },
	{SEQ_HX83102D_BOE_AL_05, ARRAY_SIZE(SEQ_HX83102D_BOE_AL_05), 0, },
	{SEQ_HX83102D_BOE_AL_06, ARRAY_SIZE(SEQ_HX83102D_BOE_AL_06), 0, },
	{SEQ_HX83102D_BOE_AL_07, ARRAY_SIZE(SEQ_HX83102D_BOE_AL_07), 0, },
	{SEQ_HX83102D_BOE_AL_08, ARRAY_SIZE(SEQ_HX83102D_BOE_AL_08), 0, },
	{SEQ_HX83102D_BOE_AL_09, ARRAY_SIZE(SEQ_HX83102D_BOE_AL_09), 0, },
	{SEQ_HX83102D_BOE_AL_10, ARRAY_SIZE(SEQ_HX83102D_BOE_AL_10), 0, },
	{SEQ_HX83102D_BOE_AL_11, ARRAY_SIZE(SEQ_HX83102D_BOE_AL_11), 0, },
	{SEQ_HX83102D_BOE_AL_12, ARRAY_SIZE(SEQ_HX83102D_BOE_AL_12), 0, },
	{SEQ_HX83102D_BOE_AL_13, ARRAY_SIZE(SEQ_HX83102D_BOE_AL_13), 0, },
	{SEQ_HX83102D_BOE_AL_14, ARRAY_SIZE(SEQ_HX83102D_BOE_AL_14), 0, },
	{SEQ_HX83102D_BOE_AL_15, ARRAY_SIZE(SEQ_HX83102D_BOE_AL_15), 0, },
	{SEQ_HX83102D_BOE_AL_16, ARRAY_SIZE(SEQ_HX83102D_BOE_AL_16), 0, },
	{SEQ_HX83102D_BOE_AL_17, ARRAY_SIZE(SEQ_HX83102D_BOE_AL_17), 0, },
	{SEQ_HX83102D_BOE_AL_18, ARRAY_SIZE(SEQ_HX83102D_BOE_AL_18), 0, },
	{SEQ_HX83102D_BOE_AL_19, ARRAY_SIZE(SEQ_HX83102D_BOE_AL_19), 0, },
	{SEQ_HX83102D_BOE_AL_20, ARRAY_SIZE(SEQ_HX83102D_BOE_AL_20), 0, },
	{SEQ_HX83102D_BOE_AL_21, ARRAY_SIZE(SEQ_HX83102D_BOE_AL_21), 0, },
	{SEQ_HX83102D_BOE_AL_22, ARRAY_SIZE(SEQ_HX83102D_BOE_AL_22), 0, },
	{SEQ_HX83102D_BOE_AL_23, ARRAY_SIZE(SEQ_HX83102D_BOE_AL_23), 0, },
	{SEQ_HX83102D_BOE_AL_24, ARRAY_SIZE(SEQ_HX83102D_BOE_AL_24), 0, },
	{SEQ_HX83102D_BOE_AL_25, ARRAY_SIZE(SEQ_HX83102D_BOE_AL_25), 0, },
	{SEQ_HX83102D_BOE_AL_26, ARRAY_SIZE(SEQ_HX83102D_BOE_AL_26), 0, },
	{SEQ_HX83102D_BOE_AL_27, ARRAY_SIZE(SEQ_HX83102D_BOE_AL_27), 0, },
	{SEQ_HX83102D_BOE_AL_28, ARRAY_SIZE(SEQ_HX83102D_BOE_AL_28), 0, },
	{SEQ_HX83102D_BOE_AL_29, ARRAY_SIZE(SEQ_HX83102D_BOE_AL_29), 0, },
	{SEQ_HX83102D_BOE_AL_30, ARRAY_SIZE(SEQ_HX83102D_BOE_AL_30), 0, },
	{SEQ_HX83102D_BOE_AL_31, ARRAY_SIZE(SEQ_HX83102D_BOE_AL_31), 0, },
	{SEQ_HX83102D_BOE_AL_32, ARRAY_SIZE(SEQ_HX83102D_BOE_AL_32), 0, },
	{SEQ_HX83102D_BOE_AL_33, ARRAY_SIZE(SEQ_HX83102D_BOE_AL_33), 0, },
	{SEQ_HX83102D_BOE_AL_34, ARRAY_SIZE(SEQ_HX83102D_BOE_AL_34), 0, },
	{SEQ_HX83102D_BOE_AL_35, ARRAY_SIZE(SEQ_HX83102D_BOE_AL_35), 0, },
	{SEQ_HX83102D_BOE_AL_36, ARRAY_SIZE(SEQ_HX83102D_BOE_AL_36), 0, },
	{SEQ_HX83102D_BOE_AL_37, ARRAY_SIZE(SEQ_HX83102D_BOE_AL_37), 0, },
	{SEQ_HX83102D_BOE_AL_38, ARRAY_SIZE(SEQ_HX83102D_BOE_AL_38), 0, },
	{SEQ_HX83102D_BOE_AL_39, ARRAY_SIZE(SEQ_HX83102D_BOE_AL_39), 0, },
	{SEQ_HX83102D_BOE_AL_40, ARRAY_SIZE(SEQ_HX83102D_BOE_AL_40), 0, },
};

static unsigned int brightness_table[EXTEND_BRIGHTNESS + 1] = {
	0,
	20, 21, 23, 25, 27, 28, 30, 32, 34, 36, /* 1: 20 */
	37, 39, 41, 43, 44, 46, 48, 50, 52, 53,
	55, 57, 59, 60, 62, 64, 66, 68, 69, 71,
	73, 75, 76, 78, 80, 82, 84, 85, 87, 89,
	91, 92, 94, 96, 98, 100, 101, 103, 105, 107,
	108, 110, 112, 114, 116, 117, 119, 121, 123, 124,
	126, 128, 130, 132, 133, 135, 137, 139, 141, 142,
	144, 146, 148, 149, 151, 153, 155, 157, 158, 160,
	162, 164, 165, 167, 169, 171, 173, 174, 176, 178,
	180, 181, 183, 185, 187, 189, 190, 192, 194, 196,
	197, 199, 201, 203, 205, 206, 208, 210, 212, 213,
	215, 217, 219, 221, 222, 224, 226, 228, 229, 231,
	233, 235, 237, 238, 240, 242, 244, 246, 264, 282, /* 128: 246 */
	300, 318, 336, 354, 372, 391, 409, 427, 445, 463,
	481, 499, 518, 536, 554, 572, 590, 608, 626, 645,
	663, 681, 699, 717, 735, 753, 772, 790, 808, 826,
	844, 862, 880, 899, 917, 935, 953, 971, 989, 1007,
	1026, 1044, 1062, 1080, 1098, 1116, 1134, 1153, 1171, 1189,
	1207, 1225, 1243, 1261, 1280, 1298, 1316, 1334, 1352, 1370,
	1388, 1407, 1425, 1443, 1461, 1479, 1497, 1515, 1534, 1552,
	1570, 1588, 1606, 1624, 1642, 1661, 1679, 1697, 1715, 1733,
	1751, 1769, 1788, 1806, 1824, 1842, 1860, 1878, 1896, 1915,
	1933, 1951, 1969, 1987, 2005, 2023, 2042, 2060, 2078, 2096,
	2114, 2132, 2150, 2169, 2187, 2205, 2223, 2241, 2259, 2277,
	2296, 2314, 2332, 2350, 2368, 2386, 2404, 2423, 2441, 2459,
	2477, 2495, 2513, 2531, 2550, 2550, 2550, 2550, 2550, 2550, /* 255: 2550 */
	2550, 2550, 2550, 2550, 2550, 2550, 2550, 2550, 2550, 2550,
	2550, 2550, 2550, 2550, 2550, 2550, 2550, 2550, 2550, 2550,
	2550, 2550, 2550, 2550, 2550, 2550, 2550, 2550, 2550, 2550,
	2550, 2550, 2550, 2550, 2550, 2550, 2550, 2550, 2550, 2550,
	2550, 2550, 2550, 2550, 2550, 2960,
};
#endif /* __HX83102D_BOE_PARAM_H__ */
