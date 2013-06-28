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

#pragma once
#include "ofConstants.h"
#include "ofAppBaseWindow.h"
#include "winman.h"

class ofPoint;
class ofBaseApp;

namespace ofxWin32 {
    class ofAppWin32Window : public ofAppBaseWindow {

    public:

        ofAppWin32Window();
        ~ofAppWin32Window(){}

        void setupOpenGL(int w, int h, int screenMode);
        void initializeWindow();
        void runAppViaInfiniteLoop(ofBaseApp * appPtr);

        void hideCursor();
        void showCursor();

        void setFullscreen(bool fullScreen);
        void toggleFullscreen();

        static void exitApp();

        void setWindowTitle(string title);
        void setWindowPosition(int x, int y);
        void setWindowShape(int w, int h);

        ofPoint		getWindowPosition();
        ofPoint		getWindowSize();
        ofPoint		getScreenSize();

        int			getWindowMode();

        int			getFrameNum();
        float		getFrameRate();
        void		setFrameRate(float targetRate);

        void		enableSetupScreen();
        void		disableSetupScreen();

        static void display(void);
        static void mouse_cb(int button, int state, int x, int y);
        static void motion_cb(int x, int y);
        static void passive_motion_cb(int x, int y);
        static void idle_cb(void);
        static void keyboard_cb(unsigned char key, int x, int y);
        static void keyboard_up_cb(unsigned char key, int x, int y);
        static void special_key_cb(int key, int x, int y) ;
        static void special_key_up_cb(int key, int x, int y) ;
        static void resize_cb(int w, int h);




    };
}
