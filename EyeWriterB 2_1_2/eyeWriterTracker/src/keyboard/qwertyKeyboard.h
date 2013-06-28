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

#ifndef QWERTYKEYBOARD_H
#define QWERTYKEYBOARD_H

#include "imageKey.h"
#include "imageToggleKey.h"
#include "keyboardBase.h"
#include "textFieldEx.h"

#include <stdlib.h>
#include <string>
#include "ofMain.h"

class qwertyKeyboard : public keyboardBase
{
    public:
        qwertyKeyboard(ofImage *background, ofImage *registered, ofImage *highlight,
                       ofImage *backgroundSm, ofImage *registeredSm, ofImage *highlightSm,
                       ofImage *backgroundSp, ofImage *registeredSp, ofImage *highlightSp,
                       textFieldEx *_TFX, ofTrueTypeFont *font, ofTrueTypeFont *fontSm);
        /** Default destructor */
        virtual ~qwertyKeyboard();

        virtual void useStandardKeys();

        /** Draws the keys */
        void draw();

        void update(int cursorX, int cursorY, bool changeParams);

        /**
         * Send Outputs to Text Box.
         * @returns Returns something to the overarching Keyboard Class
         * Return values are specified in the Keyboard Base Class under
         * the enum "Outputs"
         */
        virtual UINT sendOutputs();

        /** Should be called if shift or caps lock is pressed */
        void setCaps();
        /** Should be called if shift or caps lock is released */
        void setNoCaps();

#define QK_NUM_KEYS_TOP 10
#define QK_NUM_KEYS_MID 9
#define QK_NUM_KEYS_BOT 9

        imageKey topKeys[QK_NUM_KEYS_TOP];
        imageKey middleKeys[QK_NUM_KEYS_MID];
        imageToggleKey shiftKey;
        imageKey bottomKeys[QK_NUM_KEYS_BOT];
        imageKey spaceKeys[NUM_KEYS_SPACE];
        imageToggleKey altKey;
        imageToggleKey ctrlKey;

    protected:
    private:
        qwertyKeyboard();
        void refreshKeyIDs();
        void refreshPermanentKeyIDs(); /* Keys that don't change when shift is clicked */
        std::string topIDs[QK_NUM_KEYS_TOP];
        std::string middleIDs[QK_NUM_KEYS_MID];
        std::string bottomIDs[QK_NUM_KEYS_BOT];
        std::string spaceIDs[NUM_KEYS_SPACE];
        textFieldEx *TFX;
};

#endif // QWERTYKEYBOARD_H
