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

#ifndef WINDOW_MANAGER_H_
#define WINDOW_MANAGER_H_

#include <Windows.h>
#include <tchar.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "winmanInternal.h"

#ifdef __cplusplus
extern "C" {
#endif

//====================================================Global Variables====================================================
extern winmanStateStruct winmanState;

//==================================================File Scope Variables==================================================
/*
static HWND hWnd = NULL;
static WNDCLASSEX wc;
static HDC hDC = NULL;
static HGLRC hRC = NULL;
static HINSTANCE hInstance = NULL;

static DWORD winDwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE; //file scope because must be modified if fullscreen
static DWORD winDwStyle = WS_OVERLAPPEDWINDOW;					//file scope because must be modified if fullscreen
static const TCHAR* windowClass = _T("OpenFrameworks Window");

static bool	fullscreen = false;
static int	winWidth = 0;
static int	winHeight = 0;

static PIXELFORMATDESCRIPTOR pfd;
*/
//========================================================enums==========================================================
enum what
{
	SCREEN_WIDTH,
	SCREEN_HEIGHT,
	WINDOW_X,
	WINDOW_Y,
	WINDOW_WIDTH,
	WINDOW_HEIGHT
};

//====================================================API Functions======================================================
void	WINMAN_APIENTRY		windowCreateGLWindow(TCHAR* title, int width, int height);
BOOL	WINMAN_APIENTRY		windowInitDisplayMode(int width, int height, bool fullscreenFlag = false, DWORD bitsPerPel = 16);
BOOL	WINMAN_APIENTRY		windowInitDCRC(DWORD pixelFlags  = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
											BYTE pixelType = PFD_TYPE_RGBA,
											BYTE depthBits = 16);
int		WINMAN_APIENTRY		winmanGet(GLenum what);
void	WINMAN_APIENTRY		winmanMainLoop(void);
void	WINMAN_APIENTRY		winmanPostRedisplay(void);
void	WINMAN_APIENTRY		winmanFullScreen(void);
void	WINMAN_APIENTRY		winmanReshapeWindow(int width, int height);
void	WINMAN_APIENTRY		winmanPositionWindow(int x, int y);
void	WINMAN_APIENTRY		winmanSetWindowTitle(const TCHAR* title);
void	WINMAN_APIENTRY		winmanSwapBuffers(void);

HWND    WINMAN_APIENTRY     winmanGetActiveWindowHandle(void);

//================================================Advanced API Functions=================================================
void	WINMAN_APIENTRY		windowInitWindowSize(int w, int h);
void	WINMAN_APIENTRY		windowCreateGLWindowAdvanced(	DWORD dwExStyle,
															TCHAR* windowName,
															DWORD dwStyle,
															int x,
															int y,
															int width,
															int height,
															HWND parent,
															HMENU hMenu,
															HINSTANCE hInstance,
															LPVOID lpParam );
void    WINMAN_APIENTRY     winmanGetCursorPos();

//===================================================Private Functions===================================================
void windowInitClass();
int windowInitGL();
void ResizeGLScene(int width, int height);


#ifdef __cplusplus
} //end extern "C" {
#endif

#endif
