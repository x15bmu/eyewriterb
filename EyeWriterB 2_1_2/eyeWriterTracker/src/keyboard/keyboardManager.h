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

#ifndef KEYBOARDMANAGER_H
#define KEYBOARDMANAGER_H

#include "qwertyKeyboard.h"
#include "numericKeyboard.h"
#include "keyboardBase.h"
#include "textPrediction.h"
#include "textFieldEx.h"
#include <vector>
#include <string>
#include "ofMain.h"


class keyboardManager : public keyboardBase
{
    public:
        keyboardManager();
        virtual ~keyboardManager();

        /**
         * Draw the keyboard.
         */
        void draw();

        /**
         * Update the cursor position.
         */
        void update(int cursorX, int cursorY);
        void setChangeParams(bool change = true);
        virtual void update(int a, int b, bool c) {} //required; TODO:FIX
        virtual UINT sendOutputs() {return 0;} //required; TODO:FIX
        bool getReturnState(bool remove = true) {return TFX.getReturnState(remove);}
        void setTFXMode(TFXMODE mode) {TFX.setTFXMode(mode);}
    protected:
    private:
        /**
         * Update various paramaters of Qwerty Keyboard
         *
         * Update draw size (if size changed). Send output of keyboard (if key pressed).
         * Update parameters of keyboard based on keyboard output (i.e. caps if new sentence).
         * @param cursorX The x position of the mouse.
         * @param cursorY The y position of the mouse.
         * @param resize Whether the keys should be resized.
         */
        void updateQwerty(int cursorX, int cursorY, bool resize);
        void updateNumeric(int cursorX, int cursorY, bool resize);
        void updatePredictions();

        virtual void useStandardKeys();

        int prevScreenWidth;
        int prevScreenHeight;
        enum loadedKeyboardState
        {
            LKS_QWERTY,
            LKS_NUMERIC
        } loadedKeyboard;
        //----------Normal----------
        ofImage background;
        ofImage registered;
        ofImage highlight;
        //----------Small-----------
        ofImage backgroundSm;
        ofImage registeredSm;
        ofImage highlightSm;
        //----------Space-----------
        ofImage backgroundSp;
        ofImage registeredSp;
        ofImage highlightSp;
        //--------Prediction--------
        ofImage backgroundP;
        ofImage registeredP;
        ofImage highlightP;

        ofTrueTypeFont font;
        ofTrueTypeFont fontSm;

        qwertyKeyboard QK;
        numericKeyboard NK;
#define KM_NUM_PKEYS 6
        imageKey pKeys[KM_NUM_PKEYS];
        textFieldEx TFX;

        WordPredictor wp;
        std::vector<std::string> predictions;
};

#endif // KEYBOARDMANAGER_H
