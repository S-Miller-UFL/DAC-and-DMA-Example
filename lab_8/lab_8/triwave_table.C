//*******************************************
//triwave_table.c
//Name: Steven Miller
//Class #: 11318
//PI Name: Anthony Stross
//Description: array that contains data points to make a triangle wave from 0 to 4095
//*******************************************
#include <avr/io.h>
volatile uint16_t triangle_wave[256] =
{
	0x20,0x40,0x60,0x80,0xa0,0xc0,0xe0,0x100,
	0x120,0x140,0x160,0x180,0x1a0,0x1c0,0x1e0,0x200,
	0x220,0x240,0x260,0x280,0x2a0,0x2c0,0x2e0,0x300,
	0x320,0x340,0x360,0x380,0x3a0,0x3c0,0x3e0,0x400,
	0x420,0x440,0x460,0x480,0x4a0,0x4c0,0x4e0,0x500,
	0x520,0x540,0x560,0x580,0x5a0,0x5c0,0x5e0,0x600,
	0x620,0x640,0x660,0x680,0x6a0,0x6c0,0x6e0,0x700,
	0x720,0x740,0x760,0x780,0x7a0,0x7c0,0x7e0,0x800,
	0x81f,0x83f,0x85f,0x87f,0x89f,0x8bf,0x8df,0x8ff,
	0x91f,0x93f,0x95f,0x97f,0x99f,0x9bf,0x9df,0x9ff,
	0xa1f,0xa3f,0xa5f,0xa7f,0xa9f,0xabf,0xadf,0xaff,
	0xb1f,0xb3f,0xb5f,0xb7f,0xb9f,0xbbf,0xbdf,0xbff,
	0xc1f,0xc3f,0xc5f,0xc7f,0xc9f,0xcbf,0xcdf,0xcff,
	0xd1f,0xd3f,0xd5f,0xd7f,0xd9f,0xdbf,0xddf,0xdff,
	0xe1f,0xe3f,0xe5f,0xe7f,0xe9f,0xebf,0xedf,0xeff,
	0xf1f,0xf3f,0xf5f,0xf7f,0xf9f,0xfbf,0xfdf,0xfff,
	0xfdf,0xfbf,0xf9f,0xf7f,0xf5f,0xf3f,0xf1f,0xeff,
	0xedf,0xebf,0xe9f,0xe7f,0xe5f,0xe3f,0xe1f,0xdff,
	0xddf,0xdbf,0xd9f,0xd7f,0xd5f,0xd3f,0xd1f,0xcff,
	0xcdf,0xcbf,0xc9f,0xc7f,0xc5f,0xc3f,0xc1f,0xbff,
	0xbdf,0xbbf,0xb9f,0xb7f,0xb5f,0xb3f,0xb1f,0xaff,
	0xadf,0xabf,0xa9f,0xa7f,0xa5f,0xa3f,0xa1f,0x9ff,
	0x9df,0x9bf,0x99f,0x97f,0x95f,0x93f,0x91f,0x8ff,
	0x8df,0x8bf,0x89f,0x87f,0x85f,0x83f,0x81f,0x800,
	0x7e0,0x7c0,0x7a0,0x780,0x760,0x740,0x720,0x700,
	0x6e0,0x6c0,0x6a0,0x680,0x660,0x640,0x620,0x600,
	0x5e0,0x5c0,0x5a0,0x580,0x560,0x540,0x520,0x500,
	0x4e0,0x4c0,0x4a0,0x480,0x460,0x440,0x420,0x400,
	0x3e0,0x3c0,0x3a0,0x380,0x360,0x340,0x320,0x300,
	0x2e0,0x2c0,0x2a0,0x280,0x260,0x240,0x220,0x200,
	0x1e0,0x1c0,0x1a0,0x180,0x160,0x140,0x120,0x100,
	0xe0,0xc0,0xa0,0x80,0x60,0x40,0x20,0x0
};