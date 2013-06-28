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

#ifndef IMAGETOGGLEKEY_H
#define IMAGETOGGLEKEY_H

#include "imageKey.h"
#include "toggleKey.h"

/**
 * @class imageToggleKey imageToggleKey.h "keyboard/imageToggleKey.h"
 * @brief A toggleKey that uses images instead of rendering its background
 * like a standard toggleKey or \ref key.
 */
class imageToggleKey : public imageKey, public toggleKey
{
    public:
        /** Default constructor */
        imageToggleKey();

        /**
         * Construct imageToggleKey with file names of images
         * (not recommended if using multiple keys with the same images).
         * @param backgroundFile The file name (or file path) for the background image.
         * @param highlightFile The file name (or file path) for the highlight image.
         * @param registeredFile The file name (or file path) for the registered image.
         * @param foregroundFile The file name (or file path) for the foreground image.
         */
        imageToggleKey(string backgroundFile, string highlightFile, string registeredFile, string foregroundFile);

        /**
         * Construct imageToggleKey with pointers to images
         * (recommended if using multiple keys with the same images).
         * @param backgroundImage A pointer to the background image.
         * @param highlightImage A pointer to the highlight image.
         * @param registeredImage A pointer to the registered image.
         * @param foregroundImage A pointer to the foreground image.
         */
        imageToggleKey(ofImage *backgroundImage, ofImage *highlightImage, ofImage *registeredImage, ofImage *foregroundImage);
        virtual ~imageToggleKey();

    protected:
        virtual void keyOutProcess(bool *output, bool *drawHighlight, bool removeState) { toggleKey::keyOutProcess(output, drawHighlight, removeState); }
        virtual bool drawKeyRegistered(float timeSinceRegMillis);
    private:
};

#endif // IMAGETOGGLEKEY_H
