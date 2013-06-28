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

#include "simMouse.h"
#include "ofMain.h"

simMouse::simMouse(int& _x, int& _y) : x(_x), y(_y), hCurRestore(NULL)
{
    Init();
}

simMouse::simMouse() : x(manualX), y(manualY), hCurRestore(NULL)
{
    Init();
}

simMouse::~simMouse()
{
    restoreCursor();
}

void simMouse::Init()
{
    clickActive = false;
    clickTime = 1500;
    baseMillis  = 0;

    memset(&mouse, 0, sizeof(mouse));
    mouse.type = INPUT_MOUSE;

    if(hCurRestore)
        return; //Don't set the restore cursor if one already exits
    hCurRestore = LoadCursor(NULL, IDC_ARROW);
    hCurRestoreCopy = CopyCursor(hCurRestore);
    if(hCurRestoreCopy == NULL) {
        MessageBox(NULL,    "Could not set the Restore Cursor. When you are done \n"
                   "you must manually restore the cursor in the control panel.", "Error", MB_OK | MB_ICONEXCLAMATION);
    }
}

void simMouse::updateXY(int _x, int _y) //use this only to manually change x and y
{
    manualX = _x;
    manualY = _y;
    x = manualX;
    y = manualY;
}

void simMouse::updateRefXY(int& _x, int& _y)
{
    x = _x;
    y = _y;
}

void simMouse::setClickType(int _clickType)
{
    clickType = _clickType;
    if(clickType == SM_NOCLICK)
        restoreCursor();
    else
        setCursor();
}

void simMouse::setClickTime(int _clickTime)
{
    clickTime = _clickTime;
}

bool simMouse::setCursor()
{
    baseMillis = ofGetElapsedTimeMillis(); //reset the time
    HCURSOR hCur = LoadCursorFromFile(NORMAL_CURSOR_FILE);
    if(hCur == NULL) {
        if(MessageBox(NULL, "Could not load animated cursor. Would you like to continue\n"
                      "and use the regular cursor instead?", "Error", MB_YESNO | MB_ICONEXCLAMATION) == IDNO) {
            return false;
        }
    }
    if(!SetSystemCursor(hCur, OCR_NORMAL))
        return false;

    return true;
}

bool simMouse::restoreCursor()
{
    if(!hCurRestoreCopy)
        return false;
    if(!SetSystemCursor(hCurRestoreCopy, OCR_NORMAL))
        return false;
    if(!(hCurRestoreCopy = CopyCursor(hCurRestore)))
        return false;
    return true;
}

void simMouse::update()
{
    SetCursorPos(x, y);
    handleClick();
}

void simMouse::handleClick()
{
    if(clickType == SM_NOCLICK) {
        return;
    }

    ///////////////////////////////////////////////////////////////////
    // Create an invisible circle, currently radius 30 px.
    // If cursor remains in this circle for a set amount of time,
    // activate click.
    ///////////////////////////////////////////////////////////////////

    /*
     * It seems to be better to get the cursor position
     * from window rather than from OpenFrameworks
     */
    static POINT oldCurPos = {-100, -100};
    static POINT curPos = {0};
    GetCursorPos(&curPos);

    if(pow(curPos.x-oldCurPos.x, 2) + pow(curPos.y-oldCurPos.y, 2) > 30*30) {
        ///////////////////////////////////////////////////////////////////
        // Check if cursor exits circle
        ///////////////////////////////////////////////////////////////////

        /* Reset data */
        oldCurPos.x = curPos.x;
        oldCurPos.y = curPos.y;
        baseMillis = ofGetElapsedTimeMillis();

        /* reset the cursor animation */
        /* there's probably a better way to do this */
        HCURSOR hCur = LoadCursorFromFile(NORMAL_CURSOR_FILE);
        if(hCur)
            SetSystemCursor(hCur, OCR_NORMAL);
    } else if (ofGetElapsedTimeMillis() - baseMillis > clickTime) {
        ///////////////////////////////////////////////////////////////////
        // Check if cursor has remained in circle for set period of time
        ///////////////////////////////////////////////////////////////////
        click();
        if(clickType == SM_DOUBLECLICK)
            click();
        baseMillis = ofGetElapsedTimeMillis();
        HCURSOR hCur = LoadCursorFromFile(NORMAL_CURSOR_FILE);
        if(hCur)
            SetSystemCursor(hCur, OCR_NORMAL);
    }

}

void simMouse::click()
{
    ///////////////////////////////////////////////////////////////////
    // Click up and down
    ///////////////////////////////////////////////////////////////////
    mouse.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(1, &mouse, sizeof(INPUT));
    mouse.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, &mouse, sizeof(INPUT));
}
