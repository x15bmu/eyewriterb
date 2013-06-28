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

#ifndef IMAGEKEY_H
#define IMAGEKEY_H

#include "key.h"
#include "ofMain.h"

/**
 * @class imageKey imageKey.h "keyboard/imageKey.h"
 * @brief A \ref key that uses images instead of rendering
 * its background like a standard \ref key.
 */
class imageKey : virtual public key
{
    public:
        /** Default constructor */
        imageKey();

        /**
         * Construct imageKey with file names of images
         * (not recommended if using multiple keys with the same images).
         * @param backgroundFile The file name (or file path) for the background image.
         * @param highlightFile The file name (or file path) for the highlight image.
         * @param registeredFile The file name (or file path) for the registered image.
         * @param foregroundFile The file name (or file path) for the foreground image.
         */
        imageKey(string backgroundFile, string highlightFile, string registeredFile, string foregroundFile);

        /**
         * Construct imageKey with pointers to images
         * (recommended if using multiple keys with the same images).
         * @param backgroundImage A pointer to the background image.
         * @param highlightImage A pointer to the highlight image.
         * @param registeredImage A pointer to the registered image.
         * @param foregroundImage A pointer to the foreground image.
         */
        imageKey(ofImage *backgroundImage, ofImage *highlightImage, ofImage *registeredImage, ofImage *foregroundImage);
        virtual ~imageKey();

        /**
         * Set the key background image.
         * The image must be a format that can be loaded by of image.
         * If using multiple keys, it is recommended to intialize the image
         * by pointer rather than by file name to save memory.
         * @param fileName The file name (or file path) for the image to load.
         * @return Returns false if the function failed to load the image.
         */
        bool setKeyBackgroundImage(string fileName);

        /**
         * Set the key foreground image.
         * The image must be a format that can be loaded by of image.
         * If using multiple keys, it is recommended to intialize the image
         * by pointer rather than by file name to save memory.
         * @param fileName The file name (or file path) for the image to load.
         * @return Returns false if the function failed to load the image.
         */
        bool setKeyForegroundImage(string fileName);

        /**
         * Set the key highlight image.
         * The image must be a format that can be loaded by of image.
         * If using multiple keys, it is recommended to intialize the image
         * by pointer rather than by file name to save memory.
         * @param fileName The file name (or file path) for the image to load.
         * @return Returns false if the function failed to load the image.
         */
        bool setKeyHighlightImage(string fileName);

        /**
         * Set the key registered image.
         * The image must be a format that can be loaded by of image.
         * If using multiple keys, it is recommended to intialize the image
         * by pointer rather than by file name to save memory.
         * @param fileName The file name (or file path) for the image to load.
         * @return Returns false if the function failed to load the image.
         */
        bool setKeyRegisteredImage(string fileName);

        /**
         * Set the key background image.
         * The image must be a format that can be loaded by of image.
         * This is the recommended way to load images if using multiple keys.
         * @param image A pointer to the image to be used as the key background.
         * @return Returns false if the image passed is NULL.
         */
        bool setKeyBackgroundImage(ofImage *image);

        /**
         * Set the key foreground image.
         * The image must be a format that can be loaded by of image.
         * This is the recommended way to load images if using multiple keys.
         * @param image A pointer to the image to be used as the key background.
         * @return Returns false if the image passed is NULL.
         */
        bool setKeyForegroundImage(ofImage *image);

        /**
         * Set the key highlight image.
         * The image must be a format that can be loaded by of image.
         * This is the recommended way to load images if using multiple keys.
         * @param image A pointer to the image to be used as the key background.
         * @return Returns false if the image passed is NULL.
         */
        bool setKeyHighlightImage(ofImage *image);

        /**
         * Set the key registered image.
         * The image must be a format that can be loaded by of image.
         * This is the recommended way to load images if using multiple keys.
         * @param image A pointer to the image to be used as the key background.
         * @return Returns false if the image passed is NULL.
         */
        bool setKeyRegisteredImage(ofImage *image);

        /**
         * Reverts key drawing type to that of key (instead of imageKey).
         */
        void useStandardKey();

        /**
         * Deletes foreground image and displays keyID
         * instead of foreground image
         */
        void useKeyID();

        /**
         * Draws no foreground on the key.
         */
        void noForeground() { useKeyForeground = false; }

        /**
         * Draw a foreground on the key (default behavior).
         */
        void useForeground() { useKeyForeground = true; }

    protected:
        virtual void drawKeyBackground();
        virtual bool drawKeyRegistered(float timeSinceRegMillis);
        virtual void drawKeyHighlight(float alpha);
        virtual void drawKeyLabel();

    private:
        void init();
        enum initByPtrStruct
        {
            IBP_BACKGROUND,
            IBP_REGISTERED,
            IBP_HIGHLIGHT,
            IBP_FOREGROUND,
            IBP_SIZE
        };
        bool initByPtr[IBP_SIZE]; //so that memory is not accidently redeleted
        ofImage *background;
        ofImage *registered;
        ofImage *highlight;
        ofImage *foreground;

        bool useKeyForeground;
};

#endif // IMAGEKEY_H
