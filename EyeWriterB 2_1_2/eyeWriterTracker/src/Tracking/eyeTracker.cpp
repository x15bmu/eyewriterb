#include "eyeTracker.h"
//--------------------------------------------------------------
eyeTracker::eyeTracker() {

	bFoundOne	= false;
}

//----------------------------------------------------
void eyeTracker::setup(int width, int height) {

	w = width;					// IM.width
	h = height;					// IM.height
	magRatio = 4;				// for accurate detection. width x magRatio, height x magRatio
	divisor = 2;				// for eyeFinder, make the picture smaller to reduce computation

	// TODO: now it can't work with divisor = 1, let's debug this problem.

	printf("VideoWidth, VideoHeight = %d, %d \n", w, h);

	currentImg.allocate(w, h);
	currentEyePoint	= ofPoint(0, 0);

	targetRect.x = 0;
	targetRect.y = 0;
	targetRect.width = 128;		// for zach 128, 88 was default, might ought to be devided by 4 or 2(for resize)?
	targetRect.height = 64;

	magCurrentImg.allocate(targetRect.width * magRatio, targetRect.height * magRatio);
	smallCurrentImg.allocate(w/divisor, h/divisor);

	brightEyeImg.allocate(targetRect.width, targetRect.height);
	darkEyeImg.allocate(targetRect.width, targetRect.height);

	eFinder.setup(w, h, targetRect.width, targetRect.height, divisor);
	pFinder.setup(targetRect.width, targetRect.height, magRatio, w, h);
	gFinder.setup(targetRect.width, targetRect.height, magRatio, w, h);

	thresCal.setup(targetRect.width, targetRect.height, divisor);
	homographyCal.setup(100, 50, targetRect.width * magRatio, targetRect.height * magRatio);

	bUseGlintinBrightEye = false;
	bFoundEye = false;

}

//----------------------------------------------------
void eyeTracker::update(ofxCvGrayscaleImage & grayImgFromCam) {

	// get the image from input manager.
	currentImg.setFromPixels(grayImgFromCam.getPixels(), grayImgFromCam.width, grayImgFromCam.height);

	// get the small size image to find eye position.
	if (divisor !=1) smallCurrentImg.scaleIntoMe(currentImg, CV_INTER_LINEAR);

	// get the eye position
	bFoundOne = eFinder.update(smallCurrentImg, threshold_e, minBlobSize_e, maxBlobSize_e, true);
	bIsBrightEye = getBrightEyeDarkEye();

	if (eFinder.centroid.x > w - (targetRect.width/2) || eFinder.centroid.x < (targetRect.width/2) ||
		eFinder.centroid.y > h - (targetRect.height/2) || eFinder.centroid.y < (targetRect.height/2)){
		bFoundOne = false;
	}

	bFoundEye = false;
	bool bFoundPupil = false;

	briDarkFinder.odd ^= 1;

	if (bFoundOne){

		targetRect.x = eFinder.centroid.x - (targetRect.width/2);
		targetRect.y = eFinder.centroid.y - (targetRect.height/2);

		// make big eye image
		currentImg.setROI(targetRect);

		if (magRatio != 1) {
			magCurrentImg.scaleIntoMe(currentImg, CV_INTER_CUBIC);			// magnify by bicubic
		} else {
			magCurrentImg.setFromPixels(currentImg.getRoiPixels(), targetRect.width, targetRect.height);
		}

		currentImg.resetROI();

		// get current bright eye image & dark eye image


		// get glint position in a bright eye image, if needed. <= shoul be here..? think about it.
		if (bIsBrightEye && bUseGlintinBrightEye) {
			thresCal.update(smallCurrentImg, eFinder.diffImg, currentImg, targetRect, true);
			gFinder.findGlintCandidates(magCurrentImg, thresCal.getGlintThreshold(true), minBlobSize_g, maxBlobSize_g, true);
			targetRectBright = targetRect;
		}

		// find Pupil image again with the big eye image. (try double ellipse fit later?)
		if (!bIsBrightEye){

			// get the averages of pupil & white part.
			if (bUseAutoThreshold_g || bUseAutoThreshold_p){
				thresCal.update(smallCurrentImg, eFinder.diffImg, currentImg, targetRect, false);
			}

			if (bUseAutoThreshold_g) threshold_g = thresCal.getGlintThreshold(false);
			else threshold_g = threshold_g_frompanel;

			// get glint position with dark eye image.
			gFinder.update(magCurrentImg, threshold_g, minBlobSize_g, maxBlobSize_g, bUseGlintinBrightEye);

			if (/*gFinder.bFound*/true){ //TODO Fix

				if (bUseAutoThreshold_p) threshold_p = thresCal.getPupilThreshold();
				else threshold_p = threshold_p_frompanel;

				if (bUseHomography && gFinder.bFourGlints){
					// Homography..
					ofxCvGrayscaleAdvanced* temp = homographyCal.getWarpedImage(magCurrentImg, gFinder, magRatio);
					bFoundPupil = pFinder.update(*temp, threshold_p, minBlobSize_p, maxBlobSize_p, targetRect);

				} else {

					bFoundPupil = pFinder.update(magCurrentImg, threshold_p, minBlobSize_p, maxBlobSize_p, targetRect);
				}

				if (bFoundPupil){
					pupilCentroid = pFinder.currentPupilCenter;
					bFoundEye = true;
					targetRectDark = targetRect;
				}
			}
		}
	} //else {
//	    if(eFinderBoundingRect.width != 0 && eFinderBoundingRect.height != 0)
//	    {
//            cout << "Not found." << endl;
//            //set the ROI from the older ROI. That way we still get the correct frames for white and black
//            //todo, remove duplicated code. It's a little better
//            bool bBright = briDarkFinder.getBrightEyeDarkEye(currentImg, eFinderBoundingRect);
//
//            currentImg.setROI(targetRect);
//
//            if (bBright) brightEyeImg.setFromPixels(currentImg.getRoiPixels(), targetRect.width, targetRect.height);
//            else darkEyeImg.setFromPixels(currentImg.getRoiPixels(), targetRect.width, targetRect.height);
//
//            currentImg.resetROI();
//            bIsBrightEye = bBright;
//	    }
	//}
    //cout << "bFoundOne: " << bFoundOne << "  bFoundPupil: " << bFoundPupil << " bFoundGlint: " << gFinder.bFound << endl;
}

//----------------------------------------------------
bool eyeTracker::getBrightEyeDarkEye() {

    if(eFinder.boundingRect.width != 0 || eFinder.boundingRect.height != 0)
        eFinderBoundingRect = eFinder.boundingRect; //update the rect. if 0, don't update, we'll use the previous rect
    if(eFinderBoundingRect.width != 0 || eFinderBoundingRect.height != 0) //don't do this if no rect. Will crash program
    {
        bool bBright = briDarkFinder.getBrightEyeDarkEye(currentImg, eFinderBoundingRect);

        currentImg.setROI(targetRect);

        if (bBright) brightEyeImg.setFromPixels(currentImg.getRoiPixels(), targetRect.width, targetRect.height);
        else darkEyeImg.setFromPixels(currentImg.getRoiPixels(), targetRect.width, targetRect.height);

        currentImg.resetROI();
        return bBright;
    }
    return false; //rect hasn't been found yet. Does not really matter what is returned.
}

//----------------------------------------------------
ofPoint	eyeTracker::getGlintPoint(int glintID) {

	if (bUseHomography && gFinder.bFourGlints){
		ofPoint* temp;
		temp = homographyCal.getDstPos();
		return (temp[glintID] / magRatio);
	} else {
		return gFinder.getGlintPosition(glintID);
	}
}

//----------------------------------------------------
ofPoint	eyeTracker::getVectorGlintToPupil(int glintID) {

    if(!gFinder.bFound)
    {
        if(eFinderPrevFoundBlobCentroid.x != 0 && pupilCentroid.x != 0) //if not a bad point
            gFinder.updatePupilShift(eFinder.foundBlob.centroid - eFinderPrevFoundBlobCentroid);
        else
            return vectorGlintToPupil; //bad point, return the previous one
    }
    else //glint found
        gFinder.resetPupilShift();

	if (pupilCentroid.x != 0)
	{
		vectorGlintToPupil = pupilCentroid - getGlintPoint(glintID);
        eFinderPrevFoundBlobCentroid = eFinder.foundBlob.centroid;
	}
	return vectorGlintToPupil;
}

//----------------------------------------------------




// a bug is in the "findContours" in ofxCvContourFinder.
// I changed the order of lines.
// see findContour in ofxCvContourFinder.
//
// To use findContour with ROI, I changed the codes like below.
//
// From:
// inputCopy = input;
// inputCopy.setROI( input.getROI() );
//
// To:
// inputCopy.setROI( input.getROI() );
// inputCopy = input;


