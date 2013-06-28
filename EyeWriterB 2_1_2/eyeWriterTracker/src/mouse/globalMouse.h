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

#ifndef GLOBALMOUSE_H_
#define GLOBALMOUSE_H_

#include <windows.h>
#include "ofMain.h"
#include "simMouse.h"
#include "buttonRect.h"
#include "imageToggleKey.h"
#include "imageKey.h"
#include "keyboardManager.h"
#include "cameraOverlay.h"

//TODO: Move many of the windows specific functions to winman to make mac compatible
class globalMouse
{
public:
    globalMouse(int& x, int& y, keyboardManager *keybdMan, IplImage *cameraImg = NULL);
    globalMouse(keyboardManager *keybdMan, IplImage *cameraImg = NULL);
    ~globalMouse();

    /**
     * Activates Global Mouse mode. Resizes window, sets window styles, and allocates memory
     * to activate the Global Mouse.
     */
    void activate();

    /**
     * Deactivates Global Mouse mode. Restores window size, restores window styles, and frees
     * memory no longer needed for Global Mouse.
     */
    void deactivate();

    /**
     * Standard openFrameworks draw function. Draws contents inside the global mouse.
     */
    void draw();

    /**
     * Standard openFrameworks update function. Updates variables and other properties
     * of the globalMouse.
     * @param cursorX The x position of the cursor if cursor position was not passed by
     * reference in the constructor. Passing a value for cursorX if cursor position not
     * passed by reference in the constructor has no effect.
     * @param cursorY The y position of the cursor if cursor position was not passed by
     * reference in the constructor. Passing a value for cursorY if cursor position not
     * passed by reference in the constructor has no effect.
     */
    void update(int cursorX = 0, int cursorY = 0);

    /**
     * Set the overlay image for camera overlay class.
     * @param *cameraImg The a pointer to the overlay image to use.
     */
    void setOverlayImage(IplImage *cameraImg);

    /**
     * Set the opacity of the overlay image.
     * @param opacity A value between 0 (fully transparent) and 255 (fully opaque).
     */
    void setOverlayOpacity(int opacity);

    /**
     */
     void setClickTime(int clickTime);

private:
    void init();

    ///////////////////////////////////////////////////////////////////
    // Window Functions
    ///////////////////////////////////////////////////////////////////
    void resizeAndPosition(int width, int height);
    void setWindowStyles(DWORD style, DWORD exStyle);
    void setWindowStyle(DWORD style);
    void setWindowExStyle(DWORD exStyle);
    void restorePrevWindowStyles(); //note, this only restore the most recent window style

    void updateNoKeybd();

    ///////////////////////////////////////////////////////////////////
    // Button Functions
    ///////////////////////////////////////////////////////////////////
    void loadButtonImages();
    void drawButtons();
    void trackButtons();
    void deleteButtons(); //this content can be updated more easily this way

    ///////////////////////////////////////////////////////////////////
    // Overlay Components
    ///////////////////////////////////////////////////////////////////
    cameraOverlay *overlay;
    IplImage *cameraImage;

    ///////////////////////////////////////////////////////////////////
    // Mouse Components
    ///////////////////////////////////////////////////////////////////
    simMouse mouse;
    bool mouseByRef;
    bool active;

    ///////////////////////////////////////////////////////////////////
    // Keyboard Components
    ///////////////////////////////////////////////////////////////////
    ofImage *keyBackground, *keyHighlight, *keyRegistered,
            *keyNoClick, *keySingleClick, *keyDoubleClick, *keyKeybd;
    imageToggleKey *noClick, *singleClick, *doubleClick;
    imageKey *keybd;
    keyboardManager *KM;
    bool keybdActive;

    ///////////////////////////////////////////////////////////////////
    // Window Properties
    ///////////////////////////////////////////////////////////////////
    const int windowWidth, windowHeight;
    RECT oldWinRect;
    DWORD prevStyle;
    DWORD prevExStyle;
    int clickType; //stores the click type OUTSIDE the window. The click type inside the window is always single click
    bool clickTypeSet; //has click type been set?
};

#endif
