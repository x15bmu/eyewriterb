#ifndef CLEYE_H
#define CLEYE_H

//#include "ofVideoGrabber.h"
#include "CLEyeMulticam.h"
#include "ofMain.h"
#include "ofxOpenCv.h"


class clEye : public ofVideoGrabber
{
    public:
		clEye();
		virtual ~clEye();
		void 			listDevices();
		void			grabFrame();
		void			close();
		bool			initGrabber(int w, int h, bool bTexture = true);
		void			videoSettings();
		unsigned char*  getPixels();
		void			setDeviceGUID(GUID _deviceGUID);
		void			update();

		void			setExposure(int _exposure);
		void			setGain(int _gain);
		void			setLED(bool _led);
		void			setAutoExposure(bool _autoExposure);
		void			setAutoGain(bool _autoGain);
        void            setHFlip(bool flip);
        void            setVFlip(bool flip);

		int				getExposure();
		int				getGain();
		bool			getLED();
		bool			getAutoExposure();
		bool			getAutoGain();



		CLEyeCameraColorMode	colorMode;

	protected:
		CLEyeCameraInstance		cam;
		CLEyeCameraResolution	resolution;

		bool                    led;

		IplImage                *pCapImage;
        PBYTE                   pCapBuffer;
        IplImage                *imgForOfxCvColor;
        IplImage                *imgForOfxCvColor2;
        IplImage                **imgPtr;

};

#endif // CLEYE_H
