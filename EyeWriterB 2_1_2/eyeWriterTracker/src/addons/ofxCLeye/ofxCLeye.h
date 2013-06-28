#pragma once
//ofxCLeye
//by Elliot Woods, Kimchi and Chips 2010
//MIT license

//tested with openFrameworks 0.061 / Visual Studio 2008

#include "CLEyeMulticam.h"

#include "ofMain.h"
#include "ofxOpenCv.h"

// ofxCLeye driver

class ofxCLeye : public ofVideoGrabber
{
	public:
		ofxCLeye();
		~ofxCLeye();
		void 			listDevices();
		void			grabFrame();
		void			close();
		bool			initGrabber(int w, int h, bool bTexture = true);
		void			videoSettings();
		//unsigned char*  getPixels();
		void			setDeviceGUID(GUID _deviceGUID);
		void			update();

		void			setExposure(int _exposure);
		void			setGain(int _gain);
		void			setLED(bool _led);
		void			setAutoExposure(bool _autoExposure);
		void			setAutoGain(bool _autoGain);

		int				getExposure() { return exposure; };
		int				getGain() { return gain; };
		bool			getLED() { return led; };
		bool			getAutoExposure() { return autoExposure; };
		bool			getAutoGain() { return autoGain; };



		CLEyeCameraColorMode	colourMode;

	protected:
		CLEyeCameraInstance		_cam;
		CLEyeCameraResolution	_resolution;

		bool					led;
		int						gain, exposure;
		bool					autoGain, autoExposure;

		unsigned char			*viPixels;
};
