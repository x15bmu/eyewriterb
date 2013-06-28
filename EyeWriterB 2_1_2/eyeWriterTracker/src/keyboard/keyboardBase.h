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

#ifndef KEYBOARDBASE_H
#define KEYBOARDBASE_H

#include "key.h"
#include "imageKey.h"
#include "imageToggleKey.h"

enum SpaceKeyNames
{
    KB_SYMBOL,
    KB_COMMA,
    KB_SPACE,
    KB_ENTER,
    NUM_KEYS_SPACE
};
enum Outputs
{
    KB_OUTPUTNOCHANGE,
    KB_OUTPUTQWERTY,
    KB_OUTPUTQWERTYNOCAPS,
    KB_OUTPUTQWERTYCAPS,
    KB_OUTPUTNUMERIC,
    KB_OUTPUTSYMBOL,
    KB_OUTPUTASCIIUNICODE //ascii or unicode
};

struct KEYBDINFO
{
    int top;
    int bottom;
    int left;
    int right;
    int keyGap;
    int stdKeySize;
    int smallKeyH;
    float sizeMultiplier;
};

#define KB_KEYSIZE 127
#define KB_SPACEKEYWIDTH 670//660
#define KB_KEYHEIGHTBOTTOMROW 98
#define KB_KEYGAP 8
#define KB_OUTERGAPFROMBOTTOM 133 /*Gap to bottom of Key */
#define KB_FONTSIZE 38
#define KB_SPACEFONTSIZE 24
#define KB_NORMALHORIZGAP 12
#define KB_OUTERGAPFROMBOTTOMTOSPACE 25
#define KB_NORMSCREENWIDTH 1366
#define KB_NORMSCREENHEIGHT 768

/// @todo (Ben#1#): Remove unnecessary parameters. Split up functions.
class keyboardBase
{
    public:
        /** Routine for drawing the keyboard to screen */
        virtual void draw() = 0;
        /**
         * Routine for updating information about the keyboard, should be called before draw()
         * @param cursorX The x position of the cursor (e.g. mouse or eye position)
         * @param cursorY The y position of the cursor (e.g. mouse or eye position)
         * @param changeParams Whether parameters such as key size need to be changed.
         */
        virtual void update(int cursorX, int cursorY, bool changeParams) = 0;
        /** Send key outputs to the text field */
        virtual UINT sendOutputs() = 0;
        /** Do not load images for the keyboard. Use program-generated vector graphics instead. Possibly saves ram */
        virtual void useStandardKeys() = 0;

        keyboardBase();
        virtual ~keyboardBase();

        /**
         * Update the parameters for one key.
         * This function updates the cursors position for that key and automatically resizes
         * and repositions the key if the window changes size.
         * @param key A reference for the key to update
         * @param normDistFromCenterX The distance from the center of the screen (in pixels)
         * when ofGetWidth() returns KB_NORMSCREENWIDTH and ofGetHeight() returns KB_NORMSCREENHEIGHT
         * @param normPosY The y-coordinate (in pixels from the top of the window)
         * when ofGetWidth() returns KB_NORMSCREENWIDTH and ofGetHeight() returns KB_NORMSCREENHEIGHT.
         * @param cursorX The x-coordinate (in pixels) of the cursor.
         * @param cursorY The y-coordinate (in pixels) of the cursor.
         * @param changeParams Whether the parameters need to be update.
         * If false, this function only updates the cursor position.
         * @param normFontSize The font size when ofGetWidth() returns KB_NORMSCREENWIDTH
         * and ofGetHeight() returns KB_NORMSCREENHEIGHT. At present, this value does nothing.
         * In the future, it may change the font size if the window is resized.
         * @param normKeyWidth The width of the key (in pixels) when ofGetWidth() returns KB_NORMSCREENWIDTH
         * and ofGetHeight() returns KB_NORMSCREENHEIGHT
         * @param normKeyHeight The height of the key (in pixels) when ofGetWidth() returns KB_NORMSCREENHWIDTH
         * and ofGetHeight() returns KB_NORMSCREENHEIGHT
         */
        void updateParams(key& key, int normDistFromCenterX, int normPosY, int cursorX, int cursorY, bool changeParams,
                          int normFontSize = KB_FONTSIZE, int normKeyWidth = KB_KEYSIZE, int normKeyHeight = KB_KEYSIZE);

        /**
         * Update the parameters for arrays of keys.
         * This function is designed to update an entire keyboard of keys. It arranges the
         * keys into a grid with uniform spacing between the keys. It repositions and resizes
         * the keys as necessary. It also updates the cursor positions for each of the keys.
         * This function expects three rows of keys.
         * @param topKeys An array containing the keys in the top row of the keyboard.
         * @param numOfTopKeyElements The number of keys in the topKeys array.
         * @param middleKeys An array containing the keys in the middle row of the keyboard.
         * @param numOfMiddleKeyElements The number of keys in the middleKeys array.
         * @param bottomKeys An array containing the keys in the bottom row of the keyboard.
         * @param numOfBottomKeyElements The number of keys in the bottomKeys array.
         * @param horizOuterGapTop This parameter is not used.
         * @param horizOuterGapMiddle This parameter is not used.
         * @param horizOuterGapBottom This parameter is not used.
         * @param cursorX The x-coordinate of the cursor
         * @param cursorY The y-coordinate of the cursor.
         * @param changeParams If false, this function only updates cursor position.
         */
        void updateParams(imageKey *topKeys, int numOfTopKeyElements,
                          imageKey *middleKeys, int numOfMiddleKeyElements,
                          imageKey *bottomKeys, int numOfBottomKeyElements,
                          int horizOuterGapTop, int horizOuterGapMiddle, int horizOuterGapBottom,
                          int cursorX, int cursorY, bool changeParams);

        /**
         * Update the parameters for the keys on the same row as the spacebar.
         * This function arranges the keys into a grid with uniform spacing between the keys.
         * It repositions and resizes the keys as necessary.
         * It also updates the cursor positions for each of the keys.
         * @param spaceKeys An array containing the keys in the same row as the spacebar
         * @param numOfSpaceKeyElements The number of keys in the spaceKeys array. Must be 4.
         * @param cursorX The x-coordinate of the cursor.
         * @param cursorY The y-coordinate of the cursor.
         * @param changeParams If false, this function only updates cursor position
         */
        bool updateSpaceKeyParams(imageKey *spaceKeys, int numOfSpaceKeyElements, int cursorX, int cursorY, bool changeParams);

        /**
         * Draw the keys to the screen.
         * This function just calls drawKey() on all keys
         * @param topKeys An array containing the keys on the top row of the keyboard.
         * @param numOfTopKeyElements The number of keys in the topKeys array.
         * @param middleKeys An array containing the keys on the middle row of the keyboard.
         * @param numOfMiddleKeyElements The number of keys in the middleKeys array.
         * @param bottomKeys An array containing the keys on the bottom row of the keyboard.
         * @param numOfBottomKeyElements The number of keys in the bottomKeys array.
         * @param spaceKeys An array containing the keys on the same row as the spacebar.
         * @param numOfSpaceKeyElements The number of keys in the spaceKeys array.
         */
        void drawKeys(imageKey *topKeys, int numOfTopKeyElements,
                      imageKey *middleKeys, int numOfMiddleKeyElements,
                      imageKey *bottomKeys, int numOfBottomKeyElements,
                      imageKey *spaceKeys, int numOfSpaceKeyElements);

/// @todo (Ben#1#): Make non-static
        static KEYBDINFO getKeybdInfo() { return keybdInfo; }
        static float calcDistFromCenter(int x) { return (x - ofGetWidth()/2)/getKeybdInfo().sizeMultiplier; }



    protected:
    private:
        unsigned short prevWidth;
        unsigned short prevHeight;

        static KEYBDINFO keybdInfo;

};

#endif // KEYBOARDBASE_H
