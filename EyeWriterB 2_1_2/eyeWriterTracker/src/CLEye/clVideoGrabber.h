#ifndef CLVIDEOGRABBER_H_INCLUDED
#define CLVIDEOGRABBER_H_INCLUDED

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "baseVideoGrabber.h"
#include "threadedClVideoGrabber.h"

class clVideoGrabber : public baseVideoGrabber  {

public:
	void setup(int _width, int _height, bool _bUseTexture = false){
		vidGrabber.setup(_width, _height, this);
		width = vidGrabber.width;
		height = vidGrabber.height;

		if (width !=0 || height != 0){
			colorImg.setUseTexture(bUseTexture);
			grayImg.setUseTexture(bUseTexture);
			colorImg.allocate(width, height);
			grayImg.allocate(width, height);
		}
		vidGrabber.startCapture(false);

		bUseTexture = _bUseTexture;
		bNeedUpdate = false;
	}
	void update(){
		if (bNeedUpdate){
			//vidGrabber.lock(); //i dont see the point of this
			bNeedUpdate = false;
			//vidGrabber.unlock();
		}
	}
	void draw(int x, int y){
		if(bUseTexture){
			colorImg.draw(x,y);
		}
	}
	void setDeviceID(int device){
		vidGrabber.setDeviceID(device);
	}

	void setAutoGain(bool gain)
	{
	    vidGrabber.lock();
	    vidGrabber.setAutoGain(gain);
	    vidGrabber.unlock();
	}
	void setGain(float gain)
	{
        vidGrabber.lock();
        vidGrabber.setGain(gain);
        vidGrabber.unlock();
	}
	void setAutoExposure(bool exposure)
	{
	    vidGrabber.lock();
	    vidGrabber.setAutoExposure(exposure);
	    vidGrabber.unlock();
	}
	void setExposure(float exposure)
	{
	    vidGrabber.lock();
	    vidGrabber.setExposure(exposure);
        vidGrabber.unlock();
	}
	void setHFlip(bool flip)
	{
	    vidGrabber.lock();
	    vidGrabber.setHFlip(flip);
	    vidGrabber.unlock();
	}
	void setVFlip(bool flip)
    {
        vidGrabber.lock();
        vidGrabber.setVFlip(flip);
        vidGrabber.unlock();
    }
	bool getAutoGain()
	{
	    vidGrabber.lock();
	    bool ret = vidGrabber.getAutoGain();
	    vidGrabber.unlock();
	    return ret;
	}
	int getGain()
	{
	    vidGrabber.lock();
	    int ret = vidGrabber.getGain();
	    vidGrabber.unlock();
	    return ret;
	}
	bool getAutoExposure()
	{
	    vidGrabber.lock();
	    bool ret = vidGrabber.getAutoExposure();
	    vidGrabber.unlock();
	    return ret;
	}
	int getExposure()
	{
	    vidGrabber.lock();
	    int ret = vidGrabber.getExposure();
	    vidGrabber.unlock();
	    return ret;
	}


	//------- don't call this function from main thread, this is only for vidGrabber -------
	void frameReceived(clEye& grabber){
		vidGrabber.getThreadedPixels(colorImg, bNeedUpdate);
		vidGrabber.lock();															// lock again..
		cvCvtColor(colorImg.getCvImage(), grayImg.getCvImage(), CV_RGB2GRAY);
		vidGrabber.unlock();
	}

protected:
	threadedClVideoGrabber	vidGrabber;
	ofxCvColorImage			colorImg;

};


#endif // CLVIDEOGRABBER_H_INCLUDED
