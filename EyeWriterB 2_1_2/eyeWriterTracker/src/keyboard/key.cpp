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

#include "key.h"

int key::maxCount = 1500;

key::key()
{
    initKey();
    initFontByPtr = false;
    //font = new ofTrueTypeFont();
    //setFontSize(18);
}

key::key(ofTrueTypeFont *fontToLoad)
{
    initFontByPtr = true;
    initKey();
}

void key::initKey()
{
    w = h = 50;
    eyeX = eyeY = 0;
    counter = 0;
    regCounter = 0;
    regCounterStart = 0;
    setCounterStart = true;
    drawReg = false;
    drawHighlight = true;
    output = false;
    x = y = 0;
    setKeyColor(10, 10, 10);
    setHighlightColor(0, 115, 207);
    setRegisteredColor(64, 224, 208, 170);
    font = NULL;
}

key::~key()
{
    if(!initFontByPtr) {
        delete font;
        font = NULL;
    }
}

void key::update(float _eyeX, float _eyeY)
{
    eyeX = _eyeX;
    eyeY = _eyeY;
}

bool key::getKeyOut(bool removeState)
{
    if(output) {
        if(removeState) {
            keyOutProcess(&output, &drawHighlight, removeState);
        }
        return true;
    }
    keyNoOutProcess(removeState);
    return false;
}
void key::keyOutProcess(bool *output, bool *drawHighlight, bool removeState)
{
    *output = false;
    *drawHighlight = true;
}

bool key::setKeyColor(int r, int g, int b, int a /* = 255 */)
{
    if(r>=0 && r<=255 && g>=0 && g<=255 && b>=0 && b<=255 && a>=0 && a<=255) {
        keyColor[0] = r;
        keyColor[1] = g;
        keyColor[2] = b;
        keyColor[3] = a;
        return true;
    }
    return false;
}

bool key::setHighlightColor(int r, int g, int b)
{
    if(r>=0 && r<=255 && g>=0 && g<=255 && b>=0 && b<=255) {
        highlightColor[0] = r;
        highlightColor[1] = g;
        highlightColor[2] = b;
        return true;
    }
    return false;
}

bool key::setRegisteredColor(int r, int g, int b, int a /* = 255 */)
{
    if(r>=0 && r<=255 && g>=0 && g<=255 && b>=0 && b<=255 && a>=0 && a<=255) {
        registeredColor[0] = r;
        registeredColor[1] = g;
        registeredColor[2] = b;
        registeredColor[3] = a;
        return true;
    }
    return false;
}


void key::initFont()
{
    font = new ofTrueTypeFont();
    setFontSize(18);
    initFontByPtr = false;
}

void key::setFontPtr(ofTrueTypeFont *fontPtr)
{
    if(!initFontByPtr)
        delete font;
    font = fontPtr;
    initFontByPtr = true;
}

void key::drawKey()
{
    ///////////////////////////////////////////////////////////////////
    // Check if resize required
    ///////////////////////////////////////////////////////////////////
    /* since width & height are updated below, they store the old state of the width and height */
    if(width != w || height != h) {
        resizeObjects();
        width = w;  //need to set for inRect
        height = h; //need to set for inRect
    }

    drawKeyBackground();

    ///////////////////////////////////////////////////////////////////
    // Check mouse position
    ///////////////////////////////////////////////////////////////////
    if(inRect(eyeX, eyeY)) {
        mouseOverKey();
    } else {
        mouseLeaveKey();
        drawHighlight = true;
    }

    ///////////////////////////////////////////////////////////////////
    // Draw registered rect if needed
    ///////////////////////////////////////////////////////////////////
    if(drawReg) {
        drawReg = drawKeyRegistered(regCounterStart);
    }

    ///////////////////////////////////////////////////////////////////
    // Set the color, more color if more counter time elapsed
    ///////////////////////////////////////////////////////////////////
    alpha = ofMap(counter, 0, maxCount, 0, 255);
    if(alpha > 1) { //1 b/c floating point errors, no point in drawing if not necessary
        /* draw highlighted key if needed */
        drawKeyHighlight(alpha);

        if(counter >= maxCount) {
            drawReg = true;
            regCounterStart = ofGetElapsedTimeMillis();
            counter = 0;
            setCounterStart = true;//post message to reset counter
            output ^= 1; //reverse value, allows toggle key
        }
    }

    ///////////////////////////////////////////////////////////////////
    // Draw foreground behavior. (Default is text, but can be
    // reimplented to be picture)
    ///////////////////////////////////////////////////////////////////
    drawKeyLabel();
}

void key::mouseOverKey()
{
    ///////////////////////////////////////////////////////////////////
    // Set start moment of counter if needed
    ///////////////////////////////////////////////////////////////////
    if(setCounterStart) {
        counterStart = ofGetElapsedTimeMillis();
        setCounterStart = false;
    }

    ///////////////////////////////////////////////////////////////////
    // Set the counter value
    ///////////////////////////////////////////////////////////////////
    counter = ofGetElapsedTimeMillis() - counterStart;

    ///////////////////////////////////////////////////////////////////
    // If counter is way too big (100 ms), key "froze."
    //(It was not updated frequently enough). Reset the clock.
    // Otherwise, if counter is only a little too big, coerce counter.
    ///////////////////////////////////////////////////////////////////
    if(counter > maxCount + 100) {
        /* key froze */
        counter = 0;
        setCounterStart = true;
    } else if(counter >= maxCount) {
        counter = maxCount;
    }
}
void key::mouseLeaveKey()
{
    setCounterStart = true;
    counter *= 0.93f; //fade out
}
void key::drawKeyBackground()
{
    /* draw key */
    ofFill();
    ofSetRectMode(OF_RECTMODE_CORNER);
    ofSetColor(keyColor[0], keyColor[1], keyColor[2], keyColor[3]);
    roundedRect(x,y,w,h,r);
}
void key::drawKeyHighlight(float alpha)
{
    ofFill();
    ofSetColor(highlightColor[0], highlightColor[1], highlightColor[2], 200.f/255.f * alpha);
    roundedRect(x,y,w,h,r);
}
bool key::drawKeyRegistered(float timeSinceRegMillis)
{
    ofFill();
    ofSetColor(registeredColor[0], registeredColor[1], registeredColor[2], registeredColor[3]);
    roundedRect(x,y,w,h,r);

    if(ofGetElapsedTimeMillis() - regCounterStart > REGISTER_DISPLAY_TIME_MILLIS) {
        regCounterStart = 0;
        return false;
    }
    return true;
}
void key::drawKeyLabel()
{
    if(font == NULL)
        initFont();
    ofSetColor(200, 200 ,200);


    font->drawString(keyID, x + w/2 - font->stringWidth(keyID)/2, y + h/2 + 5); //must add 5 to center text vertically
}

void key::setFontSize(int size)
{
    if(font == NULL)
        initFont();
    font->loadFont("fonts/arial.ttf", size, true, true);
}
