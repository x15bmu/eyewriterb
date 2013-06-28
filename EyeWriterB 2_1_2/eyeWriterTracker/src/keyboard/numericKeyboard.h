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

#ifndef NUMERICKEYBOARD_H
#define NUMERICKEYBOARD_H

#include "imageToggleKey.h"
#include "imageKey.h"
#include "keyboardBase.h"
#include "textFieldEx.h"

#include <stdlib.h>
#include <string>
#include "ofMain.h"


class numericKeyboard : public keyboardBase
{
    public:
        /** Default constructor */
        numericKeyboard(ofImage *background, ofImage *registered, ofImage *highlight,
                        ofImage *backgroundSm, ofImage *registeredSm, ofImage *highlightSm,
                        ofImage *backgroundSp, ofImage *registeredSp, ofImage *highlightSp,
                        textFieldEx *_TFX, ofTrueTypeFont *font, ofTrueTypeFont *fontSm);
        /** Default destructor */
        virtual ~numericKeyboard();

        virtual void useStandardKeys();

        void draw();
        void update(int cursorX, int cursorY, bool changeParams);
        virtual UINT sendOutputs();

        void setNumeric();
        void setSymbol();

        imageToggleKey altKey;
        imageToggleKey ctrlKey;
        imageToggleKey moreKey;

        enum keyboardState
        {
            KS_NONE,
            KS_NUMERIC,
            KS_SYMBOL
        };
        keyboardState ks;

    protected:
    private:
        void loadForegroundImages();
        void refreshKeyIDs(bool useKeyID = false);
        void refreshPermanentKeyIDs(); /* Keys that do not change when more is pressed */

#define NK_NUM_KEYS_TOP 10
#define NK_NUM_KEYS_MID 10
#define NK_NUM_KEYS_BOT 9

        imageKey topKeys[NK_NUM_KEYS_TOP];
        imageKey middleKeys[NK_NUM_KEYS_MID];
        imageKey bottomKeys[NK_NUM_KEYS_BOT];
        imageKey spaceKeys[NUM_KEYS_SPACE];


        std::string topIDs[NK_NUM_KEYS_TOP];
        std::string middleIDs[NK_NUM_KEYS_MID];
        std::string bottomIDs[NK_NUM_KEYS_BOT];
        std::string spaceIDs[NUM_KEYS_SPACE];

        textFieldEx *TFX;

        numericKeyboard();
};

#endif // NUMERICKEYBOARD_H
