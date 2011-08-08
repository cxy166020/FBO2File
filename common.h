/*
 *  common.h
 *  project
 *
 *  Created by Xiaoyan Hu on 12/7/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _COMMON_
#define _COMMON_

#define BL 0x20
#define TB 0x09
#define CR 0x0D
#define LF 0x0A
#define PD 0x23

#define DIMENSION 3

#define MAX_CHAR 255

#define CONFIG_BUF_SIZE 100
#define NAME_BUF_SIZE 100

#ifdef WIN32
#include	<windows.h>
#else

// --------------------------------------------
// RGBTRIPLE - 24 bits pixel
// --------------------------------------------

typedef struct tagRGBTRIPLE			// rgbt
{
	unsigned char	rgbtBlue;		// blue
	unsigned char	rgbtGreen;		// green
	unsigned char	rgbtRed;		// red
	
} RGBTRIPLE, *PRGBTRIPLE;

#endif	// WIN32

#endif