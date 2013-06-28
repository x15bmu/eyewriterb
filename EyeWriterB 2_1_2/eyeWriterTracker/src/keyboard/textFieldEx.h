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

#ifndef TEXTFIELDEX_H
#define TEXTFIELDEX_H

#include "key.h"
#include "toggleKey.h"
#include "imageKey.h"
#include "ofImage.h"


enum KeyProcessed
{
    KP_NOKEY,

    /* For handleKeyOutput */
    KP_ASCII,
    KP_UNICODE,
    KP_BKSP,
    KP_ENTER,
    KP_TAB,
    KP_UPARROW,
    KP_LEFTARROW,
    KP_DOWNARROW,
    KP_RIGHTARROW,
    KP_DOUBLESPACEPERIOD,
    KP_PERIOD,
    KP_CHANGEMODE,

    /* For handleToggleKeyOutput */
    KP_SHIFT,
    KP_ALT,
    KP_CTRL,
    KP_MORE,
    KP_ERROR
};

enum TFXMODE
{
    TFX_STANDALONE,
    TFX_GLOBALMOUSE
};

class textFieldEx
{
    public:
        textFieldEx();
        ~textFieldEx();

        //void grabKeyStates();
        /** Add the keyID to the string */

        /** @return The type of key processed */
        UINT handleKeyOutput(key& k);
        /** @return The type of key processed. A negative value, means key released, a positive means pressed. */
        int handleToggleKeyOutput(toggleKey &k);
        /** Add the word prediction to the string*/
        void appendPKeyToString(key& pKey);
        /** Send the keys specified in the given strings to windows */
        void convertStringToOutput(const char* c);
        /**
         * Get the textString
         * @return Returns the actual text string (by pointer)
         */
        string* getTextString();
        /** Deprecated */
        void displayString();
        /** Update Field */
        void update(int cursorX, int cursorY, int x = 10, int y = 10, int w = ofGetWidth() - 20, int h = ofGetHeight() / 10);
        /** To be called during draw. Draws the text box. */
        void draw();
        /** Draws the text cursor */
        void drawCursor();
        /** checks the clear button and clears it if necessary */
        void checkClear();

        void    setTFXMode(TFXMODE requestedMode);
        int     getTFXMode() { return mode; }
        bool    getReturnState(bool remove = true);

        string          textString;
        int             oldStringLength; //this is NOT updated by the class

    private:
        string          cursor;
        string          strToDraw;

        ///Stores the string from STANDALONE mode, which is replaced in GLOBALMOUSE mode
        string          storedStandAloneStr;

        ofxTextBlock    tBlock;
        bool            cursorOn;
        imageKey        boxKey;
        bool            returnState;
        TFXMODE         mode;

        ofImage         keyImage;
        int             normImageWidth;
        int             normImageHeight;
        bool            imageLoaded;

        struct field
        {
            int x; int y; int w; int h;
        } fieldSize;
};

#endif // TEXTFIELDEX_H
