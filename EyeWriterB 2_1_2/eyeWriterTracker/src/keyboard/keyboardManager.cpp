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

#include "keyboardManager.h"

keyboardManager::keyboardManager() :

    QK(&background, &registered, &highlight,
       &backgroundSm, &registeredSm, &highlightSm,
       &backgroundSp, &registeredSp, &highlightSp,
       &TFX, &font, &fontSm),
    NK(&background, &registered, &highlight,
       &backgroundSm, &registeredSm, &highlightSm,
       &backgroundSp, &registeredSp, &highlightSp,
       &TFX, &font, &fontSm),


//       QK(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &TFX, NULL, NULL),
//       NK(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &TFX, NULL, NULL),

    loadedKeyboard(LKS_QWERTY),
    wp(TFX.textString),
    prevScreenWidth(0),
    prevScreenHeight(0)
{
    ///////////////////////////////////////////////////////////////////
    // Load key resources (images and fonts)
    ///////////////////////////////////////////////////////////////////
    bool imgSuccess = true;
    imgSuccess &= background.loadImage("images/keyboard/background.png");
    imgSuccess &= registered.loadImage("images/keyboard/registered.png");
    imgSuccess &= highlight.loadImage("images/keyboard/highlight.png");

    imgSuccess &= backgroundSm.loadImage("images/keyboard/backgroundSm.png");
    imgSuccess &= registeredSm.loadImage("images/keyboard/registeredSm.png");
    imgSuccess &= highlightSm.loadImage("images/keyboard/highlightSm.png");

    imgSuccess &= backgroundSp.loadImage("images/keyboard/backgroundSp.png");
    imgSuccess &= registeredSp.loadImage("images/keyboard/registeredSp.png");
    imgSuccess &= highlightSp.loadImage("images/keyboard/highlightSp.png");

    imgSuccess &= backgroundP.loadImage("images/keyboard/backgroundP.png");
    imgSuccess &= registeredP.loadImage("images/keyboard/registeredP.png");
    imgSuccess &= highlightP.loadImage("images/keyboard/highlightP.png");

    font.loadFont("fonts/arial.ttf", 20);
    fontSm.loadFont("fonts/arial.ttf", 13);
    if(imgSuccess) {
        for(int i = 0; i < sizeof(pKeys)/sizeof(pKeys[0]); i++) {
            pKeys[i].setKeyBackgroundImage(&backgroundP);
            pKeys[i].setKeyRegisteredImage(&registeredP);
            pKeys[i].setKeyHighlightImage(&highlightP);
        }
    } else {
        useStandardKeys();
    }
}

keyboardManager::~keyboardManager()
{
    //dtor
}

void keyboardManager::useStandardKeys()
{
    QK.useStandardKeys();
    NK.useStandardKeys();
}

void keyboardManager::update(int cursorX, int cursorY)
{
    ///////////////////////////////////////////////////////////////////
    // Check resize required
    ///////////////////////////////////////////////////////////////////
    bool resize = ((static_cast<int>(ofGetHeight()) != static_cast<int>(prevScreenHeight)) ||
                        (static_cast<int>(ofGetWidth()) != static_cast<int>(prevScreenWidth)));
    if(resize) {
        prevScreenHeight = ofGetHeight();
        prevScreenWidth = ofGetWidth();
    }

    ///////////////////////////////////////////////////////////////////
    // Update loaded keyboard
    ///////////////////////////////////////////////////////////////////
    if(loadedKeyboard == LKS_QWERTY)
        updateQwerty(cursorX, cursorY, resize);
    else
        updateNumeric(cursorX, cursorY, resize);

    /* Presage keys are handled separately */
    for(int i = 0; i < KM_NUM_PKEYS; i++)
        if(pKeys[i].getKeyOut())
            TFX.appendPKeyToString(pKeys[i]);
    /* Text prediction update */
    if(TFX.textString.length() != TFX.oldStringLength)
        updatePredictions();

    ///////////////////////////////////////////////////////////////////
    // Update keyboard parameters, e.g. size and position
    ///////////////////////////////////////////////////////////////////
    const int pKeyNormWidth = (KB_NORMSCREENWIDTH - 2*KB_NORMALHORIZGAP - (KM_NUM_PKEYS-1)*KB_KEYGAP)/KM_NUM_PKEYS;
    const int pKeyNormHeight = 90;
    //const int pKeyNormY = KB_NORMSCREENHEIGHT - 635;
    for(int i = 0; i < sizeof(pKeys)/sizeof(pKeys[0]); i++)
        updateParams(pKeys[i], calcDistFromCenter(getKeybdInfo().left) + i*(pKeyNormWidth+KB_KEYGAP),
                     getKeybdInfo().top/getKeybdInfo().sizeMultiplier - KB_KEYGAP - pKeyNormHeight,
                     cursorX, cursorY, resize, KB_FONTSIZE, pKeyNormWidth, pKeyNormHeight);
    TFX.update(cursorX, cursorY, KB_NORMALHORIZGAP, KB_NORMALHORIZGAP,
               ofGetWidth() - 2*KB_NORMALHORIZGAP, pKeys[0].getY() - 2*KB_NORMALHORIZGAP);
}

void keyboardManager::updateQwerty(int cursorX, int cursorY, bool resize)
{
    QK.update(cursorX, cursorY, resize);
    int ret = QK.sendOutputs();
    switch(ret) {
        case KP_ASCII:
        case KP_UNICODE:
            QK.setNoCaps();
            break;
        case KP_CHANGEMODE:
            loadedKeyboard = LKS_NUMERIC;
            setChangeParams();
            NK.setNumeric();
            break;
        case KP_DOUBLESPACEPERIOD:
        case KP_PERIOD:
            QK.setCaps(); //caps after peiod
    }
}

void keyboardManager::updateNumeric(int cursorX, int cursorY, bool resize)
{
    NK.update(cursorX, cursorY, resize);
    int ret = NK.sendOutputs();
    switch(ret) {
        case KP_CHANGEMODE:
            loadedKeyboard = LKS_QWERTY;
            setChangeParams();
            break;
    }
}

void keyboardManager::updatePredictions()
{
/// @todo (Ben#1#): Retain capitalization of word being predicted. Currently predictions are fully lowercase.
/// @todo (Ben#2#): More advanced text prediction, which is already supported by Presage although not currently implemented--
/// user profiles, multi-word text prediction, etc.
    predictions = wp.getPredictions();
    TFX.oldStringLength = TFX.textString.length();
    for(int i = 0; i < sizeof(pKeys)/sizeof(pKeys[0]); i++) {
        if(predictions.size() > i) {
            /* trying to display \b gives all sorts of errors and ultimately fails */
            int pos = predictions[i].find('\b');
            if(pos != string::npos)
                predictions[i].erase(pos, 1);
            pKeys[i].setKeyID(predictions[i]);
        }
        else
            pKeys[i].setKeyID("");
    }
}

void keyboardManager::draw()
{
    ///////////////////////////////////////////////////////////////////
    // Draw correct keyboard layout (QWERTY or numeric)
    ///////////////////////////////////////////////////////////////////
    if(loadedKeyboard == LKS_QWERTY)
        QK.draw();
    else
        NK.draw();

    ///////////////////////////////////////////////////////////////////
    // Draw prediction keys
    ///////////////////////////////////////////////////////////////////
    for(int i = 0; i < sizeof(pKeys)/sizeof(pKeys[0]); i++)
        pKeys[i].drawKey();

    ///////////////////////////////////////////////////////////////////
    // Draw text field
    ///////////////////////////////////////////////////////////////////
    TFX.draw();
}

void keyboardManager::setChangeParams(bool change)
{
    if(change)
        prevScreenWidth = prevScreenHeight = -1;
    else {
        prevScreenWidth = ofGetWidth();
        prevScreenHeight = ofGetHeight();
    }
}
