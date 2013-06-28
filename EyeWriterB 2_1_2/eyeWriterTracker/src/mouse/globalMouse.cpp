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

#include "globalMouse.h"
#include "roundedRect.h"
#include "winman.h"

globalMouse::globalMouse(int& x, int& y, keyboardManager *keybdMan, IplImage *cameraImg)
    : mouse(x, y), windowWidth(500), windowHeight(130), keybdActive(false), cameraImage(cameraImg)
{
    KM = keybdMan;
    mouseByRef = true;
    init();
}

globalMouse::globalMouse(keyboardManager *keybdMan, IplImage *cameraImg)
    : mouse(), windowWidth(500), windowHeight(130), keybdActive(false), cameraImage(cameraImg)
{
    KM = keybdMan;
    mouseByRef = false;
    init();
}

void globalMouse::init()
{
    noClick = NULL;
    singleClick = NULL;
    doubleClick = NULL;
    keybd = NULL;

    keyBackground = NULL;
    keyHighlight = NULL;
    keyRegistered = NULL;
    keyNoClick = NULL;
    keySingleClick = NULL;
    keyDoubleClick = NULL;
    keyKeybd = NULL;

    overlay = NULL;
}

globalMouse::~globalMouse()
{
    deleteButtons();
}

void globalMouse::setOverlayImage(IplImage *cameraImg)
{
    cameraImage = cameraImg;
    if(overlay) {
        overlay->setImage(cameraImage);
    }
}

void globalMouse::setOverlayOpacity(int opacity)
{
    if(overlay)
        overlay->setOpacity(opacity);
}

void globalMouse::setClickTime(int clickTime)
{
    mouse.setClickTime(clickTime);
}

void globalMouse::activate()
{
    ///////////////////////////////////////////////////////////////////
    // Init overlay
    ///////////////////////////////////////////////////////////////////
    overlay = new cameraOverlay(cameraImage);
    overlay->setImageSize((640*2)/4, (480)/4);

    ///////////////////////////////////////////////////////////////////
    // Init keyboard
    ///////////////////////////////////////////////////////////////////
    KM->setTFXMode(TFX_GLOBALMOUSE);
    keybdActive = false; //always start with mouse buttons

    ///////////////////////////////////////////////////////////////////
    // Set rect for restoring the window position later
    ///////////////////////////////////////////////////////////////////
    HWND hWnd = winmanGetActiveWindowHandle();
    GetWindowRect(hWnd, &oldWinRect);

    loadButtonImages();

    ///////////////////////////////////////////////////////////////////
    // Initialize default mouse states
    ///////////////////////////////////////////////////////////////////
    clickType = SM_NOCLICK;
    clickTypeSet = false;
    mouse.setClickType(SM_NOCLICK);

    ///////////////////////////////////////////////////////////////////
    // Initialize window state
    ///////////////////////////////////////////////////////////////////
    resizeAndPosition(windowWidth, windowHeight); //automatically positioned at bottom right
    setWindowStyles(WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, WS_EX_NOACTIVATE | WS_EX_APPWINDOW);
    /*
     * Must set styles after because reshaping the window resets
     * the styles. Must set window position because certain window
     * data is cached and not refreshed until resize.
     */

	SetWindowPos(   hWnd,
					HWND_TOPMOST,
					0, 0, 0, 0, //NOMOVE, NOSIZE
					SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED | SWP_NOACTIVATE);
    ShowWindow(hWnd, SW_SHOW);
}
void globalMouse::deactivate()
{
    delete overlay;
    overlay = NULL;

    KM->setTFXMode(TFX_STANDALONE);

    ///////////////////////////////////////////////////////////////////
    // Restore window properties
    ///////////////////////////////////////////////////////////////////
    winmanReshapeWindow(oldWinRect.right - oldWinRect.left, oldWinRect.bottom - oldWinRect.top);
    winmanPositionWindow(oldWinRect.left, oldWinRect.top);

    restorePrevWindowStyles();
    HWND hWnd = winmanGetActiveWindowHandle();
    SetWindowPos(hWnd,
                 HWND_NOTOPMOST,
                 0, 0, 0, 0, //NOMOVE, NOSIZE
                 SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);

    deleteButtons();

    mouse.restoreCursor();
}

void globalMouse::loadButtonImages()
{
    ///////////////////////////////////////////////////////////////////
    // Init buttons
    ///////////////////////////////////////////////////////////////////
    deleteButtons();

#define LOAD_IMAGE(i)               \
do                                  \
{                                   \
    i = new ofImage();              \
    if( !(i->loadImage("images/GlobalMouse/" #i ".png")) ) \
        throw "\"" #i ".png\" failed to load!" "\n Reverting to standard keys."; \
} while (0)

    try
    {
        ///////////////////////////////////////////////////////////////////
        // Attempt to load key images
        ///////////////////////////////////////////////////////////////////
        LOAD_IMAGE(keyBackground);
        LOAD_IMAGE(keyHighlight);
        LOAD_IMAGE(keyRegistered);
        LOAD_IMAGE(keyNoClick);
        LOAD_IMAGE(keySingleClick);
        LOAD_IMAGE(keyDoubleClick);
        LOAD_IMAGE(keyKeybd);

        noClick = new imageToggleKey(keyBackground, keyHighlight, keyRegistered, keyNoClick);
        singleClick = new imageToggleKey(keyBackground, keyHighlight, keyRegistered, keySingleClick);
        doubleClick = new imageToggleKey(keyBackground, keyHighlight, keyRegistered, keyDoubleClick);
        keybd = new imageKey(keyBackground, keyHighlight, keyRegistered, keyKeybd);

    }
    catch(const char* str)
    {
        ///////////////////////////////////////////////////////////////////
        // Load default keys if no images
        ///////////////////////////////////////////////////////////////////
        MessageBox(NULL, str, "Error!", MB_OK | MB_ICONERROR);

        /* Construct */
        noClick = new imageToggleKey();
        singleClick = new imageToggleKey();
        doubleClick = new imageToggleKey();
        keybd = new imageKey();

        /* Set Names */
        noClick->setKeyID("No Click");
        singleClick->setKeyID("Single Click");
        doubleClick->setKeyID("Double Click");
        keybd->setKeyID("Keyboard");

        /* Font Size */
        noClick->setFontSize(12);
        singleClick->setFontSize(12);
        doubleClick->setFontSize(12);
        keybd->setFontSize(12);

        /* Registered (Key-Pressed) Colors */
        noClick->setRegisteredColor(0, 0, 122);
        singleClick->setRegisteredColor(0, 0, 122);
        doubleClick->setRegisteredColor(0, 0, 122);
        keybd->setRegisteredColor(0, 0, 122);

        /* Delete Images */
        delete keyBackground;
        delete keyHighlight;
        delete keyRegistered;
        delete keyNoClick;
        delete keySingleClick;
        delete keyDoubleClick;
        delete keyKeybd;
        keyBackground = keyHighlight = keyRegistered = keyNoClick = keySingleClick = keyDoubleClick = keyKeybd = NULL;
    }
    catch(...)
    {
        MessageBox(NULL, "Caught exception of unknown type", "Error!", MB_OK | MB_ICONERROR);
    }
}

void globalMouse::deleteButtons()
{
    ///////////////////////////////////////////////////////////////////
    // Delete Images
    ///////////////////////////////////////////////////////////////////
    delete keyBackground;
    delete keyHighlight;
    delete keyRegistered;
    delete keyNoClick;
    delete keySingleClick;
    delete keyDoubleClick;
    delete keyKeybd;
    keyBackground = keyHighlight = keyRegistered = keyNoClick = keySingleClick = keyDoubleClick = keyKeybd = NULL;

    ///////////////////////////////////////////////////////////////////
    // Delete Buttons
    ///////////////////////////////////////////////////////////////////
    delete singleClick;
    delete doubleClick;
    delete noClick;
    delete keybd;
    singleClick = doubleClick = noClick = NULL;
    keybd = NULL;
}

void globalMouse::update(int cursorX, int cursorY)
{
    ///////////////////////////////////////////////////////////////////
    // Update mouse
    ///////////////////////////////////////////////////////////////////
    mouse.update();
    if(!mouseByRef)
        mouse.updateXY(cursorX, cursorY);

    ///////////////////////////////////////////////////////////////////
    // Update keyboard if active
    ///////////////////////////////////////////////////////////////////
    if(keybdActive) {
        /* Assumes calibration is done in fullscreen mode, which it should be */
        POINT cursorPt;
        GetCursorPos(&cursorPt);
        ScreenToClient(winmanGetActiveWindowHandle(), &cursorPt);
        KM->update(cursorPt.x, cursorPt.y);

        /* Swap back to mouse mode if needed */
        if(KM->getReturnState()) {
            keybdActive = false;
            resizeAndPosition(windowWidth, windowHeight);
        }
    }
    else
        updateNoKeybd();
}

/**
 * Update function called when keyboard not active
 */
void globalMouse::updateNoKeybd()
{
    ///////////////////////////////////////////////////////////////////
    // Get window client rect
    ///////////////////////////////////////////////////////////////////
    HWND hWnd = winmanGetActiveWindowHandle();

    RECT clientRect;
    GetClientRect(hWnd, &clientRect);

    /* This works because a rect is the equivalent (in memory) of two consecutive points
     * typedef struct tagRECT {LONG left; LONG top; LONG right; LONG bottom;} RECT */
    ClientToScreen(hWnd, (LPPOINT)&clientRect.left); //left and top
    ClientToScreen(hWnd, (LPPOINT)&clientRect.right); //right and bottom

    ///////////////////////////////////////////////////////////////////
    // Get cusor position
    ///////////////////////////////////////////////////////////////////
    POINT cursorPos;
    GetCursorPos(&cursorPos);

    ///////////////////////////////////////////////////////////////////
    // Activate no click if inside window.
    // Restore previous click functionality if cursor leaves window.
    ///////////////////////////////////////////////////////////////////
    if(cursorPos.x >= clientRect.left && cursorPos.x <= clientRect.right &&
       cursorPos.y >= clientRect.top  && cursorPos.y <= clientRect.bottom)
       {
            if(!clickTypeSet)
            {
                clickType = mouse.getClickType();
                clickTypeSet = true;
                mouse.setClickType(SM_NOCLICK);
            }
       }
    else if(clickTypeSet)
    {
        mouse.setClickType(clickType);
        clickTypeSet = false;
    }

    ///////////////////////////////////////////////////////////////////
    // Get the outputs of the buttons
    ///////////////////////////////////////////////////////////////////
     if(noClick->getKeyOut() && clickType != SM_NOCLICK)
    {
        clickType = SM_NOCLICK;
        clickTypeSet = true;
    }
    else if(singleClick->getKeyOut() && clickType != SM_SINGLECLICK)
    {
        clickType = SM_SINGLECLICK;
        clickTypeSet = true;
    }
    else if(doubleClick->getKeyOut() && clickType != SM_DOUBLECLICK)
    {
        clickType = SM_DOUBLECLICK;
        clickTypeSet = true;
    }
    else if(keybd->getKeyOut())
    {
        keybdActive = true;
        winmanReshapeWindow(oldWinRect.right - oldWinRect.left, oldWinRect.bottom - oldWinRect.top);
        winmanPositionWindow(oldWinRect.left, oldWinRect.top);
        //clickType = SM_NOCLICK;
        //clickTypeSet = true;
    }

    ///////////////////////////////////////////////////////////////////
    // Set button toggles according to the mouse click type
    ///////////////////////////////////////////////////////////////////
    switch(clickType)
    {
        case SM_NOCLICK:
            noClick->setKeyOut(true);
            singleClick->setKeyOut(false);
            doubleClick->setKeyOut(false);
            break;
        case SM_SINGLECLICK:
            noClick->setKeyOut(false);
            singleClick->setKeyOut(true);
            doubleClick->setKeyOut(false);
            break;
        case SM_DOUBLECLICK:
            noClick->setKeyOut(false);
            singleClick->setKeyOut(false);
            doubleClick->setKeyOut(true);
            break;
    }

    ///////////////////////////////////////////////////////////////////
    // Update the cursor position for buttons
    ///////////////////////////////////////////////////////////////////
    ScreenToClient(hWnd, &cursorPos);
    noClick->update(cursorPos.x, cursorPos.y);
    singleClick->update(cursorPos.x, cursorPos.y);
    doubleClick->update(cursorPos.x, cursorPos.y);
    keybd->update(cursorPos.x, cursorPos.y);
}
void globalMouse::draw()
{
    overlay->drawImage(0, 0);
    if(keybdActive)
        KM->draw();
    else
        drawButtons();
}

void globalMouse::setWindowStyles(DWORD style, DWORD exStyle)
{
    HWND hWnd = winmanGetActiveWindowHandle();

    ///////////////////////////////////////////////////////////////////
    // Get styles so they can be restored later
    ///////////////////////////////////////////////////////////////////
    DWORD tempPrevStyle = GetWindowLongPtr(hWnd, GWL_STYLE);
    DWORD tempPrevExStyle = GetWindowLongPtr(hWnd,GWL_EXSTYLE);

    ///////////////////////////////////////////////////////////////////
    // Set the new required styles.
    // Also, ensure the new style isn't the same as the old style.
    // This prevents accidently overwriting a previous style.
    ///////////////////////////////////////////////////////////////////
    if(tempPrevStyle != style)
    {
        prevStyle = tempPrevStyle;
        SetWindowLongPtr(hWnd, GWL_STYLE, style);
    }
    if(tempPrevExStyle != exStyle)
    {
        prevExStyle = tempPrevExStyle;
        LONG_PTR lp = SetWindowLongPtr(hWnd, GWL_EXSTYLE, exStyle);
        if(lp==0)
        {
            MessageBox(NULL, "Failed to set extended window style", "Error", MB_OK | MB_ICONEXCLAMATION);
        }
    }
    ShowWindow(hWnd, SW_SHOW);
}

void globalMouse::setWindowStyle(DWORD style)
{
    HWND hWnd = winmanGetActiveWindowHandle();
    setWindowStyles(style, GetWindowLongPtr(hWnd, GWL_EXSTYLE));
}

void globalMouse::setWindowExStyle(DWORD exStyle)
{
    HWND hWnd = winmanGetActiveWindowHandle();
    setWindowStyles(GetWindowLongPtr(hWnd, GWL_STYLE), exStyle);
}

void globalMouse::restorePrevWindowStyles()
{
    setWindowStyles(prevStyle, prevExStyle);
}

void globalMouse::resizeAndPosition(int width, int height)
{
    //resize
    winmanReshapeWindow(width, height);

    //Get size of desktop, excluding taskbar
    RECT workArea;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);

    HWND hWnd = winmanGetActiveWindowHandle();
    RECT winRect;
    GetWindowRect(hWnd, &winRect);

    width = winRect.right - winRect.left;
    height = winRect.bottom - winRect.top;

    //position at bottom right of screen
    winmanPositionWindow(workArea.right - width, workArea.bottom - height);
}

void globalMouse::drawButtons()
{
    int w = ofGetWidth() / 4 - 50.0/4.0; //5 gaps of 10px, divided over 4 keys
    int h = w;
    int r = 5;
    int y = ofGetHeight() / 2;

    int x = 10 + w / 2;

    /* coordinates were originally for RECTMODE_CENTER */
    x -= w / 2;
    y -= h / 2;
    noClick->setDimensions(x, y, w, h, r);
    noClick->drawKey();

    x += 10 + w;
    singleClick->setDimensions(x, y, w, h, r);
    singleClick->drawKey();

    x += 10 + w;
    doubleClick->setDimensions(x, y, w, h, r);
    doubleClick->drawKey();

    x += 10 + w;
    keybd->setDimensions(x, y, w, h, r);
    keybd->drawKey();
}
