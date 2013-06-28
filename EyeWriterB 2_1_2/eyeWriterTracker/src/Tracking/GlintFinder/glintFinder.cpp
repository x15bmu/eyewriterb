#include "glintFinder.h"
//--------------------------------------------------------------------
void glintFinder::setup(int eyeWidth, int eyeHeight, float _magRatio, float IMwidth, float IMheight){

	magRatio = _magRatio;

	w = eyeWidth;		// before magnified
	h = eyeHeight;		// before magnified

	inputWidth = IMwidth;
	inputHeight = IMheight;

	eyeImage.allocate(w * magRatio, h * magRatio);
	checkBrightEye.allocate(w * magRatio, h * magRatio);
	//	bIsVerticalLED = false;
	bFourGlints = false;					// 2 or 4

	glintROI.x = 0;
	glintROI.y = 0;
	glintROI.width = w * magRatio;
	glintROI.height = h * magRatio;

	pctGlintROIorigin.x = 0;								// percentage (0.0 - 1.0)
	pctGlintROIorigin.y = 0;
	pctGlintROIend.x = 1;
	pctGlintROIend.y = 1;

	bUseContrastStretch = false;							// overridden by controlpanel.
	bFirstRecFrame = true;
	bRecordRef = false;
	bUseRecordRef = false;

	maxDistancePct = 1.2;
	minDistancePct = 0.8;

	gLineChecker.setup(w * magRatio, h * magRatio);				// checking glint candidates.

}

//--------------------------------------------------------------------
/**
 * Update the pupil shift (i.e. how far the pupil has shifted. Good for estimating glint position if no glint detected
 * @param shift The shift since the last frame.
 */
void glintFinder::updatePupilShift(ofPoint shift)
{
    pupilShift += shift;

    //The glints move the opposite way of the pupil.
    //If the pupil moves up, glints move down with
	//respect to the pupil.
    glintPos[GLINT_BOTTOM_LEFT] -= shift;
    glintPos[GLINT_BOTTOM_RIGHT] -= shift;
}
//--------------------------------------------------------------------
/**
 * Reset the pupil shift. Call when the glint has been detected again.
 */
void glintFinder::resetPupilShift()
{
    pupilShift.x = 0;
    pupilShift.y = 0;
}

//--------------------------------------------------------------------
/**
 * Get the amount the pupil has shifted since the last reset (pupil is reset
 * when resetPupilShift() is called). This is NOT the same value as the input
 * shift for updatePupilShift(ofPoint shift). It is an accumulation of all the
 * total shifts since reset.
 */
ofPoint glintFinder::getNetPupilShift()
{
    return pupilShift;
}

//--------------------------------------------------------------------
bool glintFinder::update(ofxCvGrayscaleAdvanced & blackEyeImg, float threshold, float minBlobSize, float maxBlobSize, bool bUseBrightEyeCheck) {

	bFound = false;
	bTwoGlintsFound = false;

	glintROI.x = w * magRatio * pctGlintROIorigin.x;				// should ROI be decided by calibration data?(and 4 or 2)
	glintROI.y = h * magRatio * pctGlintROIorigin.y;
	glintROI.width = w * magRatio * ( pctGlintROIend.x - pctGlintROIorigin.x );
	glintROI.height = h * magRatio * ( pctGlintROIend.y - pctGlintROIorigin.y );

//		glintROI.x = 0;
//		glintROI.y = h * magRatio/2 - 30;
//		glintROI.width = w * magRatio;
//		glintROI.height = h * magRatio/2;

	eyeImage = blackEyeImg;
	eyeImage.setROI(glintROI);

	if (bUseContrastStretch) eyeImage.contrastStretch();

	eyeImage.threshold(threshold, false);

	int nGlints;

	if (bFourGlints) nGlints = 4;
	else nGlints = 2;

	contourFinder.findContours(eyeImage, minBlobSize, maxBlobSize, nGlints + 4, true, true);
	eyeImage.resetROI();

	if (nGlints == 2){			// for now
		if (refChecker.getnFrames(false) > 0 && bUseRecordRef){		// if (!bUseRecordRef), control panel changes min&max distance values;
			if (refChecker.bNeedCalculate) refChecker.calculateAverages();

			//Measure the average distance between the 2 glints. If too big or too small,
			//the correct glints have not been found. Maybe this isnt what it does.
			//Possibly it actually finds the length of the line...but not sure,
			//more work to do.
			gLineChecker.minDistance = refChecker.glintHis[GLINT_ELEMENT_BOTTOM].avgDistance * minDistancePct;
			gLineChecker.maxDistance = refChecker.glintHis[GLINT_ELEMENT_BOTTOM].avgDistance * maxDistancePct;
		}
		gLineChecker.update(eyeImage, nGlints, contourFinder, bUseBrightEyeCheck, contourFinderBright);	// now ROI doesn't work.
	}

	if (!bFourGlints && (gLineChecker.leftGlintID != -1) && (gLineChecker.rightGlintID != -1)){

		if (refChecker.getnFrames(false) > 0){
			if (!refChecker.checkSize(contourFinder.blobs[gLineChecker.leftGlintID], GLINT_BOTTOM_LEFT, 0, 1.8f)) {
				if (refChecker.bNeedCalculate) refChecker.calculateAverages();

				ofRectangle temp = contourFinder.blobs[gLineChecker.leftGlintID].boundingRect;
				ofPoint vec;
				vec.x = refChecker.glintHis[GLINT_ELEMENT_BOTTOM].avgWidth[GLINT_ELEMENT_LEFT] / 2;
				vec.y = refChecker.glintHis[GLINT_ELEMENT_BOTTOM].avgWidth[GLINT_ELEMENT_LEFT] / 2;

				contourFinder.blobs[gLineChecker.leftGlintID].centroid.x = temp.x + temp.width - vec.x;
				contourFinder.blobs[gLineChecker.leftGlintID].centroid.y = temp.y + vec.y;
			}

			if (!refChecker.checkSize(contourFinder.blobs[gLineChecker.rightGlintID], GLINT_BOTTOM_RIGHT, 0, 1.8f)){
				if (refChecker.bNeedCalculate) refChecker.calculateAverages();

				ofRectangle temp = contourFinder.blobs[gLineChecker.rightGlintID].boundingRect;
				ofPoint vec;
				vec.x = refChecker.glintHis[GLINT_ELEMENT_BOTTOM].avgWidth[GLINT_ELEMENT_RIGHT] / 2;
				vec.y = refChecker.glintHis[GLINT_ELEMENT_BOTTOM].avgWidth[GLINT_ELEMENT_RIGHT] / 2;

				contourFinder.blobs[gLineChecker.rightGlintID].centroid.x = temp.x + vec.x;
				contourFinder.blobs[gLineChecker.rightGlintID].centroid.y = temp.y + vec.y;
			}
		}

		glintPos[GLINT_BOTTOM_LEFT] = contourFinder.blobs[gLineChecker.leftGlintID].centroid / magRatio;
		glintPos[GLINT_BOTTOM_RIGHT] = contourFinder.blobs[gLineChecker.rightGlintID].centroid / magRatio;

		bFound = true;
		bTwoGlintsFound = true;

		if (bRecordRef){
			if (bFirstRecFrame){
				bFirstRecFrame = false;
				refChecker.clear();
			}
			refChecker.addGlints(contourFinder.blobs[gLineChecker.leftGlintID], contourFinder.blobs[gLineChecker.rightGlintID], false);
		} else if (!bRecordRef && !bFirstRecFrame){
			bFirstRecFrame = true;
		}
	} else if(contourFinder.blobs.size() == 1 && contourFinder.blobs[0].area < 1000) {
	    //there is only one glint, not two and its area is not too big (1000
	    //is enormous, far too big for a glint, which is <500) Guess which glint it is (left or right)
	    //and estimate position of other glint based on the position of the found one

	    //compare centroid of found contour to glintPos (which holds the previous glint positions)
        ofPoint centroid = contourFinder.blobs[0].centroid / magRatio;
        float deltaLX = centroid.x - glintPos[GLINT_BOTTOM_LEFT].x;
        float deltaLY = centroid.y - glintPos[GLINT_BOTTOM_LEFT].y;
        float distSquaredLeft = deltaLX*deltaLX + deltaLY*deltaLY;

        float deltaRX = centroid.x - glintPos[GLINT_BOTTOM_RIGHT].x;
        float deltaRY = centroid.y - glintPos[GLINT_BOTTOM_RIGHT].y;
        float distSquaredRight = deltaRX*deltaRX + deltaRY*deltaRY;

        //check to see which previous glint location is closest to the current one
        //also check to see if the glint is on the correct side of the bright eye glint
        //if it isn't, dont assign it
        if(distSquaredLeft < distSquaredRight && centroid.x < lastBrightCentroid.x/magRatio) {
            glintPos[GLINT_BOTTOM_LEFT] = centroid;
            glintPos[GLINT_BOTTOM_RIGHT].x += deltaLX;
            glintPos[GLINT_BOTTOM_RIGHT].y += deltaLY;

            bFound = true;
        } else if(distSquaredRight < distSquaredLeft && centroid.x > lastBrightCentroid.x/magRatio) {
            glintPos[GLINT_BOTTOM_RIGHT] = centroid;
            glintPos[GLINT_BOTTOM_LEFT].x += deltaRX;
            glintPos[GLINT_BOTTOM_LEFT].y += deltaRY;

            bFound = true;
        }
	}

	// check each glint when more glints are found. or make glint tracker?
	// or check x and y coodinates? distance between glints.

//	if (gChecker.horiCombi.size() >= nGlints / 2) {
//
//		if (!bFourGlints){
//			glintPos[GLINT_BOTTOM_LEFT] = gChecker.getGlintCentroid(GLINT_BOTTOM_LEFT) / magRatio;
//			glintPos[GLINT_BOTTOM_RIGHT] = gChecker.getGlintCentroid(GLINT_BOTTOM_RIGHT) / magRatio;
//
//		} else {

//			for (int i = 0; i < nGlints; i++) {
//				ofPoint gp = contourFinder.blobs[i].centroid;
//
//				if (gp.x < averagePoint.x && gp.y < averagePoint.y) glintPos[GLINT_TOP_LEFT] = gp / magRatio;
//				else if (gp.x >= averagePoint.x && gp.y < averagePoint.y) glintPos[GLINT_TOP_RIGHT] = gp / magRatio;
//				else if (gp.x < averagePoint.x && gp.y >= averagePoint.y) glintPos[GLINT_BOTTOM_LEFT] = gp / magRatio;
//				else if (gp.x >= averagePoint.x && gp.y >= averagePoint.y) glintPos[GLINT_BOTTOM_RIGHT] = gp / magRatio;
//			}
//
//		}
//
//		bFound = true;
//	}

	return bFound;

}

//--------------------------------------------------------------------
void glintFinder::findGlintCandidates(ofxCvGrayscaleAdvanced & eyeImg, float _threshold, float minBlobSize, float maxBlobSize, bool isBrightEye){

	if (isBrightEye){
		eyeImg.threshold(_threshold, false);
		contourFinderBright.findContours(eyeImg, minBlobSize, maxBlobSize, 3, true, true);
		if(contourFinderBright.blobs.size() != 0)
            lastBrightCentroid = contourFinderBright.blobs[0].centroid;
		checkBrightEye = eyeImg;
	}
}

//--------------------------------------------------------------------
ofPoint	glintFinder::getGlintPosition(int glintID) {

	if (glintID != GLINT_IN_BRIGHT_EYE) {
		// this is glint positions in targetRect.
		return ofPoint(glintPos[glintID].x + glintROI.x / magRatio , glintPos[glintID].y + glintROI.y / magRatio);
	} else {
		if (contourFinderBright.blobs.size() > 0){
			return contourFinderBright.blobs[0].centroid;	// this returns raw centroid value which means it's not divided by magRatio.
		} else {											// so might ought to be changed.
			return ofPoint(0,0);
		}
	}
}

//--------------------------------------------------------------------
void glintFinder::drawLine(float x, float y, float width, float height, float len = -1) {

	ofPoint tempPos;

	if (len == -1) {
		len = width / 10;
	}

	ofEnableAlphaBlending();

    if(bFound && bTwoGlintsFound)
        ofSetColor(0, 100, 0, 255); //dark green
    else if(bFound)
        ofSetColor(255, 140, 0, 255); //orange
    else
        ofSetColor(178, 34, 34, 255); //firebrick red

	tempPos = getGlintPosition(GLINT_BOTTOM_LEFT); //get the last known glint position
	drawCross(tempPos, x, y, width, height, len);

    if(bFound && bTwoGlintsFound)
        ofSetColor(0, 200, 0, 255); //green
    else if(bFound)
        ofSetColor(255, 215, 0, 255); //gold
    else
        ofSetColor(255, 0, 0, 255); //red

	tempPos = getGlintPosition(GLINT_BOTTOM_RIGHT);
	drawCross(tempPos, x, y, width, height, len);

	if (bFourGlints) {

		ofSetColor(0, 200, 200, 100);
		//tempPos = getGlintPosition(GLINT_TOP_LEFT);
		drawCross(tempPos, x, y, width, height, len);

		ofSetColor(200, 200, 0, 100);
		tempPos = getGlintPosition(GLINT_TOP_RIGHT);
		drawCross(tempPos, x, y, width, height, len);
	}

	ofDisableAlphaBlending();
}

//--------------------------------------------------------------------
void glintFinder::drawLineOnBrightGlint(float x, float y, float width, float height, float len){

	ofPoint tempPos;

	if (len == -1) {
		len = width / 10;
	}

	ofEnableAlphaBlending();

	ofSetColor(255, 255, 255, 255);
	tempPos = getGlintPosition(GLINT_IN_BRIGHT_EYE) / magRatio;
//	contourFinderBright.draw(x,y);
	drawCross(tempPos, x, y, width, height, len);

	ofDisableAlphaBlending();
}

//--------------------------------------------------------------------
void glintFinder::drawCross(ofPoint & pos, float x, float y, float width, float height, float len) {

	ofLine(x + (pos.x * width/w), y + (pos.y * height/h) - len, x + (pos.x * width/w), y + (pos.y * height/h) + len);
	ofLine(x + (pos.x * width/w) - len, y + (pos.y * height/h), x + (pos.x * width/w) + len, y + (pos.y * height/h));
}

//--------------------------------------------------------------------
void glintFinder::draw(float x, float y) {
    gLineChecker.myStripesImage.draw(x, y);
    //TODO: Remove
    y=0;
	// ofTranslate didn't work well easily because of glScissor.

	ofEnableAlphaBlending();

	ofSetColor(255,255,255);
	eyeImage.draw(x, y);
	contourFinder.draw(x + glintROI.x, y + glintROI.y); //draws glint roi box and contour

	float	tempWidth = w * magRatio;
	float	tempHeight = h * magRatio;

	glScissor(x, ofGetHeight() - (y + tempHeight), tempWidth, tempHeight);			// top & bottom are oppositte.
	glEnable(GL_SCISSOR_TEST);
	gLineChecker.draw(x, y); //draws large rectangle between 2 glints
	drawLine(x, y, tempWidth, tempHeight); //quite a misnamed function. actually handles the drawing of the cross on the glints
	contourFinderBright.draw(x, y); //draws pupil contour and roi box
	glDisable(GL_SCISSOR_TEST);

	ofDisableAlphaBlending();

	ofSetColor(255, 255, 255);

	string accuracy;
	if(bFound && bTwoGlintsFound)
        accuracy = "Good";
    else if(bFound)
        accuracy = "Fair";
    else
        accuracy = "Poor";
	ofDrawBitmapString("glintFinder    Tracking Accuracy: " + accuracy, x + 1, y + eyeImage.height + 12);

}

//--------------------------------------------------------------------

