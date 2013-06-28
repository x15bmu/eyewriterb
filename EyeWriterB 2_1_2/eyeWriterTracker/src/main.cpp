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

#include "ofMain.h"
#include "testApp.h"
#include "ofAppWin32Window.h"
#include "test.h"

//========================================================================

/**
 * @mainpage
 * The EyeWriterB is an eye-tracking system designed to allow a disabled person
 * to use a computer. It makes use of a basic webcam (a modified PlayStation3 camera)
 * and 2 banks of LEDs to track a person's eye and translate a person's gaze direction
 * to a set of screen coordinates, forming the backend of the software. At the front end,
 * the software provides mouse and keyboard functionality. By looking at a point on the
 * screen for a certain amount of time, a person is able to click the mouse. A person
 * can also use a virtual keyboard to enter text by looking at keys for a certain amount
 * of time.
 *
 * The EyeWriterB is based on a the open-source <a href="http://www.eyewriter.org/">EyeWriter
 * </a>project. It also implements several open-source libraries including <a href=
 * "http://opencv.org/">OpenCV</a>, <a href="http://presage.sourceforge.net/">Presage</a>, and
 * <a href="http://www.openframeworks.cc/">openFrameworks</a>.
 *
 * The EyeWriterB is not intended to be a replacement for a mouse and keyboard. Rather, it is
 * designed to allow people to use a computer when they otherwise would be unable to do so.
 *
 * @author Ben Mattinson
 */
int main( ){
    ofxWin32::ofAppWin32Window window;
	ofSetupOpenGL(&window, 1380,860, OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new testApp());

}
