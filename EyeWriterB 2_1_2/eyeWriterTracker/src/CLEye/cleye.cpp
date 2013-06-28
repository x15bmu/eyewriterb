#include "cleye.h"

clEye::clEye()
{
    cam = NULL;
    pCapImage = NULL;
    pCapBuffer = NULL;
    deviceID = 0;
    colorMode = CLEYE_COLOR_RAW; //TODO: rewrite to include mono as well. will be faster.
    attemptFramerate = 30;
}

clEye::~clEye()
{
    //dtor
}

bool clEye::initGrabber(int w, int h, bool bTexture /* = true */)
{
    bool bOk = true;

    GUID guid = CLEyeGetCameraUUID(deviceID);

    CLEyeCameraResolution _resolution = CLEYE_VGA;
    if(w == 320 && h == 240)
        _resolution = CLEYE_QVGA;
    else if(w != 640 || h != 480)
        ofLog(OF_LOG_WARNING, "Desired frame size not supported by CL. Reverting to 640x480");

    width = w;
    height = h; //prevent from displaying weird if initialization fails

    if(bOk &= (attemptFramerate > 0))
        cam = CLEyeCreateCamera(guid, colorMode, _resolution, attemptFramerate);
    if(bOk &= (cam != NULL))
        bOk &= CLEyeCameraStart(cam);
    if(bOk)
    {
        CLEyeCameraGetFrameDimensions(cam, width, height);

        //--------------------------------------------
        //----   Create Images for Getting Data   ----
        //--------------------------------------------
        pCapImage = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 4); //we're doing color. TODO: implement B&W
        imgForOfxCvColor = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
        imgForOfxCvColor2 = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
        imgPtr = &imgForOfxCvColor;
        cvGetImageRawData(pCapImage, &pCapBuffer);

        CLEyeSetCameraParameter(cam, CLEYE_GAIN, 0); //default
        CLEyeSetCameraParameter(cam, CLEYE_EXPOSURE, 511); //default

        bUseTexture = bTexture;
        if(bUseTexture)
            ofLog(OF_LOG_ERROR, "Texture usage not supported at this time.");
        ofLog(OF_LOG_NOTICE, "Created CL camera successfully.");
        bGrabberInited = true;
        return true;
    }
    else
    {
        ofLog(OF_LOG_ERROR, "Failed to initialize CL Camera. Check camera with CL Eye Test.");
        MessageBox(NULL, "Failed to initialize CL Camera. Check camera with CL Eye Test.", "Fatal Error", MB_ICONERROR | MB_OK);
        return false;
    }
}

void clEye::grabFrame()
{
    if(bGrabberInited)
    {
        cvGetImageRawData(pCapImage, &pCapBuffer);
        if(CLEyeCameraGetFrame(cam, pCapBuffer))
        {
            memset(&pCapBuffer, 20, sizeof pCapBuffer);
            IplImage **buffer = (imgPtr == &imgForOfxCvColor) ? &imgForOfxCvColor2 : &imgForOfxCvColor;
            cvCvtColor(pCapImage, *buffer, CV_RGBA2RGB);

            //swap the buffers
            IplImage **temp;
            temp = imgPtr;
            imgPtr = buffer;
            buffer = temp;

            bIsFrameNew = true;
        }
    }
}
void clEye::update()
{
    grabFrame();
}

void clEye::listDevices()
{
    /* From ofxCLeye by Elliot Woods, Kimchi and Chips 2010, MIT license */
    int numCams = CLEyeGetCameraCount();

	cout << "Found " << numCams << " PlayStation Eye(s):\n";

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

void clEye::setDeviceGUID(GUID _deviceGUID)
{
    int numCams = CLEyeGetCameraCount();

    //this doesnt work.

	for (int i=0; i<numCams; i++)
		if (CLEyeGetCameraUUID(i) == _deviceGUID)
		{
			ofVideoGrabber::setDeviceID(i);
			return;
		}
    ofLog(OF_LOG_WARNING, "Could not find camera with specified GUID. Selecting default device (0, unless previously set).");
}

void clEye::close()
{
    CLEyeCameraStop(cam);
    CLEyeDestroyCamera(cam);
    cvReleaseImage(&pCapImage);
    bGrabberInited = false;
}

void clEye::videoSettings()
{
    ofLog(OF_LOG_WARNING, "Warning: videoSettings() not supported by clEye");
}

unsigned char* clEye::getPixels()
{
    return (unsigned char*) (*imgPtr)->imageData;
}

void clEye::setExposure(int _exposure)
{
    if(bGrabberInited)
        CLEyeSetCameraParameter(cam, CLEYE_EXPOSURE, _exposure);
}

void clEye::setGain(int _gain)
{
    if(bGrabberInited)
        CLEyeSetCameraParameter(cam, CLEYE_GAIN, _gain);
}

void clEye::setLED(bool _led)
{
    if(bGrabberInited)
    {
        CLEyeCameraLED(cam, _led);
        led = _led;
    }
}

void clEye::setAutoExposure(bool _autoExposure)
{
    if(bGrabberInited)
        CLEyeSetCameraParameter(cam, CLEYE_AUTO_EXPOSURE, _autoExposure);
}

void clEye::setAutoGain(bool _autoGain)
{
    if(bGrabberInited)
        CLEyeSetCameraParameter(cam, CLEYE_AUTO_GAIN, _autoGain);
}

void clEye::setHFlip(bool flip)
{
    if(bGrabberInited)
        if(flip != CLEyeGetCameraParameter(cam, CLEYE_HFLIP))
            CLEyeSetCameraParameter(cam, CLEYE_HFLIP, flip);
}

void clEye::setVFlip(bool flip)
{
    if(bGrabberInited)
        if(flip != CLEyeGetCameraParameter(cam, CLEYE_VFLIP))
            CLEyeSetCameraParameter(cam, CLEYE_VFLIP, flip);
}

int clEye::getExposure()
{
    if(bGrabberInited)
        return CLEyeGetCameraParameter(cam, CLEYE_EXPOSURE);
    return 511; //default
}

int clEye::getGain()
{
    if(bGrabberInited)
        return CLEyeGetCameraParameter(cam, CLEYE_GAIN);
    return 79; //default
}

bool clEye::getLED()
{
    return led;
}

bool clEye::getAutoExposure()
{
    if(bGrabberInited)
        return CLEyeGetCameraParameter(cam, CLEYE_AUTO_EXPOSURE);
    return false; //i guess
}

bool clEye::getAutoGain()
{
    if(bGrabberInited)
        return CLEyeGetCameraParameter(cam, CLEYE_AUTO_GAIN);
    return false; //i guess
}
