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

#ifndef SIMMOUSE_H_
#define SIMMOUSE_H_

#include <windows.h>

enum
{
    SM_NOCLICK,
    SM_SINGLECLICK,
    SM_DOUBLECLICK
};

class simMouse
{
    public:
        /**
         * Construct a simMouse object which will require manually passing the x- and
         * y-coordinates of the cursor during the update loop.
         */
        simMouse();

        /**
         * Construct a simMouse object by using reference to the x- and y-coordinates of the
         * mouse cursor.
         * @param x A reference to the variable containing the x-coordinate of the cursor.
         * @param y A reference to the variable containing the y-coordinate of the cursor.
         */
        simMouse(int& _x, int& _y);
        ~simMouse();
        void    setClickType(int _clickType);
        int     getClickType() {return clickType;}
        void    update();
        void    handleClick();
        bool    setCursor();       //sets IDC_ARROW to cursor required for simMouse, returns true if successful
        bool    restoreCursor();   //restores cursor to IDC_ARROW, returns true if successful
        void    updateRefXY(int& _x, int& _y);
        void    updateXY(int _x, int _y);

        /**
         * Set the time that the user must hold the cursor over a location before triggering a click
         * @param _clickTime The time in milliseconds that a user must hold the cursor over an area
         * before triggering a click.
         */
        void    setClickTime(int _clickTime);

    protected:
        void click();
        void Init();

    private:
        INPUT mouse;
        HCURSOR hCurRestore;
        HCURSOR hCurRestoreCopy;

        int manualX;            //for use with manual updating only
        int manualY;            //for use with manual updating only
        int& x;                 //store the xy coordinates
        int& y;                 //store the xy coordinates
        int clickType;
        bool clickActive;
        int clickTime;        //the amount of time until the mouse clicks
        int baseMillis;       //stores the value from which to start the counter in ms
#define NORMAL_CURSOR_FILE "data/aero_link_circle working.ani"
/*
 * implemented as define rather than const char
 * to deal with multiple constructors
 */
};

#endif
