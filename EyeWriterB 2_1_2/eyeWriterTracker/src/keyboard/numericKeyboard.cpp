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

#include "numericKeyboard.h"

using std::string;
using std::copy;

numericKeyboard::numericKeyboard(ofImage *background, ofImage *registered, ofImage *highlight,
                                 ofImage *backgroundSm, ofImage *registeredSm, ofImage *highlightSm,
                                 ofImage *backgroundSp, ofImage *registeredSp, ofImage *highlightSp,
                                 textFieldEx *_TFX, ofTrueTypeFont *font, ofTrueTypeFont *fontSm) :
    moreKey(background, highlight, registered, NULL),
    ctrlKey(backgroundSm, highlight, registered, NULL),
    altKey(backgroundSm, highlight, registered, NULL)
{
    ///////////////////////////////////////////////////////////////////
    // Set key resources (images and fonts)
    ///////////////////////////////////////////////////////////////////
    for(int i = 0; i < NK_NUM_KEYS_TOP; i++) {
        topKeys[i].setKeyBackgroundImage(background);
        topKeys[i].setKeyRegisteredImage(registered);
        topKeys[i].setKeyHighlightImage(highlight);
        topKeys[i].setFontPtr(font);
    }
    for(int i = 0; i < NK_NUM_KEYS_MID; i++) {
        middleKeys[i].setKeyBackgroundImage(background);
        middleKeys[i].setKeyRegisteredImage(registered);
        middleKeys[i].setKeyHighlightImage(highlight);
        middleKeys[i].setFontPtr(font);
    }
    for(int i = 0; i < NK_NUM_KEYS_BOT; i++) {
        bottomKeys[i].setKeyBackgroundImage(background);
        bottomKeys[i].setKeyRegisteredImage(registered);
        bottomKeys[i].setKeyHighlightImage(highlight);
        bottomKeys[i].setFontPtr(font);
    }
    for(int i = 0; i < NUM_KEYS_SPACE; i++) {
        if(i == KB_SPACE) {
            spaceKeys[i].setKeyBackgroundImage(backgroundSp);
            spaceKeys[i].setKeyRegisteredImage(registeredSp);
            spaceKeys[i].setKeyHighlightImage(highlightSp);
            spaceKeys[i].setFontPtr(fontSm);
        }
        else {
            spaceKeys[i].setKeyBackgroundImage(backgroundSm);
            spaceKeys[i].setKeyRegisteredImage(registeredSm);
            spaceKeys[i].setKeyHighlightImage(highlightSm);
            spaceKeys[i].setFontPtr(fontSm);
        }
    }

    ///////////////////////////////////////////////////////////////////
    // Set special key resources
    ///////////////////////////////////////////////////////////////////
    bottomKeys[8].setKeyForegroundImage("images/keyboard/UNI_BOT_8.png"); //BKSP : universal set, bottom row, 9th key (more key is not bottom row)
    spaceKeys[KB_ENTER].setKeyForegroundImage("images/keyboard/UNI_SPA_5.png"); // ENTER : universal set, space row, 6th key

    altKey.setFontPtr(fontSm);
    ctrlKey.setFontPtr(fontSm);

    spaceKeys[KB_SYMBOL].setKeyID("ABC");
    refreshPermanentKeyIDs();

    ks = KS_NONE;
    TFX = _TFX;
}


numericKeyboard::~numericKeyboard()
{
    //dtor
}

void numericKeyboard::useStandardKeys()
{
    ///////////////////////////////////////////////////////////////////
    // Don't use key images.
    ///////////////////////////////////////////////////////////////////
    for(int i = 0; i < NK_NUM_KEYS_TOP; i++)
        topKeys[i].useStandardKey();
    for(int i = 0; i < NK_NUM_KEYS_MID; i++)
        middleKeys[i].useStandardKey();
    for(int i = 0; i < NK_NUM_KEYS_BOT; i++)
        bottomKeys[i].useStandardKey();
    for(int i = 0; i < NUM_KEYS_SPACE; i++)
        spaceKeys[i].useStandardKey();
    moreKey.useStandardKey();
    altKey.useStandardKey();
    ctrlKey.useStandardKey();
}

void numericKeyboard::update(int cursorX, int cursorY, bool changeParams)
{
    updateParams(topKeys, NK_NUM_KEYS_TOP, middleKeys, NK_NUM_KEYS_MID, bottomKeys, NK_NUM_KEYS_BOT,
                 KB_NORMALHORIZGAP, KB_NORMALHORIZGAP, KB_NORMALHORIZGAP + KB_KEYSIZE + KB_KEYGAP, cursorX, cursorY, changeParams);
    updateParams(moreKey, calcDistFromCenter(getKeybdInfo().left),
                 KB_NORMSCREENHEIGHT - KB_OUTERGAPFROMBOTTOM - KB_KEYSIZE,
                 cursorX, cursorY, changeParams, KB_SPACEFONTSIZE);
    updateParams(altKey, calcDistFromCenter(getKeybdInfo().right) - 3*KB_KEYSIZE - 2*KB_KEYGAP,
                 KB_NORMSCREENHEIGHT - KB_OUTERGAPFROMBOTTOMTOSPACE - KB_KEYHEIGHTBOTTOMROW,
                 cursorX, cursorY, changeParams, KB_SPACEFONTSIZE, KB_KEYSIZE, KB_KEYHEIGHTBOTTOMROW);

    updateParams(ctrlKey, calcDistFromCenter(getKeybdInfo().right) - 2*KB_KEYSIZE - KB_KEYGAP,
                 KB_NORMSCREENHEIGHT - KB_OUTERGAPFROMBOTTOMTOSPACE - KB_KEYHEIGHTBOTTOMROW,
                 cursorX, cursorY, changeParams, KB_SPACEFONTSIZE, KB_KEYSIZE, KB_KEYHEIGHTBOTTOMROW);
    if(!updateSpaceKeyParams(spaceKeys, NUM_KEYS_SPACE, cursorX, cursorY, changeParams))
        MessageBox(NULL, "Error updating space-row parameters.", "Error", MB_ICONERROR | MB_OK);
}

void numericKeyboard::draw()
{
    drawKeys(topKeys, NK_NUM_KEYS_TOP, middleKeys, NK_NUM_KEYS_MID, bottomKeys, NK_NUM_KEYS_BOT, spaceKeys, NUM_KEYS_SPACE);
    moreKey.drawKey();
    altKey.drawKey();
    ctrlKey.drawKey();
}

void numericKeyboard::setNumeric()
{
    string topIDsNumeric[NK_NUM_KEYS_TOP] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"};
    string middleIDsNumeric[NK_NUM_KEYS_MID] = {"@", "#", "$", "%", "&", "*", "(", ")", "-", "+"};
    string bottomIDsNumeric[NK_NUM_KEYS_BOT] = {"!", ";", ":", "'", "\"", "?", "/", ".", "BKSP"};

    copy(&topIDsNumeric[0], &topIDsNumeric[NK_NUM_KEYS_TOP], topIDs);
    copy(&middleIDsNumeric[0], &middleIDsNumeric[NK_NUM_KEYS_MID], middleIDs);
    copy(&bottomIDsNumeric[0], &bottomIDsNumeric[NK_NUM_KEYS_BOT], bottomIDs);
    refreshKeyIDs(true);

    ks = KS_NUMERIC;
}

void numericKeyboard::setSymbol()
{
    string topIDsSymbol[NK_NUM_KEYS_TOP] = {"~", "`", "/", "|", "^", "DEGREE", "_", "=", "EURO", "POUND_STERLING"};
    string middleIDsSymbol[NK_NUM_KEYS_MID] = {"TAB", "{", "}", "[", "]", "<", ">", "UP_ARROW", "¢", "DOT"};
    string bottomIDsSymbol[NK_NUM_KEYS_BOT] = {"!", ";", ":", "'", "\"", "LEFT_ARROW", "DOWN_ARROW", "RIGHT_ARROW", "BKSP"};

    copy(&topIDsSymbol[0], &topIDsSymbol[NK_NUM_KEYS_TOP], topIDs);
    copy(&middleIDsSymbol[0], &middleIDsSymbol[NK_NUM_KEYS_MID], middleIDs);
    copy(&bottomIDsSymbol[0], &bottomIDsSymbol[NK_NUM_KEYS_BOT], bottomIDs);
    refreshKeyIDs();

    //TODO: error checking in case images do not load
    topKeys[5].setKeyForegroundImage("images/keyboard/SYM_TOP_5.png"); //degree
    topKeys[8].setKeyForegroundImage("images/keyboard/SYM_TOP_8.png"); //euro
    topKeys[9].setKeyForegroundImage("images/keyboard/SYM_TOP_9.png"); //pound
    middleKeys[0].setKeyForegroundImage("images/keyboard/SYM_MID_0.png"); //tab
    middleKeys[8].setKeyForegroundImage("images/keyboard/SYM_MID_8.png"); //cent
    middleKeys[9].setKeyForegroundImage("images/keyboard/SYM_MID_9.png"); //dot

    middleKeys[7].setKeyForegroundImage("images/keyboard/SYM_MID_7.png"); //up arrow
    bottomKeys[5].setKeyForegroundImage("images/keyboard/SYM_BOT_6.png"); //left arrow
    bottomKeys[6].setKeyForegroundImage("images/keyboard/SYM_BOT_7.png"); //down arrow
    bottomKeys[7].setKeyForegroundImage("images/keyboard/SYM_BOT_8.png"); //right arrow

    ks = KS_SYMBOL;
}

void numericKeyboard::refreshKeyIDs(bool useKeyID /* = false */)
{
   for(int i = 0; i < NK_NUM_KEYS_TOP; i++)
   {
       topKeys[i].setKeyID(topIDs[i]);
       if(useKeyID) topKeys[i].useKeyID();
   }
   for(int i = 0; i < NK_NUM_KEYS_MID; i++)
   {
       middleKeys[i].setKeyID(middleIDs[i]);
       if(useKeyID) middleKeys[i].useKeyID();
   }
   for(int i = 0; i < NK_NUM_KEYS_BOT; i++)
   {
       bottomKeys[i].setKeyID(bottomIDs[i]);
       if(useKeyID && i != NK_NUM_KEYS_BOT - 1) bottomKeys[i].useKeyID();
   }
}

void numericKeyboard::refreshPermanentKeyIDs()
{
    try
    {
        if(NUM_KEYS_SPACE != 4)
            throw "Dimension Mismatch in Space Array";
        spaceKeys[0].setKeyID("ABC");
        spaceKeys[1].setKeyID(",");
        spaceKeys[2].setKeyID(" ");
        spaceKeys[3].setKeyID("ENTER");
    }
    catch (const char* c)
    {
        MessageBox(NULL, c, "Error", MB_ICONERROR | MB_OK);
    }
    moreKey.setKeyID("More");
    altKey.setKeyID("alt");
    ctrlKey.setKeyID("ctrl");
}

UINT numericKeyboard::sendOutputs()
{
    UINT ret = KB_OUTPUTNOCHANGE;
    UINT tempRet = KB_OUTPUTNOCHANGE;
    for(int i = 0; i < NK_NUM_KEYS_TOP; i++)
    {
        if(tempRet = TFX->handleKeyOutput(topKeys[i]))
           ret = tempRet;
    }
    for(int i = 0; i < NK_NUM_KEYS_MID; i++)
    {
        if(tempRet = TFX->handleKeyOutput(middleKeys[i]))
            ret = tempRet;
    }
    for(int i = 0; i < NK_NUM_KEYS_BOT; i++)
    {
        if(tempRet = TFX->handleKeyOutput(bottomKeys[i]))
            ret = tempRet;
    }
    for(int i = 0; i < NUM_KEYS_SPACE; i++)
    {
        if(tempRet = TFX->handleKeyOutput(spaceKeys[i]))
            ret = tempRet;
    }
    TFX->handleToggleKeyOutput(altKey);
    TFX->handleToggleKeyOutput(ctrlKey);

    int moreVal;
    if((moreVal = TFX->handleToggleKeyOutput(moreKey)) > 0)
        setSymbol();
    else if(moreVal < 0)
        setNumeric();
/*
    if(ret == KP_ASCII || ret == KP_UNICODE)
        ret = KB_OUTPUTASCIIUNICODE;
    else if(ret == KP_CHANGEMODE)
        ret = KB_OUTPUTQWERTY;
*/
    /* NOTE!! The code for ret is incomplete */
    return ret;
}
