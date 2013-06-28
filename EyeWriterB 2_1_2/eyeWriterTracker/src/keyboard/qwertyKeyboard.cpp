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

#include "qwertyKeyboard.h"

using std::string;
using std::copy;

qwertyKeyboard::qwertyKeyboard(ofImage *background, ofImage *registered, ofImage *highlight,
                               ofImage *backgroundSm, ofImage *registeredSm, ofImage *highlightSm,
                               ofImage *backgroundSp, ofImage *registeredSp, ofImage *highlightSp,
                               textFieldEx *_TFX, ofTrueTypeFont *font, ofTrueTypeFont *fontSm) :
    shiftKey(background, highlight, registered, NULL),
    altKey(backgroundSm, highlightSm, registeredSm, NULL),
    ctrlKey(backgroundSm, highlightSm, registeredSm, NULL)

{
    for(int i = 0; i < QK_NUM_KEYS_TOP; i++)
    {
        topKeys[i].setKeyBackgroundImage(background);
        topKeys[i].setKeyRegisteredImage(registered);
        topKeys[i].setKeyHighlightImage(highlight);
        topKeys[i].setFontPtr(font);
    }
    for(int i = 0; i < QK_NUM_KEYS_MID; i++)
    {
        middleKeys[i].setKeyBackgroundImage(background);
        middleKeys[i].setKeyRegisteredImage(registered);
        middleKeys[i].setKeyHighlightImage(highlight);
        middleKeys[i].setFontPtr(font);
    }
    for(int i = 0; i < QK_NUM_KEYS_BOT; i++)
    {
        bottomKeys[i].setKeyBackgroundImage(background);
        bottomKeys[i].setKeyRegisteredImage(registered);
        bottomKeys[i].setKeyHighlightImage(highlight);
        bottomKeys[i].setFontPtr(font);
    }
    for(int i = 0; i < NUM_KEYS_SPACE; i++)
    {
        if(i == KB_SPACE)
        {
            spaceKeys[i].setKeyBackgroundImage(backgroundSp);
            spaceKeys[i].setKeyRegisteredImage(registeredSp);
            spaceKeys[i].setKeyHighlightImage(highlightSp);
            spaceKeys[i].setFontPtr(fontSm);
        }
        else
        {
            spaceKeys[i].setKeyBackgroundImage(backgroundSm);
            spaceKeys[i].setKeyRegisteredImage(registeredSm);
            spaceKeys[i].setKeyHighlightImage(highlightSm);
            spaceKeys[i].setFontPtr(fontSm);
        }
    }
    shiftKey.setKeyForegroundImage("images/keyboard/QWE_BOT_0.png");
    altKey.setFontPtr(fontSm);
    ctrlKey.setFontPtr(fontSm);


    bottomKeys[8].setKeyForegroundImage("images/keyboard/UNI_BOT_8.png"); //BKSP : universal set, bottom row, 9th key (More key is not bottom row)
    spaceKeys[KB_ENTER].setKeyForegroundImage("images/keyboard/UNI_SPA_5.png"); // ENTER : universal set, space row, 6th key

    setCaps();

    refreshPermanentKeyIDs();

    TFX = _TFX;
}

qwertyKeyboard::~qwertyKeyboard()
{
    shiftKey.setKeyOut(false);
    altKey.setKeyOut(false);
    ctrlKey.setKeyOut(false);
    TFX->handleToggleKeyOutput(shiftKey);
    TFX->handleToggleKeyOutput(altKey);
    TFX->handleToggleKeyOutput(ctrlKey);
}

void qwertyKeyboard::useStandardKeys()
{
    for(int i = 0; i < QK_NUM_KEYS_TOP; i++)
        topKeys[i].useStandardKey();
    for(int i = 0; i < QK_NUM_KEYS_MID; i++)
        middleKeys[i].useStandardKey();
    for(int i = 0; i < QK_NUM_KEYS_BOT; i++)
        bottomKeys[i].useStandardKey();
    for(int i = 0; i < NUM_KEYS_SPACE; i++)
        spaceKeys[i].useStandardKey();
    shiftKey.useStandardKey();
    altKey.useStandardKey();
    ctrlKey.useStandardKey();
}

void qwertyKeyboard::update(int cursorX, int cursorY, bool changeParams)
{
    updateParams(topKeys, QK_NUM_KEYS_TOP, middleKeys, QK_NUM_KEYS_MID, bottomKeys, QK_NUM_KEYS_BOT,
                 KB_NORMALHORIZGAP, KB_NORMALHORIZGAP + KB_KEYSIZE/2, KB_NORMALHORIZGAP + KB_KEYSIZE + KB_KEYGAP,
                 cursorX, cursorY, changeParams);
    updateParams(shiftKey, calcDistFromCenter(getKeybdInfo().left),
                 KB_NORMSCREENHEIGHT - KB_OUTERGAPFROMBOTTOM - KB_KEYSIZE,
                 cursorX, cursorY, changeParams, KB_SPACEFONTSIZE);

    updateParams(altKey, calcDistFromCenter(getKeybdInfo().right) - 3*KB_KEYSIZE - 2*KB_KEYGAP,
                 KB_NORMSCREENHEIGHT - KB_OUTERGAPFROMBOTTOMTOSPACE - KB_KEYHEIGHTBOTTOMROW,
                 cursorX, cursorY, changeParams, KB_SPACEFONTSIZE, KB_KEYSIZE, KB_KEYHEIGHTBOTTOMROW);

    updateParams(ctrlKey, calcDistFromCenter(getKeybdInfo().right) - 2*KB_KEYSIZE - KB_KEYGAP,
                 KB_NORMSCREENHEIGHT - KB_OUTERGAPFROMBOTTOMTOSPACE - KB_KEYHEIGHTBOTTOMROW,
                 cursorX, cursorY, changeParams, KB_SPACEFONTSIZE, KB_KEYSIZE, KB_KEYHEIGHTBOTTOMROW);

    if(!updateSpaceKeyParams(spaceKeys, NUM_KEYS_SPACE, cursorX, cursorY, changeParams))
        MessageBox(NULL, "Error updating space-row parameters.", "Error", MB_OK | MB_ICONERROR);
}

void qwertyKeyboard::draw()
{
    drawKeys(topKeys, QK_NUM_KEYS_TOP, middleKeys, QK_NUM_KEYS_MID, bottomKeys, QK_NUM_KEYS_BOT, spaceKeys, NUM_KEYS_SPACE);
    shiftKey.drawKey();
    altKey.drawKey();
    ctrlKey.drawKey();
}
UINT qwertyKeyboard::sendOutputs()
{
    UINT ret = KB_OUTPUTNOCHANGE;
    UINT tempRet = KB_OUTPUTNOCHANGE;
    for(int i = 0; i < QK_NUM_KEYS_TOP; i++) {
        if(tempRet = TFX->handleKeyOutput(topKeys[i]))
           ret = tempRet;
    }
    for(int i = 0; i < QK_NUM_KEYS_MID; i++) {
        if(tempRet = TFX->handleKeyOutput(middleKeys[i]))
            ret = tempRet;
    }
    for(int i = 0; i < QK_NUM_KEYS_BOT; i++) {
        if(tempRet = TFX->handleKeyOutput(bottomKeys[i]))
            ret = tempRet;
    }
    for(int i = 0; i < NUM_KEYS_SPACE; i++) {
        if(tempRet = TFX->handleKeyOutput(spaceKeys[i]))
            ret = tempRet;
    }
    TFX->handleToggleKeyOutput(altKey);
    TFX->handleToggleKeyOutput(ctrlKey);

    int shiftVal;
    if((shiftVal = TFX->handleToggleKeyOutput(shiftKey)) > 0)
        setCaps();
    else if(shiftVal < 0)
        setNoCaps();
/*
    if(ret == KP_ASCII || ret == KP_UNICODE)
        ret = KB_OUTPUTASCIIUNICODE;
    else if(ret == KP_CHANGEMODE)
        ret = KB_OUTPUTNUMERIC;
*/
    /* NOTE!!! The code for ret is incomplete */
    return ret;
}

void qwertyKeyboard::setCaps()
{
    string ucTop[QK_NUM_KEYS_TOP]      = {"Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P"};
    string ucMiddle[QK_NUM_KEYS_MID]   = {"A", "S", "D", "F", "G", "H", "J", "K", "L"};
    string ucBottom[QK_NUM_KEYS_BOT]   = {"Z", "X", "C", "V", "B", "N", "M", ".", "BKSP"};

    copy(&ucTop[0], &ucTop[QK_NUM_KEYS_TOP], topIDs);
    copy(&ucMiddle[0], &ucMiddle[QK_NUM_KEYS_MID], middleIDs);
    copy(&ucBottom[0], &ucBottom[QK_NUM_KEYS_BOT], bottomIDs);

    refreshKeyIDs();

    shiftKey.setKeyOut(true);
}

void qwertyKeyboard::setNoCaps()
{
    string lcTop[QK_NUM_KEYS_TOP]      = {"q", "w", "e", "r", "t", "y", "u", "i", "o", "p"};
    string lcMiddle[QK_NUM_KEYS_MID]   = {"a", "s", "d", "f", "g", "h", "j", "k", "l"};
    string lcBottom[QK_NUM_KEYS_BOT]   = {"z", "x", "c", "v", "b", "n", "m", ".", "BKSP"};

    copy(&lcTop[0], &lcTop[QK_NUM_KEYS_TOP], topIDs);
    copy(&lcMiddle[0], &lcMiddle[QK_NUM_KEYS_MID], middleIDs);
    copy(&lcBottom[0], &lcBottom[QK_NUM_KEYS_BOT], bottomIDs);

    refreshKeyIDs();
    shiftKey.setKeyOut(false);
}

void qwertyKeyboard::refreshKeyIDs()
{
    for(int i = 0; i < QK_NUM_KEYS_TOP; i++)
        topKeys[i].setKeyID(topIDs[i]);
    for(int i = 0; i < QK_NUM_KEYS_MID; i++)
        middleKeys[i].setKeyID(middleIDs[i]);
    for(int i = 0; i < QK_NUM_KEYS_BOT; i++)
        bottomKeys[i].setKeyID(bottomIDs[i]);
}

void qwertyKeyboard::refreshPermanentKeyIDs()
{
    try {
        if(NUM_KEYS_SPACE != 4)
            throw "Dimension Mismatch in Space Array";
        spaceKeys[0].setKeyID("@123");
        spaceKeys[1].setKeyID(",");
        spaceKeys[2].setKeyID(" ");
        spaceKeys[3].setKeyID("ENTER");
    } catch (const char* c) {
        MessageBox(NULL, c, "Error", MB_ICONERROR | MB_OK);
    }

    shiftKey.setKeyID("SHIFT");
    altKey.setKeyID("alt");
    ctrlKey.setKeyID("ctrl");
}
