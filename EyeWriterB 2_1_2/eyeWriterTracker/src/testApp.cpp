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

#include "testApp.h"
#include "stdio.h"
#include "winman.h"

//#define USE_MOUSE
#define USE_EYE
#if (not defined USE_MOUSE) && (not defined USE_EYE)
    #error Define either USE_MOUSE or USE_EYE above
#endif

//--------------------------------------------------------------
testApp::testApp() :
#ifdef USE_MOUSE
    GM(mouseX, mouseY, &KM)
#elif defined USE_EYE
    GM(&KM)
#else
    #error Define either USE_MOUSE or USE_EYE at top of file
#endif
{

}

//--------------------------------------------------------------
void testApp::setup(){
    winmanSetWindowTitle("EyeWriterB 2.1.2");


	//---- setup standard application settings

	ofSetFrameRate(60);

	mode = MODE_TRACKING;
	TM.setup();
	CM.setup();
	OT.setup();
	typeScene.setup();
	eyeSmoothed.set(0,0,0);

	eyeApp.setup();

	ponger.setup();



	BT.setup("catch me!", 50,50,180,180);

	timeSince = 0;
	bMouseSimulation = false;
	bMouseEyeInputSimulation = false;
	useEyeAsCursor = false;
}


//--------------------------------------------------------------
void testApp::update(){

//	ofBackground(70,70,70);
	ofBackground(72,72,72);

	// update the tracking manager (and internally, its input manager)
	TM.update();

	// update the calibration manager
	CM.update();

	// record some points if we are in auto mode
	if (CM.bAutomatic == true && CM.bAmInAutodrive == true && CM.bInAutoRecording){

		if (TM.bGotAnEyeThisFrame()){
			ofPoint trackedEye = TM.getEyePoint();
			CM.fitter.registerCalibrationInput(trackedEye.x,trackedEye.y);
			cout << "Got one at: " << CM.fitter.currentPoint << endl;
			CM.inputEnergy = 1;
		}
	}
	/* <MOD> */
//	if (CM.bAutomatic && CM.bAmInAutodrive && CM.bInAutoPostRecording) {
//	    //if we didn't get any points this time. Need to redo the point
//	    if(CM.fitter.nPtsInPoints[CM.fitter.currentPoint] == 0) {
//            CM.startTime += CM.totalTimePerDot - CM.postTimePerDot;
//            //TODO: Create a message saying "Eye not tracked" or equivalent.
//            cout << "Could not calibrate eye at this position. Trying again." << endl;
//	    }
//	}
	/* </MOD> */

	if (!bMouseSimulation){
	// smooth eye data in...
		if (CM.fitter.bBeenFit){

			ofPoint trackedEye;

			if (bMouseEyeInputSimulation) {
				trackedEye.x = mouseX;
				trackedEye.y = mouseY;
			} else {
				trackedEye = TM.getEyePoint();
			}

			screenPoint = CM.fitter.getCalibratedPoint(trackedEye.x, trackedEye.y);
			eyeSmoothed.x = CM.smoothing * eyeSmoothed.x + (1-CM.smoothing) * screenPoint.x;
			eyeSmoothed.y = CM.smoothing * eyeSmoothed.y + (1-CM.smoothing) * screenPoint.y;
		}

	} else {

		eyeSmoothed.x = mouseX;
		eyeSmoothed.y = mouseY;
	}
#ifdef USE_MOUSE
    if(useEyeAsCursor)
        GM.update(mouseX, mouseY);
#elif defined USE_EYE
    if(useEyeAsCursor)
        GM.update(eyeSmoothed.x, eyeSmoothed.y);
#endif

    if(mode == MODE_TRACKING) {
	    key::setKeyPressTime(TM.panel.getValueF("KEY_PRESS_TIME")*1000);
    }
	if (mode == MODE_TEST){
		ofPoint pt = eyeSmoothed;
		if (BT.update(pt.x, pt.y)){
			BT.x = ofRandom(100,ofGetWidth()-100);
			BT.y = ofRandom(100,ofGetHeight()-100);
		}
	}

	if(mode == MODE_OFFSET) {
#ifdef USE_MOUSE
        OT.update(mouseX, mouseY);
#elif defined USE_EYE
        OT.update(eyeSmoothed.x, eyeSmoothed.y);
#endif
	}

	if( mode == MODE_DRAW ){
		ofPoint pt = eyeSmoothed;
		//if( ofGetElapsedTimef() - timeSince >= 1.0/8.0 ){

            #ifdef USE_MOUSE
			eyeApp.update( mouseX, mouseY );
			#elif defined USE_EYE
			eyeApp.update( pt.x, pt.y);
            #endif

		//	timeSince = ofGetElapsedTimef();
		//}
	}

	if(mode == MODE_KEYBOARD) {
	    ofPoint pt = eyeSmoothed;
	    #ifdef USE_MOUSE
	    KM.update(mouseX, mouseY);
	    #elif defined USE_EYE
	    KM.update(pt.x, pt.y);
        #endif
	}
//	if (mode == MODE_TYPING){
//		ofPoint pt = eyeSmoothed;
//		typeScene.update(pt.x, pt.y);
//	}

//	if (mode == MODE_PONG){
//		ofPoint pt = eyeSmoothed;
//		ponger.update(pt.x, pt.y);
//	}

}

//--------------------------------------------------------------
void testApp::draw(){
    if(useEyeAsCursor)
    {
        ofEnableAlphaBlending();
        GM.setOverlayImage(TM.getOverlayImage());
        GM.draw();
        ofDisableAlphaBlending();
    }
    else
    {
        ofSetColor(255, 255, 255);

        if (mode == MODE_TRACKING)			TM.draw();
        if (mode == MODE_CALIBRATING)		CM.draw();
        if (mode == MODE_TEST)				BT.draw();
        if (mode == MODE_OFFSET)            OT.draw();
        if (mode == MODE_DRAW )				eyeApp.draw();
        if (mode == MODE_KEYBOARD) {
            ofEnableAlphaBlending();
            KM.draw();
            ofDisableAlphaBlending();
        }
//        if (mode == MODE_TYPING)			typeScene.draw();
//        if (mode == MODE_PONG)				ponger.draw();

        // draw a green dot to see how good the tracking is:
        if ((CM.fitter.bBeenFit && mode != MODE_OFFSET) || bMouseSimulation){
            ofSetColor(0,255,0,120);
            ofFill();
            ofCircle(eyeSmoothed.x, eyeSmoothed.y, 20);
        }

        if (TM.IM.bRecord) {
            ofSetColor(255, 0, 0);
            ofFill();
            ofRect(0, ofGetHeight()-10, 10, 10);
        }

        if (mode != MODE_TRACKING) {
            ofEnableAlphaBlending();
            ofSetColor(255, 255, 255, 120);
            TM.drawInput(0, 0, TM.IM.width/4, TM.IM.height/4, TM.IM.width/4, 0, TM.IM.width/4, TM.IM.height/4);
            ofDisableAlphaBlending();
        }

        ofSetColor(255, 255, 255);
        ofDrawBitmapString("FrameRate: " + ofToString(ofGetFrameRate(), 5), 1, ofGetHeight() - 20);
    }
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){


	switch (key){
        case    '1':
            if(0 < NUMBER_OF_MODES && mode != -1)
                mode = 0; //modes start at 0, but mode 1 should be first screen
            break;
        case    '2':
            if(1 < NUMBER_OF_MODES && mode != -1)
                mode = 1;
            break;
        case    '3':
            if(2 < NUMBER_OF_MODES && mode != -1)
                mode = 2;
            break;
        case    '4':
            if(3 < NUMBER_OF_MODES && mode != -1)
                mode = 3;
            break;
        case    '5':
            if(4 < NUMBER_OF_MODES && mode != -1)
                mode = 4;
            break;
        case    '6':
            if(5 < NUMBER_OF_MODES && mode != -1)
                mode = 5;
            break;
        case    '7':
            if(6 < NUMBER_OF_MODES && mode != -1)
                mode = 6;
            break;
        case    '8':
            if(7 < NUMBER_OF_MODES && mode != -1)
                mode = 7;
            break;
        case    '9':
            if(8 < NUMBER_OF_MODES && mode != -1)
                mode = 8;
            break;
        case    '0':
            if(9 < NUMBER_OF_MODES && mode != -1)
                mode = 9;
            break;

		case	OF_KEY_RETURN:
			mode ++;
			mode %= NUMBER_OF_MODES; // number of modes;
			break;


		case	'm':
		case	'M':
			bMouseSimulation = !bMouseSimulation;
			break;

		case	'r':
		case	'R':
			if (TM.IM.bRecord) TM.IM.stopRecord();
			else TM.IM.startRecord();
			break;

		case	'n':
		case	'N':
			bMouseEyeInputSimulation = !bMouseEyeInputSimulation;
			break;


		case	'f':
		case	'F':
			ofToggleFullscreen();
			break;

		case	'c':
		case	'C':
			TM.setOriginalPosition();
			break;

		case 'x':
		case 'X':
			TM.bOriginalPositon = false;
			break;

        case    's':
        case    'S':
            useEyeAsCursor ^= 1; //if true, set false; if false, set true
            if(useEyeAsCursor)
            {
                GM.activate();
                GM.setOverlayOpacity(TM.panel.getValueI("OVERLAY_OPACITY"));
                GM.setClickTime(TM.panel.getValueF("CLICK_TIME")*1000);
                mode = -1;
            }
            else
            {
                GM.deactivate();
                mode = 0;
            }
            break;


	}

	if (mode == MODE_CALIBRATING){
		CM.keyPressed(key);
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

	if (mode == MODE_TRACKING)			TM.mouseDragged(x, y, button);
	if (mode == MODE_CALIBRATING)		CM.mouseDragged(x, y, button);
	if (mode == MODE_OFFSET)           OT.mouseDragged(x, y, button);

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){


	if (mode == MODE_TRACKING)			TM.mousePressed(x, y, button);
	if (mode == MODE_CALIBRATING)		CM.mousePressed(x, y, button);
	if (mode == MODE_OFFSET)            OT.mousePressed(x, y, button);


}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

	if (mode == MODE_TRACKING)			TM.mouseReleased();
	if (mode == MODE_CALIBRATING)		CM.mouseReleased(x,y,button);
	if (mode == MODE_OFFSET)            OT.mouseReleased();

}

//--------------------------------------------------------------
void testApp::resized(int w, int h){

}
