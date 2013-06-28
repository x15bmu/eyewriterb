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

#include "imageToggleKey.h"

imageToggleKey::imageToggleKey() : imageKey()
{
    //ctor
}
imageToggleKey::imageToggleKey(string backgroundFile, string highlightFile, string registeredFile, string foregroundFile) :
    imageKey(backgroundFile, highlightFile, registeredFile, foregroundFile)
{

}
imageToggleKey::imageToggleKey(ofImage *backgroundImage, ofImage *highlightImage, ofImage *registeredImage, ofImage *foregroundImage) :
    imageKey(backgroundImage, highlightImage, registeredImage, foregroundImage)
{

}

imageToggleKey::~imageToggleKey()
{
    //dtor
}

/*
void imageToggleKey::keyOutProcess(bool *output, bool *drawHighlight)
{
    *drawHighlight = true;
}
*/

bool imageToggleKey::drawKeyRegistered(float timeSinceRegMillis)
{
    imageKey::drawKeyRegistered(timeSinceRegMillis);

    return output;
}
