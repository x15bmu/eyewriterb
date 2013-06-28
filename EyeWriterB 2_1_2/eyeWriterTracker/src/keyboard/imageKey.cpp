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

#include "imageKey.h"

imageKey::imageKey()
{
    for(int i = 0; i < IBP_SIZE; i++)
        initByPtr[i] = false;
    init();
}
imageKey::imageKey(string backgroundFile, string highlightFile, string registeredFile, string foregroundFile)
{
    for(int i = 0; i < IBP_SIZE; i++)
        initByPtr[i] = false;
    init();
    setKeyBackgroundImage(backgroundFile);
    setKeyHighlightImage(highlightFile);
    setKeyRegisteredImage(registeredFile);
    setKeyForegroundImage(foregroundFile);
}

imageKey::imageKey(ofImage *backgroundImage, ofImage *highlightImage, ofImage *registeredImage, ofImage *foregroundImage)
{
    for(int i = 0; i < IBP_SIZE; i++)
        initByPtr[i] = true;
    init();
    setKeyBackgroundImage(backgroundImage);
    setKeyHighlightImage(highlightImage);
    setKeyRegisteredImage(registeredImage);
    setKeyForegroundImage(foregroundImage);
}

void imageKey::init()
{
    key();
    useKeyForeground = true;
    background = registered = highlight = foreground = NULL;
}


imageKey::~imageKey()
{
    if(!initByPtr[IBP_BACKGROUND])
        delete background;
    if(!initByPtr[IBP_REGISTERED])
        delete registered;
    if(!initByPtr[IBP_HIGHLIGHT])
        delete highlight;
    if(!initByPtr[IBP_FOREGROUND])
        delete foreground;
}

void imageKey::useStandardKey()
{
    if(!initByPtr[IBP_BACKGROUND])
        delete background;
    if(!initByPtr[IBP_REGISTERED])
        delete registered;
    if(!initByPtr[IBP_HIGHLIGHT])
        delete highlight;
    if(!initByPtr[IBP_FOREGROUND])
        delete foreground;
    background = registered = highlight = foreground = NULL;
}
void imageKey::useKeyID()
{
    if(!initByPtr[IBP_FOREGROUND])
        delete foreground;
    foreground = NULL;
}

bool imageKey::setKeyBackgroundImage(string fileName)
{
    if(!initByPtr[IBP_BACKGROUND])
        delete background;
    initByPtr[IBP_BACKGROUND] = false;

    background = new ofImage();
    return background->loadImage(fileName);
}

bool imageKey::setKeyRegisteredImage(string fileName)
{
    if(!initByPtr[IBP_REGISTERED])
        delete registered;
    initByPtr[IBP_REGISTERED] = false;

    registered = new ofImage();
    return registered->loadImage(fileName);
}

bool imageKey::setKeyHighlightImage(string fileName)
{
    if(!initByPtr[IBP_HIGHLIGHT])
        delete highlight;
    initByPtr[IBP_HIGHLIGHT] = false;

    highlight = new ofImage();
    bool ret = highlight->loadImage(fileName);
    highlight->setImageType(OF_IMAGE_COLOR_ALPHA);
    return ret;
}

bool imageKey::setKeyForegroundImage(string fileName)
{
    if(!initByPtr[IBP_FOREGROUND])
        delete foreground;
    initByPtr[IBP_FOREGROUND] = false;

    foreground = new ofImage();
    return foreground->loadImage(fileName);
}

bool imageKey::setKeyBackgroundImage(ofImage *image)
{
    if(!initByPtr[IBP_BACKGROUND])
        delete background;
    initByPtr[IBP_BACKGROUND] = true;

    background = image;
    if(!background)
        return false;
    return true;
}

bool imageKey::setKeyRegisteredImage(ofImage *image)
{
    if(!initByPtr[IBP_REGISTERED])
        delete registered;
    initByPtr[IBP_REGISTERED] = true;
    registered = image;
    if(!registered)
        return false;
    return true;
}

bool imageKey::setKeyHighlightImage(ofImage *image)
{
    if(!initByPtr[IBP_HIGHLIGHT])
        delete highlight;
    initByPtr[IBP_HIGHLIGHT] = true;
    highlight = image;
    if(!highlight)
        return false;
    highlight->setImageType(OF_IMAGE_COLOR_ALPHA);
    return true;
}

bool imageKey::setKeyForegroundImage(ofImage *image)
{
    if(!initByPtr[IBP_FOREGROUND])
        delete foreground;
    initByPtr[IBP_FOREGROUND] = true;
    foreground = image;
    return true;
}

void imageKey::drawKeyBackground()
{
    if(!background) {
        /* execute default background function */
        key::drawKeyBackground();
        return;
    }
    /* if we don't set this to white, we get weird results because of alpha blending, see drawHighlight for more info */
    ofSetColor(255, 255, 255);
    background->draw(x, y, w, h);
}

bool imageKey::drawKeyRegistered(float timeSinceRegMillis)
{
    if(!registered) {
        /* execute default registered function */
        return key::drawKeyRegistered(timeSinceRegMillis);
    }
    /* if we don't set this to white, we get weird results because of alpha blending, see drawHighlight for more info */
    ofSetColor(255, 255, 255);
    registered->draw(x, y, w, h);
    if(ofGetElapsedTimeMillis() - timeSinceRegMillis > REGISTER_DISPLAY_TIME_MILLIS)
        return false;
    return true;
}

void imageKey::drawKeyHighlight(float alpha)
{
    if(!highlight) {
        /* execute default highlight function */
        key::drawKeyHighlight(alpha);
        return;
    }

    /* error checking to ensure nothing fails horribly */
    if(highlight->type != OF_IMAGE_COLOR_ALPHA)
        highlight->setImageType(OF_IMAGE_COLOR);

    ofEnableAlphaBlending();
    /* Every pixel in the texture (image) is tinted (i.e. multiplied) by the value of the color */
    ofSetColor(255, 255, 255, alpha);
    highlight->draw(x, y, w, h);
    ofSetColor(255, 255, 255, 255); //reset so that we don't draw other images with transparency as well
    //ofDisableAlphaBlending(); /* Bad stuff happens if we disable */
}

void imageKey::drawKeyLabel()
{
    if(useKeyForeground) {
        if(!foreground) {
            /* execute default label function */
            key::drawKeyLabel();
        } else {
            foreground->draw(x, y, w, h);
        }
    }
}
