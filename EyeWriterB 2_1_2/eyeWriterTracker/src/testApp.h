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

#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"

#include "trackingManager.h"
#include "calibrationManager.h"
#include "buttonTrigger.h"
#include "offsetTraining.h"
#include "typingScene.h"

#include "pongScene.h"
#include "keyboardManager.h"

#include "eyePlotterTestApp.h"
#include "globalMouse.h"

#include <windows.h>


enum{

	MODE_TRACKING,
	MODE_CALIBRATING,
	MODE_TEST,
	MODE_OFFSET,
	MODE_DRAW,
	MODE_KEYBOARD,
	//MODE_TYPING,
	//MODE_PONG,
	NUMBER_OF_MODES

};

class testApp : public ofBaseApp {

	public:

		testApp();
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void resized(int w, int h);

		//----- scenes

		trackingManager			TM;
		calibrationManager		CM;

		buttonTrigger			BT;
		offsetTraining          OT;
		eyePlotterTestApp	    eyeApp;
		typingScene				typeScene;
		pongScene				ponger;
		keyboardManager         KM;

        globalMouse             GM;

		ofPoint eyeSmoothed;
		ofPoint screenPoint;
		float waitFpsTime;
		float timeSince;

		int mode;

		bool bMouseSimulation;
		bool bMouseEyeInputSimulation;

		//Global Mouse
        int prevWindowMode;
		bool windowModeSet;
		bool useEyeAsCursor;
		DWORD normStyle;
		DWORD normExStyle;

		//------ drawing
		void drawHelp();

		static const float rotSmooth = .9;


};

#endif
