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

#include "winman.h"

//==================================================File Scope Variables==================================================
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

//==========================================================Code=========================================================
void KillGLWindow(void)
{
	if(fullscreen)
	{
		ChangeDisplaySettings(NULL, 0); //switch to desktop
		ShowCursor(TRUE);
	}

    ///////////////////////////////////////////////////////////////////
    // Delete OpenGL Rendering Context if exists
    ///////////////////////////////////////////////////////////////////
	if(hRC)
	{
		if(!wglMakeCurrent(NULL, NULL))
			MessageBox(NULL, _T("Release of RC and DC Failed"), _T("Shutdown Error"), MB_OK | MB_ICONERROR);
		if(!wglDeleteContext(hRC))
			MessageBox(NULL, _T("Could not delete Rendering Context"), _T("Shutdown Error"), MB_OK | MB_ICONERROR);
		hRC = NULL;
	}

    ///////////////////////////////////////////////////////////////////
    // Release Device Context if exists
    ///////////////////////////////////////////////////////////////////
	if(hDC && !ReleaseDC(hWnd, hDC))
	{
		MessageBox(NULL, _T("Could not release Device Context"), _T("Shutdown Error"), MB_OK | MB_ICONERROR);
		hDC = NULL;
	}

    ///////////////////////////////////////////////////////////////////
    // Destroy Window if exists
    ///////////////////////////////////////////////////////////////////
	if(hWnd && !DestroyWindow(hWnd))
	{
		MessageBox(NULL, _T("Could not destroy Window Handle"), _T("Shutdown Error"), MB_OK | MB_ICONERROR);
		hWnd = NULL;
	}

	///////////////////////////////////////////////////////////////////
	// Unregister Window Class
	///////////////////////////////////////////////////////////////////
	if(!UnregisterClass(windowClass, hInstance))
	{
		MessageBox(NULL, _T("Could not unregister Window Class"), _T("Shutdown Error"), MB_OK | MB_ICONERROR);
		hInstance = NULL;
	}
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	switch(uMsg)
	{
	case WM_CLOSE:
        PostQuitMessage(0);
        return 0;
    case WM_MOVE:
		{
		    /*
		     * HIWORD and LOWORD of lParam yields ClientRect position.
		     * Set WindowRect position instead of ClientRect position.
		     */
		    RECT rect;
		    GetWindowRect(hWnd, &rect);
		    winmanState.windowPosX = rect.left;
            winmanState.windowPosY = rect.top;
		}
		break;
	case WM_SIZE:
		ResizeGLScene(LOWORD(lParam), HIWORD(lParam));
		{
			RECT clientRect;
			GetClientRect(hWnd, &clientRect);
			winmanState.windowWidth = clientRect.right - clientRect.left;
			winmanState.windowHeight = clientRect.bottom - clientRect.top;
		}
		break;

    case WM_SIZING:
        //does not resize if WS_EX_NOACTIVATE, must do so manually
		if(GetWindowLongPtr(hWnd, GWL_EXSTYLE) & WS_EX_NOACTIVATE)
        {
            RECT *prc = (RECT *)lParam;
            SetWindowPos(hWnd, NULL, prc->left, prc->top, prc->right - prc->left, prc->bottom - prc->top, 0);
        }
        break;

	case WM_PAINT:
		winmanState.callbacks.winmanDisplayFuncCB();
		break;

	case WM_MOUSEMOVE:
		winmanState.mouse.pos.x = LOWORD(lParam);
		winmanState.mouse.pos.y = HIWORD(lParam);
		if(wParam & MK_LBUTTON || wParam & MK_MBUTTON || wParam & MK_RBUTTON)
			winmanState.callbacks.winmanMotionFuncCB(winmanState.mouse.pos.x, winmanState.mouse.pos.y);
		else
			winmanState.callbacks.winmanPassiveMotionFuncCB(winmanState.mouse.pos.x, winmanState.mouse.pos.y);
		break;
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
        {
            winmanState.mouse.pos.x = LOWORD(lParam);
            winmanState.mouse.pos.y = HIWORD(lParam);

            GLboolean pressed = GL_TRUE;
            int button = -1;

            switch(uMsg)
            {
            case WM_LBUTTONDOWN:
                pressed = GL_TRUE;
                button = WINMAN_LEFT_BUTTON;
                break;
            case WM_MBUTTONDOWN:
                pressed = GL_TRUE;
                button = WINMAN_MIDDLE_BUTTON;
                break;
            case WM_RBUTTONDOWN:
                pressed = GL_TRUE;
                button = WINMAN_RIGHT_BUTTON;
                break;
            case WM_LBUTTONUP:
                pressed = GL_FALSE;
                button = WINMAN_LEFT_BUTTON;
                break;
            case WM_MBUTTONUP:
                pressed = GL_FALSE;
                button = WINMAN_MIDDLE_BUTTON;
                break;
            case WM_RBUTTONUP:
                pressed = GL_FALSE;
                button = WINMAN_RIGHT_BUTTON;
                break;
            default:
                pressed = GL_FALSE;
                button = -1;
                break;
            }

            winmanState.callbacks.winmanMouseFuncCB(button,
                                                    pressed ? WINMAN_DOWN : WINMAN_UP,
                                                    winmanState.mouse.pos.x,
                                                    winmanState.mouse.pos.y);
        }
        break;


    case WM_SYSKEYDOWN:
    case WM_KEYDOWN:
        {
            GetCursorPos(&winmanState.mouse.pos);
            int keypress = -1;
            switch(wParam)
            {
               /*
                * Code in this block borrowed from freeglut,
                * to convert windows keys to winman keys
                * This was done to increase compatibility with
                * glut, which oF is based on.
                */
#				define KEY(a,b) case a: keypress = b; break;

                KEY( VK_F1,     WINMAN_KEY_F1        );
                KEY( VK_F2,     WINMAN_KEY_F2        );
                KEY( VK_F3,     WINMAN_KEY_F3        );
                KEY( VK_F4,     WINMAN_KEY_F4        );
                KEY( VK_F5,     WINMAN_KEY_F5        );
                KEY( VK_F6,     WINMAN_KEY_F6        );
                KEY( VK_F7,     WINMAN_KEY_F7        );
                KEY( VK_F8,     WINMAN_KEY_F8        );
                KEY( VK_F9,     WINMAN_KEY_F9        );
                KEY( VK_F10,    WINMAN_KEY_F10       );
                KEY( VK_F11,    WINMAN_KEY_F11       );
                KEY( VK_F12,    WINMAN_KEY_F12       );
                KEY( VK_PRIOR,  WINMAN_KEY_PAGE_UP   );
                KEY( VK_NEXT,   WINMAN_KEY_PAGE_DOWN );
                KEY( VK_HOME,   WINMAN_KEY_HOME      );
                KEY( VK_END,    WINMAN_KEY_END       );
                KEY( VK_LEFT,   WINMAN_KEY_LEFT      );
                KEY( VK_UP,     WINMAN_KEY_UP        );
                KEY( VK_RIGHT,  WINMAN_KEY_RIGHT     );
                KEY( VK_DOWN,   WINMAN_KEY_DOWN      );
                KEY( VK_INSERT, WINMAN_KEY_INSERT    );

                case VK_DELETE:
#					define WINMAN_DELETE 127 //the ascii character code for delete

                    /* The delete key should be treated as an ASCII keypress: */
                    winmanState.callbacks.winmanKeyboardFuncCB(WINMAN_DELETE, winmanState.mouse.pos.x, winmanState.mouse.pos.y);
                    break;
            }
            if(keypress != -1)
                winmanState.callbacks.winmanSpecialFuncCB(keypress, winmanState.mouse.pos.x, winmanState.mouse.pos.y);
        }
		break;
	case WM_SYSCHAR:
	case WM_CHAR:
		{
			GetCursorPos(&winmanState.mouse.pos);
			winmanState.callbacks.winmanKeyboardFuncCB((char)wParam, winmanState.mouse.pos.x, winmanState.mouse.pos.y);
		}
		break;
	case WM_SYSKEYUP:
	case WM_KEYUP:
		{
			GetCursorPos(&winmanState.mouse.pos);
			int keypress = -1;
			switch(wParam)
			{
				/* macro defined in WM_KEYDOWN */
				KEY( VK_F1,     WINMAN_KEY_F1        );
				KEY( VK_F2,     WINMAN_KEY_F2        );
				KEY( VK_F3,     WINMAN_KEY_F3        );
				KEY( VK_F4,     WINMAN_KEY_F4        );
				KEY( VK_F5,     WINMAN_KEY_F5        );
				KEY( VK_F6,     WINMAN_KEY_F6        );
				KEY( VK_F7,     WINMAN_KEY_F7        );
				KEY( VK_F8,     WINMAN_KEY_F8        );
				KEY( VK_F9,     WINMAN_KEY_F9        );
				KEY( VK_F10,    WINMAN_KEY_F10       );
				KEY( VK_F11,    WINMAN_KEY_F11       );
				KEY( VK_F12,    WINMAN_KEY_F12       );
				KEY( VK_PRIOR,  WINMAN_KEY_PAGE_UP   );
				KEY( VK_NEXT,   WINMAN_KEY_PAGE_DOWN );
				KEY( VK_HOME,   WINMAN_KEY_HOME      );
				KEY( VK_END,    WINMAN_KEY_END       );
				KEY( VK_LEFT,   WINMAN_KEY_LEFT      );
				KEY( VK_UP,     WINMAN_KEY_UP        );
				KEY( VK_RIGHT,  WINMAN_KEY_RIGHT     );
				KEY( VK_DOWN,   WINMAN_KEY_DOWN      );
				KEY( VK_INSERT, WINMAN_KEY_INSERT    );

				case VK_DELETE:
					/* WINMAN_DELETE is defined in WM_KEYDOWN */
					/* The delete key should be treated as an ASCII keypress: */
					winmanState.callbacks.winmanKeyboardUpFuncCB(WINMAN_DELETE, winmanState.mouse.pos.x, winmanState.mouse.pos.y);
					break;
				default:
					{
						BYTE state[ 256 ];
						WORD code [ 2 ];

						GetKeyboardState( state );

						if( ToAscii( (UINT)wParam, 0, state, code, 0 ) == 1 )
							wParam = code[ 0 ];
						winmanState.callbacks.winmanKeyboardUpFuncCB((char)wParam, winmanState.mouse.pos.x, winmanState.mouse.pos.y);
					}
					break;
			}
			if(keypress != -1)
				winmanState.callbacks.winmanSpecialUpFuncCB(keypress, winmanState.mouse.pos.x, winmanState.mouse.pos.y);
		}
		break;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void ResizeGLScene(int width, int height)
{
	if(height == 0)
		height = 1;
	glViewport(0, 0, width, height);
}

void windowInitClass()
{
    ///////////////////////////////////////////////////////////////////
    // Standard window intialization
    ///////////////////////////////////////////////////////////////////
	hInstance = GetModuleHandle(NULL);
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize		= sizeof(WNDCLASSEX);
	wc.style		= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc	= WindowProc;
	wc.hInstance	= hInstance;
	wc.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName= windowClass;

	if(!(RegisterClassEx(&wc)))
	{
	    MessageBox(NULL, "Failed to Register Window Class", "Error", MB_OK | MB_ICONERROR);
	}
}

void windowInitWindowSize(int w, int h)
{
	winmanState.windowWidth = w;
	winmanState.windowHeight = h;
}


void windowCreateGLWindowAdvanced(	DWORD dwExStyle,
											TCHAR* windowName,
											DWORD dwStyle,
											int x,
											int y,
											int width,
											int height,
											HWND parent,
											HMENU hMenu,
											HINSTANCE hInstance,
											LPVOID lpParam )
{
	if(!fullscreen)
	{
		/*
		 * we do not want to do this if we are in fullscreen
		 * because specific styles are required for fullscreen
		 */
		winDwExStyle = dwExStyle;
		winDwStyle = dwStyle;
	}

	if(!(hWnd = CreateWindowEx( winDwExStyle,
                                windowClass,
                                windowName,
                                winDwStyle,
                                x, y, width, height,
                                parent,
                                hMenu,
                                hInstance,
                                lpParam )))
    {
		MessageBox(NULL, "Failed to create window!", "Error", MB_OK | MB_ICONEXCLAMATION);
        KillGLWindow();
		return;
    }

	/* Adjust the client area, depending on the windowStyle */

	RECT WindowRect;
	WindowRect.left		= (long)0;
	WindowRect.top		= (long)0;
	WindowRect.right	= (long)width + 20;
	WindowRect.bottom	= (long)height + 20;
	AdjustWindowRectEx(&WindowRect, winDwStyle, FALSE, winDwExStyle);

    winmanState.windowWidth = width;
    winmanState.windowHeight = height;
}

void windowCreateGLWindow(TCHAR* windowName, int width, int height)
{
	windowCreateGLWindowAdvanced(	winDwExStyle,
							windowName,
							winDwStyle,
							0,
							0,
							width,
							height,
							NULL,
							NULL,
							hInstance,
							NULL);
}


BOOL windowInitDisplayMode(int width, int height, bool fullscreenFlag /* = false */, DWORD bitsPerPel /* = 16 */)
{
	fullscreen = fullscreenFlag;

	if(fullscreen)
	{
		DEVMODE dmScreenSettings;
		ZeroMemory(&dmScreenSettings, sizeof(DEVMODE));
		dmScreenSettings.dmSize			= sizeof(DEVMODE);
		dmScreenSettings.dmPelsWidth	= width;
		dmScreenSettings.dmPelsHeight	= height;
		dmScreenSettings.dmBitsPerPel	= bitsPerPel;
		dmScreenSettings.dmFields		= DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		if(ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			if(MessageBox(NULL, _T("The requested fullscreen mode is not supported by \nyour video card. Use windowed mode instead?"),
				_T("Error"), MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
			{
				fullscreen = FALSE;
			}
			else
			{
				MessageBox(NULL, _T("Program will now exit"), _T("Error"), MB_OK | MB_ICONSTOP);
				PostQuitMessage(0);
				return FALSE;
			}
		}
	}
	/* need to check again because the user may have cancelled fullscreen mode */
	if(fullscreen)
	{
		winDwExStyle = WS_EX_APPWINDOW;
		winDwStyle = WS_POPUP;
	}
	else
	{
		winDwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		winDwStyle = WS_OVERLAPPEDWINDOW;
	}
	return TRUE;
}

BOOL windowInitDCRC(DWORD pixelFlags  /* = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER */,
					BYTE pixelType /* = PFD_TYPE_RGBA */,
					BYTE depthBits /* = 16 */)
{
	/* pixelFlags MUST support OpenGL (unless we are not using it, which we are) */
	pixelFlags |= PFD_SUPPORT_OPENGL;

	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize		= sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion	= 1;
	pfd.dwFlags		= pixelFlags;
	pfd.iPixelType	= pixelType;
	pfd.cDepthBits	= depthBits;
	pfd.iLayerType	= PFD_MAIN_PLANE;	/* not sure if this is required.
										 * MSDN says it is is ignored but
										 * this may only be for the software
										 * implementation of OpenGL
										 */

	hDC = GetDC(hWnd);
	GLuint PixelFormat = ChoosePixelFormat(hDC, &pfd);
	SetPixelFormat(hDC, PixelFormat, &pfd);
	hRC = wglCreateContext(hDC);
	wglMakeCurrent(hDC, hRC);

	return TRUE;
}

/*
 * Get different info about the window and screen
 * derived from GLUT's implementation, glutGet(GLenum eWhat)
 */
int winmanGet(GLenum what)
{
	switch(what)
	{
	case SCREEN_WIDTH:	return GetSystemMetrics(SM_CXSCREEN);
	case SCREEN_HEIGHT: return GetSystemMetrics(SM_CYSCREEN);
	case WINDOW_X:		return winmanState.windowPosX;
	case WINDOW_Y:		return winmanState.windowPosY;
	case WINDOW_WIDTH:	return winmanState.windowWidth;
	case WINDOW_HEIGHT:	return winmanState.windowHeight;
	}
	return 0;
}

void winmanPostRedisplay()
{
	winmanState.redisplay = true;
}

void winmanFullScreen()
{
	RECT rect;
	rect.left	= 0;
	rect.top	= 0;
	rect.right	= winmanGet(SCREEN_WIDTH);
	rect.bottom	= winmanGet(SCREEN_HEIGHT);

    /* Optional, but transition seems smoother without it */
    //ShowWindow(hWnd, SW_HIDE);

	SetWindowLongPtr(hWnd, GWL_STYLE, WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	ShowWindow(hWnd, SW_SHOW);

	AdjustWindowRect ( &rect, WS_POPUP | WS_CLIPSIBLINGS |
                       WS_CLIPCHILDREN, FALSE );

	SetWindowPos(	hWnd,
					NULL,
					rect.left,
					rect.top,
					rect.right - rect.left,
					rect.bottom - rect.top,
					SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOSENDCHANGING | SWP_NOZORDER);

	winmanState.fullscreen = true;
}

void winmanReshapeWindow(int width, int height)
{
    /* Optional, but transition seems smoother without it */
    //ShowWindow(hWnd, SW_HIDE);

    SetWindowLongPtr(hWnd, GWL_STYLE, winDwStyle);
    ShowWindow(hWnd, SW_SHOW);

	ResizeGLScene(width, height);
	RECT winRect;
	GetWindowRect(hWnd, &winRect);

	int w = width;
	int h = height;

	/* take window decorations into account */
	w += GetSystemMetrics( SM_CXSIZEFRAME ) * 2;
    h += GetSystemMetrics( SM_CYSIZEFRAME ) * 2 + GetSystemMetrics( SM_CYCAPTION );

	SetWindowPos(   hWnd,
					NULL,
					winRect.left,
					winRect.top,
					w,
					h,
					SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOSENDCHANGING | SWP_NOZORDER);

	/* resizing deactivates fullscreen */
	winmanState.fullscreen = false;

	winmanState.callbacks.winmanReshapeFuncCB(width, height);

	/* force window redisplay */
	winmanState.redisplay = true;

	winmanState.windowWidth = width;
	winmanState.windowHeight = height;

}

void winmanPositionWindow(int x, int y)
{
	RECT winRect;

    /* "GetWindowRect" returns the pixel coordinates of the outside of the window */
    GetWindowRect( hWnd, &winRect );
    MoveWindow(
        hWnd,
        x,
        y,
        winRect.right - winRect.left,
        winRect.bottom - winRect.top,
        TRUE
    );
}

void winmanSetWindowTitle(const TCHAR* title)
{
	SetWindowText(hWnd, title);
}

void winmanSwapBuffers()
{
	SwapBuffers(hDC);
}


void winmanMainLoop()
{
	MSG msg;
	BOOL active = true;

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	while(active)
	{
		if(PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
				active = false;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if(winmanState.redisplay)
			{
				winmanState.redisplay = false;
				RedrawWindow(hWnd, NULL, NULL, RDW_NOERASE | RDW_INTERNALPAINT | RDW_INVALIDATE | RDW_UPDATENOW);
			}
			winmanState.callbacks.winmanIdleFuncCB();
		}
	}
	KillGLWindow();
}

HWND winmanGetActiveWindowHandle()
{
    return hWnd;
}


