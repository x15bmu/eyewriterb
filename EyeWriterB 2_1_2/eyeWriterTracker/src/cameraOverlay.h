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

#ifndef CAMERAOVERLAY_H
#define CAMERAOVERLAY_H

#include "ofxOpenCv.h"
#include <windows.h>

/**
 * This class creates a semi-transparent overlay window that
 * ignores all input i.e. it is transparent to mouse and keyboard events.
 * This is used for overlaying an image of the eye in the top left of the screen.
 */
class cameraOverlay
{
    public:
        /** Default constructor */
        cameraOverlay(IplImage *img, int overlayWidth = 0, int overlayHeight = 0);

        /**
         * Set the image address carried by the overlay. Image is not
         * changed within this class. Image address is not thread-safe.
         */
        void setImage(IplImage *img);

        /**
         * Set the width/height of the image. Passing 0 for either of these parameters
         * will cause the image to retain the previous value for the parameter set to 0.
         */
        void setImageSize(int w, int h);

        /**
         * Draw the image at the specified screen coordinates.
         * By default, draws the image with its original width and height at the top right of the screen.
         * Note, this uses screen coordinates, not window coordinates.
         * @param x The x coordinate to draw at.
         * @param y The y coordinate to draw at.
         * @return Returns false if function failed.
         */
        bool drawImage(int x = 0, int y = 0);

        /**
         * Set the opacity of the overlay.
         * @param value A number between 0 (fully transparent) and 255 (fully opaque)
         */
        void setOpacity(int value);

        /** Default destructor */
        ~cameraOverlay();
    protected:
        //Images for screen
        IplImage    *image;
        IplImage    *resizedImage;

        //Bitmaps and info
        HBITMAP     bitmap;
        CvMat       bitmapMat;
        void        *bitmapData;
        int         bitmapWidth, bitmapHeight, bitmapBitCount;
        //For storing default bitmap of HDCs
        HBITMAP     oldBitmap;

        //Device Context Handles
        HDC         memDC;

        //Keep track of whether class initialized properly.
        //Set to false if class failed to do so. May fail
        //because window class did not register or create properly.
        bool        initSuccess;

    private:
        int         width;
        int         height;
        int         opacity;

        bool        resizeNeeded;
        /**
         * Check if the image needs to be resized. Sets resizeNeeded bool.
         * Allocates new memory if necessary.
         * @return false if function failed to execute correctly
         */
        bool        checkResizeNeeded();
        /** Check if image properties changed. Recreates bitmaps if necessary. Returns false if function failed. */
        bool        checkImgPropsChanged(IplImage *img);

        /**
         * Allocate bitmap resources.
         * @param img The image to later be converted to a bitmap
         * @return Returns false if function failed
         */
        bool        allocBitmap(IplImage *img);
        /** Destroy bitmap.*/
        void        destroyBitmap();

        //////////////////////////////////////////////////////////////
        // Win32 Functions/Variables
        //////////////////////////////////////////////////////////////
        //Window Handle
        HWND overlayWindow;
        //Window Class
        static const char *overlayClassName;
        //Window Procedure Callback
        static LRESULT CALLBACK wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif // CAMERAOVERLAY_H
