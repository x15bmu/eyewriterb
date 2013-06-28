/*
	This file is part of EyeWriterB
	Copyright (C) 2013  Ben Mattinson <eyewriterb@gmail.com>
	
	EyeWriterB is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    EyeWriterB is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with EyeWriterB.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef WINMANINTERNAL_H_
#define WINMANINTERNAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <windows.h>

//============================Defines===============================//
#define WINMAN_APIENTRY


//These are designed to be the same as the keycodes for freeglut
#define	WINMAN_KEY_F1						0x0001
#define	WINMAN_KEY_F2						0x0002
#define	WINMAN_KEY_F3						0x0003
#define	WINMAN_KEY_F4						0x0004
#define	WINMAN_KEY_F5						0x0005
#define	WINMAN_KEY_F6						0x0006
#define	WINMAN_KEY_F7						0x0007
#define	WINMAN_KEY_F8						0x0008
#define	WINMAN_KEY_F9						0x0009
#define	WINMAN_KEY_F10						0x000A
#define	WINMAN_KEY_F11						0x000B
#define	WINMAN_KEY_F12						0x000C
#define	WINMAN_KEY_LEFT						0x0064
#define	WINMAN_KEY_UP						0x0065
#define	WINMAN_KEY_RIGHT					0x0066
#define	WINMAN_KEY_DOWN						0x0067
#define	WINMAN_KEY_PAGE_UP					0x0068
#define	WINMAN_KEY_PAGE_DOWN				0x0069
#define	WINMAN_KEY_HOME						0x006A
#define	WINMAN_KEY_END						0x006B
#define	WINMAN_KEY_INSERT					0x006C

//Mouse
#define	WINMAN_DOWN							0x0000
#define	WINMAN_UP							0x0001

#define	WINMAN_LEFT_BUTTON					0x0000
#define WINMAN_MIDDLE_BUTTON				0x0001
#define WINMAN_RIGHT_BUTTON					0x0002





//===========================Callbacks=============================//

typedef		void (*mouseFunc)(int, int, int, int);
typedef		void (*motionFunc)(int, int);
typedef		void (*passiveMotionFunc)(int, int);
typedef		void (*idleFunc)(void);
typedef		void (*displayFunc)(void) ;


typedef		void (*keyboardFunc)(unsigned char, int, int);
typedef		void (*keyboardUpFunc)(unsigned char, int, int);
typedef		void (*specialFunc)(int, int, int);
typedef		void (*specialUpFunc)(int, int, int);

typedef		void (*reshapeFunc)(int, int);

struct winmanCallbacks
{
	mouseFunc			winmanMouseFuncCB;
	motionFunc			winmanMotionFuncCB;
	passiveMotionFunc	winmanPassiveMotionFuncCB;
	idleFunc			winmanIdleFuncCB;
	displayFunc			winmanDisplayFuncCB;

	keyboardFunc		winmanKeyboardFuncCB;
	keyboardUpFunc		winmanKeyboardUpFuncCB;
	specialFunc			winmanSpecialFuncCB;
	specialUpFunc		winmanSpecialUpFuncCB;

	reshapeFunc			winmanReshapeFuncCB;
};

void	WINMAN_APIENTRY		winmanMouseFunc(mouseFunc callback);
void	WINMAN_APIENTRY		winmanMotionFunc(motionFunc callback);
void	WINMAN_APIENTRY		winmanPassiveMotionFunc(passiveMotionFunc callback);
void	WINMAN_APIENTRY		winmanIdleFunc(idleFunc callback);
void	WINMAN_APIENTRY		winmanDisplayFunc(displayFunc callback);

void	WINMAN_APIENTRY		winmanKeyboardFunc(keyboardFunc callback);
void	WINMAN_APIENTRY		winmanKeyboardUpFunc(keyboardUpFunc callback);
void	WINMAN_APIENTRY		winmanSpecialFunc(specialFunc callback);
void	WINMAN_APIENTRY		winmanSpecialUpFunc(specialUpFunc callback);

void	WINMAN_APIENTRY		winmanReshapeFunc(reshapeFunc callback);



//=============================Structs============================//
struct winmanMouseStruct
{
	POINT pos;
	int leftMB;
	int middleMB;
	int rightMB;
};

struct winmanStateStruct
{
	winmanMouseStruct mouse;
	winmanCallbacks	  callbacks;
	int		windowWidth;
	int		windowHeight;
	int     windowPosX;
	int     windowPosY;
	bool	redisplay;
	bool	fullscreen;
	bool    keys[256];
};

extern winmanStateStruct winmanState; //must be global so we can call it in the LRESULT CALLBACK Window Procedure

#ifdef __cplusplus
} //end extern "C" {
#endif

#endif
