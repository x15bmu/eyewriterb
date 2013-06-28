//ofxCLeye
//by Elliot Woods, Kimchi and Chips 2010
//MIT license

#include "ofxCLeye.h"

ofxCLeye::ofxCLeye() :
ofVideoGrabber()
{
	colourMode = CLEYE_MONO_RAW;
	attemptFramerate = 60;
	gain = 20;
	exposure = 511;
}

ofxCLeye::~ofxCLeye()
{
	close();
}

void ofxCLeye::listDevices()
{
	int numCams = CLEyeGetCameraCount();

	cout << "ofxCLeye found " << numCams << "PlayStation Eye:\n";

	GUID guidCam;
	for (int i=0; i<numCams; i++)
	{
		guidCam = CLEyeGetCameraUUID(i);
		printf("Camera %d GUID: [%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x]\n",
				i, guidCam.Data1, guidCam.Data2, guidCam.Data3,
				guidCam.Data4[0], guidCam.Data4[1], guidCam.Data4[2],
				guidCam.Data4[3], guidCam.Data4[4], guidCam.Data4[5],
				guidCam.Data4[6], guidCam.Data4[7]);
	}

}

void ofxCLeye::grabFrame()
{
	if (bGrabberInited == true)
	{
		bIsFrameNew = false;
		if (CLEyeCameraGetFrame(_cam, (PBYTE) viPixels)){

			bIsFrameNew = true;

			if (bDoWeNeedToResize == true){
			    cout<<"resizing"<<endl;

				int inputW, inputH;
				CLEyeCameraGetFrameDimensions(_cam, inputW, inputH);

				float scaleW =	(float)inputW / (float)width;
				float scaleH =	(float)inputH / (float)height;

				for(int i=0;i<width;i++){
					for(int j=0;j<height;j++){

						float posx = i * scaleW;
						float posy = j * scaleH;

						/*

						// start of calculating
						// for linear interpolation

						int xbase = (int)floor(posx);
						int xhigh = (int)ceil(posx);
						float pctx = (posx - xbase);

						int ybase = (int)floor(posy);
						int yhigh = (int)ceil(posy);
						float pcty = (posy - ybase);
						*/

						int posPix = (((int)posy * inputW) + ((int)posx));

						pixels[(j*width) + i] = viPixels[posPix];

					}
				}

			} else {

				memcpy(pixels, viPixels, width*height);

			}

			if (bUseTexture){
				tex.loadData(pixels, width, height, GL_LUMINANCE);
			}

		}
	}
}

void ofxCLeye::close()
{
	// Stop camera capture
	CLEyeCameraStop(_cam);
	// Destroy camera object
	CLEyeDestroyCamera(_cam);
	_cam = NULL;

	delete[] viPixels;

	bGrabberInited = false;
}

bool ofxCLeye::initGrabber(int w, int h, bool bTexture)
{
	bUseTexture = bTexture;


	if (bChooseDevice){
		ofLog(OF_LOG_NOTICE, "choosing %i", deviceID);
	} else {
		device = 0;
	}

	width = w;
	height = h;
	bGrabberInited = false;

	////////////////////////////////////////
	// RESOLUTION
	////////////////////////////////////////
	if (width == 640 && height == 480)
		_resolution=CLEYE_VGA;
	else if (width == 320 && height == 240)
		_resolution=CLEYE_QVGA;
	else
	{
		ofLog(OF_LOG_WARNING, "Selected resolution " + ofToString(width) + "x" + ofToString(height) + " is not available with ofxCLeye");
		ofLog(OF_LOG_WARNING, "ofxCLeye using 640x480 instead");
		_resolution=CLEYE_VGA;
	}
	////////////////////////////////////////

	////////////////////////////////////////
	// FPS
	////////////////////////////////////////
	if( attemptFramerate < 0)
		return false;
	////////////////////////////////////////

	////////////////////////////////////////
	// INITIALISE
	////////////////////////////////////////
	GUID guid = CLEyeGetCameraUUID(deviceID);
	_cam = CLEyeCreateCamera(guid, colourMode, _resolution, attemptFramerate);
	bool bOk = (_cam != NULL);
	if (bOk) bOk &= CLEyeCameraStart(_cam);
	////////////////////////////////////////


	int ourRequestedWidth = width;
	int ourRequestedHeight = height;

	if (bOk == true){

		////////////////////////////////////////
		// GAIN/EXPOSURE
		////////////////////////////////////////
		if (gain<=0) //default
			gain=20;
		if (exposure<=0) //default
			exposure=511;
		CLEyeSetCameraParameter(_cam, CLEYE_GAIN, gain);
		CLEyeSetCameraParameter(_cam, CLEYE_EXPOSURE, exposure);
		////////////////////////////////////////

		CLEyeCameraGetFrameDimensions(_cam, width, height);
		bGrabberInited = true;

		//this line should also accomodate non-colour values
		//here we init an array at the cam's native resolution
		viPixels= new unsigned char[width * height];

		if (width == ourRequestedWidth && height == ourRequestedHeight){
			bDoWeNeedToResize = false;
		} else {
			bDoWeNeedToResize = true;
			width = ourRequestedWidth;
			height = ourRequestedHeight;
		}

		pixels	= new unsigned char[width * height];

		if (bUseTexture){
			// create the texture, set the pixels to black and
			// upload them to the texture (so at least we see nothing black the callback)
			tex.allocate(width,height,GL_LUMINANCE);
			memset(pixels, 0, width*height);
			tex.loadData(pixels, width, height, GL_LUMINANCE);
		}

		ofLog(OF_LOG_NOTICE, "ofxCLeye created camera successfully");
		return true;
	} else {
		ofLog(OF_LOG_ERROR, "error allocating a video device");
		ofLog(OF_LOG_ERROR, "please check your camera with CL-Eye Test");
		bGrabberInited = false;
		return false;
	}
}


void ofxCLeye::videoSettings()
{
	ofLog(OF_LOG_WARNING, "ofxCLeye does not support videoSettings()");
}

void ofxCLeye::setDeviceGUID(GUID _deviceGUID)
{
	int numCams = CLEyeGetCameraCount();

	for (int i=0; i<numCams; i++)
		if (CLEyeGetCameraUUID(i) == _deviceGUID)
		{
			ofVideoGrabber::setDeviceID(i);
			return;
		}

	ofLog(OF_LOG_WARNING, "ofxCLeye cannot find camera with selected GUID");
	deviceID		= 0;
	bChooseDevice	= false;
}

void ofxCLeye::update()
{
	grabFrame();
}

void ofxCLeye::setExposure(int _exposure)
{
	exposure = _exposure;

	if (bGrabberInited)
		CLEyeSetCameraParameter(_cam, CLEYE_EXPOSURE, exposure);

}

void ofxCLeye::setGain(int _gain)
{
	gain = _gain;

	if (bGrabberInited)
		CLEyeSetCameraParameter(_cam, CLEYE_GAIN, gain);


}

void ofxCLeye::setLED(bool _led)
{
	led = _led;

	if (bGrabberInited)
		CLEyeCameraLED(_cam, led);
}

void ofxCLeye::setAutoExposure(bool _autoExposure)
{
	autoExposure = _autoExposure;

	if (bGrabberInited)
		CLEyeSetCameraParameter(_cam, CLEYE_AUTO_EXPOSURE, autoExposure);
}

void ofxCLeye::setAutoGain(bool _autoGain)
{
	autoGain = _autoGain;

	if (bGrabberInited)
		CLEyeSetCameraParameter(_cam, CLEYE_AUTO_GAIN, autoGain);
}
