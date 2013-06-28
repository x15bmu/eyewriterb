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

#include "keyboardBase.h"

KEYBDINFO keyboardBase::keybdInfo;

keyboardBase::keyboardBase() : prevWidth(0), prevHeight(0)
{
    //ctor
}

keyboardBase::~keyboardBase()
{
    //dtor
}

void keyboardBase::updateParams(key& key, int normDistFromCenterX, int normPosY, int cursorX, int cursorY, bool changeParams,
                                int normFontSize /* = KB_FONTSIZE */, int normKeyWidth /* = KB_KEYSIZE */, int normKeyHeight /* = KB_KEYSIZE */)
{
    key.update(cursorX, cursorY);
    if(!changeParams)
        return;
    else {
        ///////////////////////////////////////////////////////////////////
        // Set key position and size
        ///////////////////////////////////////////////////////////////////
        float widthRatio = static_cast<float>(ofGetWidth()) / KB_NORMSCREENWIDTH;
        float heightRatio = static_cast<float>(ofGetHeight()) / KB_NORMSCREENHEIGHT;
        float sizeMultiplier = min(widthRatio, heightRatio);

        normKeyWidth *= sizeMultiplier;
        if(normKeyWidth == 0) normKeyWidth = 1; //must have at least key of size 1px by 1px
        normKeyHeight *= sizeMultiplier;
        if(normKeyHeight == 0) normKeyHeight = 1;

        normFontSize *= sizeMultiplier;
        if(normFontSize == 0) normFontSize = 1; //must have at least font size of 1

        key.setX(ofGetWidth()/2 + normDistFromCenterX * sizeMultiplier);
        key.setY(normPosY * sizeMultiplier);
        key.setWidth(normKeyWidth);
        key.setHeight(normKeyHeight);
        //key.setFontSize(normFontSize);
    }
}

void keyboardBase::updateParams(imageKey *topKeys, int numOfTopKeyElements,
                                imageKey *middleKeys, int numOfMiddleKeyElements,
                                imageKey *bottomKeys, int numOfBottomKeyElements,
                                int horizOuterGapTop, int horizOuterGapMiddle, int horizOuterGapBottom,
                                int cursorX, int cursorY, bool changeParams)
{
    ///////////////////////////////////////////////////////////////////
    // Update cursor positions
    ///////////////////////////////////////////////////////////////////
    for(int i = 0; i < numOfTopKeyElements; i++)
        topKeys[i].update(cursorX, cursorY);
    for(int i = 0; i < numOfMiddleKeyElements; i++)
        middleKeys[i].update(cursorX, cursorY);
    for(int i = 0; i < numOfBottomKeyElements; i++)
        bottomKeys[i].update(cursorX, cursorY);

    if(!changeParams)
        return;
    else
    {
        ///////////////////////////////////////////////////////////////////
        // Initialize key positions and sizes
        ///////////////////////////////////////////////////////////////////

        /* Initialize size properties */
        float widthRatio = static_cast<float>(ofGetWidth()) / KB_NORMSCREENWIDTH;
        float heightRatio = static_cast<float>(ofGetHeight()) / KB_NORMSCREENHEIGHT;
        float sizeMultiplier = min(widthRatio, heightRatio);

        float keySize =  KB_KEYSIZE * sizeMultiplier;
        if(keySize < 1) keySize = 1; //must have at least key of size 1px by 1px
        float keyGap = KB_KEYGAP * sizeMultiplier;
        if(keyGap  < 1) keyGap = 1; //must have at least a 1px gap

        /* Set positions and sizes */
        int totalWidth = numOfTopKeyElements*keySize + (numOfTopKeyElements-1)*keyGap;
        int startXPosTop = (ofGetWidth() - totalWidth) / 2;
        totalWidth = numOfMiddleKeyElements*keySize + (numOfMiddleKeyElements-1)*keyGap;
        int startXPosMiddle = (ofGetWidth() - totalWidth) / 2;
        totalWidth = (numOfBottomKeyElements+1)*keySize + (numOfBottomKeyElements)*keyGap; //we're not drawing the Shift/more key here
        int startXPosBottom = (ofGetWidth() - totalWidth) / 2 + keySize + keyGap;

        /* Set gaps */
        horizOuterGapTop *= sizeMultiplier;
        horizOuterGapMiddle *= sizeMultiplier;
        horizOuterGapBottom *= sizeMultiplier;

        //Vertical gap from bottom to the zxcv row
        int outerGapFromBottom = sizeMultiplier * KB_OUTERGAPFROMBOTTOM;

        int fontSize = sizeMultiplier * KB_FONTSIZE;
        if(fontSize == 0) fontSize = 1; //must have at least font size of 1

        ///////////////////////////////////////////////////////////////////
        // Set key positions and sizes
        ///////////////////////////////////////////////////////////////////
        for(int i = 0; i < numOfTopKeyElements; i++) {
            topKeys[i].setX(startXPosTop + i*(keySize + keyGap));
            topKeys[i].setY(KB_NORMSCREENHEIGHT*sizeMultiplier - outerGapFromBottom - 2*keyGap - 3*keySize);
            topKeys[i].setWidth(keySize);
            topKeys[i].setHeight(keySize);
            //topKeys[i].setFontSize(fontSize);
        }
        for(int i = 0; i < numOfMiddleKeyElements; i++) {
            middleKeys[i].setX(startXPosMiddle + i*(keySize + keyGap));
            middleKeys[i].setY(KB_NORMSCREENHEIGHT*sizeMultiplier - outerGapFromBottom - keyGap - 2*keySize);
            middleKeys[i].setWidth(keySize);
            middleKeys[i].setHeight(keySize);
            //middleKeys[i].setFontSize(fontSize);
        }
        for(int i = 0; i < numOfBottomKeyElements; i++) {
            bottomKeys[i].setX(startXPosBottom + i*(keySize + keyGap));
            bottomKeys[i].setY(KB_NORMSCREENHEIGHT*sizeMultiplier - outerGapFromBottom - keySize);
            bottomKeys[i].setWidth(keySize);
            bottomKeys[i].setHeight(keySize);
            //bottomKeys[i].setFontSize(fontSize);
        }
        keybdInfo.left = topKeys[0].getX();
        keybdInfo.right = topKeys[numOfTopKeyElements-1].getX() + topKeys[numOfTopKeyElements-1].getWidth();
        keybdInfo.top = topKeys[0].getY();
        //keybdInfo.bottom = bottomKeys[0].getY() + bottomKeys[0].getHeight();
        keybdInfo.keyGap = keyGap;
        keybdInfo.stdKeySize = keySize;
        keybdInfo.sizeMultiplier = sizeMultiplier;
    }
}

bool keyboardBase::updateSpaceKeyParams(imageKey *spaceKeys, int numOfSpaceKeyElements, int cursorX, int cursorY, bool changeParams)
{
    if(numOfSpaceKeyElements != NUM_KEYS_SPACE)
        return false;

    ///////////////////////////////////////////////////////////////////
    // Update cursor positions
    ///////////////////////////////////////////////////////////////////
    for(int i = 0; i < numOfSpaceKeyElements; i++)
        spaceKeys[i].update(cursorX, cursorY);
    if(!changeParams)
        return true;
    else {
        ///////////////////////////////////////////////////////////////////
        // Initialize key positions and sizes
        ///////////////////////////////////////////////////////////////////

        /* Initialize size properties */
        float widthRatio = static_cast<float>(ofGetWidth()) / KB_NORMSCREENWIDTH;
        float heightRatio = static_cast<float>(ofGetHeight()) / KB_NORMSCREENHEIGHT;
        float sizeMultiplier = min(widthRatio, heightRatio);

        float keyWidth =  KB_KEYSIZE * sizeMultiplier;
        if(keyWidth < 1) keyWidth = 1; //must have at least key of size 1px by 1px
        float keyGap = KB_KEYGAP * sizeMultiplier;
        if(keyGap < 1) keyGap = 1; //must have at least a 1px gap

        /* Set positions and sizes */
        int totalWidth = 5*keyWidth + KB_SPACEKEYWIDTH*sizeMultiplier + 5 * keyGap;
        int startPosX = (ofGetWidth() - totalWidth) / 2;

        int fontSize = sizeMultiplier * KB_SPACEFONTSIZE;
        if(fontSize == 0) fontSize = 1; //must have at least font size of 1

        ///////////////////////////////////////////////////////////////////
        // Set key positions and sizes
        ///////////////////////////////////////////////////////////////////
        for(int i = 0; i < numOfSpaceKeyElements; i++) {
            /* Set width */
            if(i == 2) {
                //space key needs different width
                spaceKeys[i].setWidth(KB_SPACEKEYWIDTH*sizeMultiplier);
            } else {
                spaceKeys[i].setWidth(keyWidth);
            }
            /* Set Height */
            spaceKeys[i].setHeight(KB_KEYHEIGHTBOTTOMROW * sizeMultiplier);
            /* Set X Pos */
            if (i == 0) {
                spaceKeys[0].setX(startPosX);
            } else if(i == KB_ENTER) {
                spaceKeys[i].setX(spaceKeys[i-1].getX() + spaceKeys[i-1].getWidth() + 3*keyGap + 2*keyWidth); //missing ctrl and alt
            } else {
                spaceKeys[i].setX(spaceKeys[i-1].getX() + spaceKeys[i-1].getWidth() + keyGap);
            }
            /* Set Y Pos */
            spaceKeys[i].setY(sizeMultiplier*(KB_NORMSCREENHEIGHT - KB_OUTERGAPFROMBOTTOMTOSPACE - KB_KEYHEIGHTBOTTOMROW));
            /* Set Font Size */
            //spaceKeys[i].setFontSize(fontSize);
            keybdInfo.smallKeyH = KB_KEYHEIGHTBOTTOMROW * sizeMultiplier;
            keybdInfo.bottom = spaceKeys[0].getHeight() + spaceKeys[0].getWidth();
        }
    }
    return true;
}

void keyboardBase::drawKeys(imageKey *topKeys, int numOfTopKeyElements,
                            imageKey *middleKeys, int numOfMiddleKeyElements,
                            imageKey *bottomKeys, int numOfBottomKeyElements,
                            imageKey *spaceKeys, int numOfSpaceKeyElements)
{
    for(int i = 0; i < numOfTopKeyElements; i++)
        topKeys[i].drawKey();
    for(int i = 0; i < numOfMiddleKeyElements; i++)
        middleKeys[i].drawKey();
    for(int i = 0; i < numOfBottomKeyElements; i++)
        bottomKeys[i].drawKey();
    for(int i = 0; i < numOfSpaceKeyElements; i++)
        spaceKeys[i].drawKey();
}

