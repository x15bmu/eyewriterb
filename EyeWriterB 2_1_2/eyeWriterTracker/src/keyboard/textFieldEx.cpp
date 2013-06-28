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

#include "textFieldEx.h"

#include "key.h"
#include <string>
#include <windows.h>
#include <locale>

textFieldEx::textFieldEx() :
    textString(""), storedStandAloneStr(""),
    oldStringLength(0), mode(TFX_STANDALONE),
    returnState(false), imageLoaded(false)
{
    tBlock.init("fonts/arial.ttf", 14);

    boxKey.setKeyBackgroundImage("images/keyboard/backgroundBk.png");
    boxKey.setKeyHighlightImage("images/keyboard/highlightBk.png");
    boxKey.setKeyRegisteredImage("images/keyboard/registeredBk.png");
    boxKey.noForeground();

    setTFXMode(TFX_STANDALONE);

    boxKey.setKeyID("clear");
    boxKey.setHighlightColor(179, 27, 27);
}

textFieldEx::~textFieldEx()
{

}

UINT textFieldEx::handleKeyOutput(key &k)
{
    UINT ret = 0;
    if(k.getKeyOut())
    {
        INPUT kb = { 0 };
        kb.type = INPUT_KEYBOARD;

        string keyID = k.getKeyID();

        if(keyID == " " && textString.length() != 0 && textString.substr(textString.length()-1, textString.length()) == " ")
        {
            textString = textString.substr(0, textString.length() - 1) + ". ";

            kb.ki.wVk = VK_BACK;
            SendInput(1, &kb, sizeof(INPUT));
            kb.ki.dwFlags = KEYEVENTF_KEYUP;
            SendInput(1, &kb, sizeof(INPUT));

            kb.ki.wVk = VK_OEM_PERIOD;
            kb.ki.dwFlags = 0;
            SendInput(1, &kb, sizeof(INPUT));
            kb.ki.dwFlags = KEYEVENTF_KEYUP;
            SendInput(1, &kb, sizeof(INPUT));

            kb.ki.wVk = VK_SPACE;
            kb.ki.dwFlags = 0;
            SendInput(1, &kb, sizeof(INPUT));
            kb.ki.dwFlags = KEYEVENTF_KEYUP;
            SendInput(1, &kb, sizeof(INPUT));
            return KP_DOUBLESPACEPERIOD;
        }
        else if(keyID == "@123" || keyID == "ABC") {
            return KP_CHANGEMODE;
        }
        else if(keyID == "BKSP") {
            kb.ki.wVk = VK_BACK;
            if(textString.length() != 0)
                textString = textString.substr(0, textString.length() - 1);
            ret = KP_BKSP;
        }
        else if(keyID == "ENTER") {
            kb.ki.wVk = VK_RETURN;
            textString += "\n";
            ret = KP_ENTER;
        }
        else if(keyID == "DEGREE") {
            kb.ki.wScan = '°'; //U+00B0
            kb.ki.dwFlags = KEYEVENTF_UNICODE;
            textString += "°";
            ret = KP_UNICODE;
        }
        else if(keyID == "EURO") {
            kb.ki.wScan = '€'; //U+20AC
            kb.ki.dwFlags = KEYEVENTF_UNICODE;
            textString += "€";
            ret = KP_UNICODE;
        }
        else if(keyID == "POUND_STERLING") {
            kb.ki.wScan = '£'; //U+00A3
            kb.ki.dwFlags = KEYEVENTF_UNICODE;
            textString += "£";
            ret = KP_UNICODE;
        }
        else if(keyID == "TAB") {
            kb.ki.wVk = VK_TAB;
            textString += "\t";
            ret = KP_TAB;
        }
        else if(keyID == "UP_ARROW") {
            kb.ki.wVk = VK_UP;
            ret = KP_UPARROW;
        }
        else if(keyID == "DOT") {
            kb.ki.wScan = '·'; //U+00B7
            kb.ki.dwFlags = KEYEVENTF_UNICODE;
            textString += "·";
            ret = KP_UNICODE;
        }
        else if(keyID == "LEFT_ARROW") {
            kb.ki.wVk = VK_LEFT;
            ret = KP_LEFTARROW;
        }
        else if(keyID == "DOWN_ARROW") {
            kb.ki.wVk = VK_DOWN;
            ret = KP_DOWNARROW;
        }
        else if(keyID == "RIGHT_ARROW") {
            kb.ki.wVk = VK_RIGHT;
            ret = KP_RIGHTARROW;
        }
        else if(keyID == ".") {
            kb.ki.wVk = VK_OEM_PERIOD;
            textString += '.';
            ret = KP_PERIOD;
        }
        else {
            convertStringToOutput(keyID.c_str());
            textString += keyID;
            return KP_ASCII;
        }
        if(mode == TFX_GLOBALMOUSE) {
            if(ret != KP_UNICODE) //unicode cannot send a key down signal
                SendInput(1, &kb, sizeof(INPUT));
            kb.ki.dwFlags |= KEYEVENTF_KEYUP;
            SendInput(1, &kb, sizeof(INPUT));
        }
        return ret;
    }
    return KP_NOKEY;
}

int textFieldEx::handleToggleKeyOutput(toggleKey &k)
{
    int ret = KP_NOKEY;
    if(k.getPrevOutput() != k.getKeyOut(false))
    {
        INPUT kb = { 0 };
        kb.type = INPUT_KEYBOARD;
        if(k.getKeyID() == "SHIFT")
        {
            kb.ki.wVk = VK_SHIFT;
            ret = KP_SHIFT;
        }
        else if(k.getKeyID() == "ctrl")
        {
            kb.ki.wVk = VK_CONTROL;
            ret = KP_CTRL;
        }
        else if(k.getKeyID() == "alt")
        {
            kb.ki.wVk = VK_MENU;
            ret = KP_ALT;
        }
        else if(k.getKeyID() == "More")
        {
            //ret *= k.getKeyOut();
            ret = KP_MORE;
            //return KP_MORE;
        }
        else
            return KP_ERROR;

        if(k.getKeyOut() == false)
        {
            kb.ki.dwFlags = KEYEVENTF_KEYUP;
            ret *= -1;
        }
        if(mode == TFX_GLOBALMOUSE) {
            SendInput(1, &kb, sizeof(INPUT));
        }
        return ret;
    }
    return KP_NOKEY;
}

void textFieldEx::appendPKeyToString(key& pKey)
{
    //printf(pKey.getKeyID().c_str());

    if(pKey.getKeyID() == "")
        return; //prevent the program from crashing
    int position = textString.rfind(" ");
    /*
     * check to ensure:
        1. there is a space
        2. the space is not the last character
        3. the first character of the last word is the same as the first character of the suggestion (caps or not)
     * If any of these fail, they are resolved by the else ifs in order
     */
    if( position != string::npos
        && position != textString.length()-1
        && (textString[position + 1] == pKey.getKeyID()[0]
           || textString[position + 1] == toupper(pKey.getKeyID()[0])) )
    {
        for(int i = 0; i < textString.length()-(position+2); i++)
            convertStringToOutput("\b");
        convertStringToOutput(pKey.getKeyID().substr(1).c_str());
        convertStringToOutput(" ");

        textString = textString.substr(0, position + 2) + pKey.getKeyID().substr(1, pKey.getKeyID().length()) + " ";
        //add 2 to keep the space and preserve capitalization and add a space after the word
    }
    else if (position == string::npos)
    {
        for(int i = 0; i < textString.length() - 1; i++)
            convertStringToOutput("\b");
        convertStringToOutput(pKey.getKeyID().substr(1).c_str());
        convertStringToOutput(" ");

        textString = textString.substr(0, 1) + pKey.getKeyID().substr(1, pKey.getKeyID().length()) + " ";
    }
    else if(position == textString.length()-1)
    {
        convertStringToOutput(pKey.getKeyID().c_str());
        convertStringToOutput(" ");
        textString += pKey.getKeyID() + " ";
    }
    else if (textString[position + 1] != pKey.getKeyID()[0])
    {
        for(int i = 0; i < textString.length() - (position + 1); i++)
            convertStringToOutput("\b");
        convertStringToOutput(pKey.getKeyID().c_str());
        convertStringToOutput(" ");
        textString = textString.substr(0, position + 1) + pKey.getKeyID().substr(0, pKey.getKeyID().length()) + " ";
    }
}

void textFieldEx::convertStringToOutput(const char* c)
{
    if(mode != TFX_GLOBALMOUSE)
        return;
    INPUT input = {0};
    input.type = INPUT_KEYBOARD;

    int repeats = strlen(c);
    for (int i = 0; i < repeats; i++)
        printf("%c", c[i]);

    for(int i = 0; i < strlen(c); i++)
    {
        bool shift = HIBYTE(VkKeyScan(c[i])) & 1; //a hibyte 1 represents shift pressed
        if(shift)
        {
            input.ki.wVk = VK_SHIFT;
            input.ki.dwFlags = 0;
            SendInput(1, &input, sizeof(INPUT));
        }

        input.ki.wVk = LOBYTE(VkKeyScan(c[i]));
        input.ki.dwFlags = 0;
        SendInput(1, &input, sizeof(INPUT));
        input.ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(1, &input, sizeof(INPUT));

        if(shift)
        {
            input.ki.wVk = VK_SHIFT;
            input.ki.dwFlags = KEYEVENTF_KEYUP;
            SendInput(1, &input, sizeof(INPUT));
        }
    }
}

void textFieldEx::update(int cursorX, int cursorY, int x, int y, int w, int h)
{
    fieldSize.x = x;
    fieldSize.y = y;
    fieldSize.w = w;
    fieldSize.h = h;

    boxKey.setWidth(ofGetWidth() / 15);
    boxKey.setHeight(h + 2); //+1 removes white pixels
    boxKey.setX(fieldSize.x + fieldSize.w - boxKey.getWidth() + 2); //+2 removes white pixel in top right
    boxKey.setY(y - 2);
    //boxKey.setRadius(5);
    boxKey.update(cursorX, cursorY);
    checkClear();
}

void textFieldEx::draw()
{
//draw text box--------------------------------------------------------
    ofFill();
    ofSetColor(255,255,255);
    ofRect(fieldSize.x, fieldSize.y, fieldSize.w, fieldSize.h);
    ofNoFill();
    ofSetColor(0,0,0);
    ofRect(fieldSize.x, fieldSize.y, fieldSize.w, fieldSize.h);

    ofFill();//set back to fill

//draw "clear textbox" button------------------------------------------

    boxKey.drawKey();
    if(imageLoaded)
    {
        const int stdBoxWidth = 76;
        const int stdBoxHeight = 94;
        float sizeMultiplier = min(boxKey.getWidth()/stdBoxWidth, boxKey.getHeight()/stdBoxHeight);
        int w = sizeMultiplier * normImageWidth;
        int h = sizeMultiplier * normImageHeight;
        keyImage.draw(boxKey.getX() + boxKey.getWidth()/2 - w/2,
                      boxKey.getY() + boxKey.getHeight()/2 - h/2,
                      w, h);
    }


//draw the textbox-----------------------------------------------------
    drawCursor();

    strToDraw = textString + cursor;

    tBlock.words.size();
    tBlock.setText(strToDraw);
    if(cursorOn)
    {
        tBlock.wrapTextX(ofGetWidth() - 15 - boxKey.getWidth());
    }
    else
    {
        tBlock.wrapTextX(ofGetWidth() - 20 - boxKey.getWidth());
    }
    tBlock.setColor(0,0,0,255);
    tBlock.draw(fieldSize.x, fieldSize.y);
}

void textFieldEx::drawCursor()
{
    if((int)ofGetElapsedTimef() % 2 == 1)
    {
        cursor = "|";
        cursorOn = true;
    }
    else
    {
        cursor = " ";
        cursorOn = false;
    }
}

void textFieldEx::checkClear()
{
    if(boxKey.getKeyOut())
    {
        if(mode == TFX_STANDALONE)
            textString.clear();
        else if(mode == TFX_GLOBALMOUSE)
            returnState = true;
    }
}

void textFieldEx::displayString()
{
    ofSetColor(0,0,0);
    ofDrawBitmapString(textString, 20, 20);
}

void textFieldEx::setTFXMode(TFXMODE requestedMode)
{
    ///////////////////////////////////////////////////////////////////
    // Store or restore string as necessary
    ///////////////////////////////////////////////////////////////////
    if(requestedMode == TFX_GLOBALMOUSE && mode == TFX_STANDALONE) {
        /* Store string and clear field */
        storedStandAloneStr = textString;
        textString.clear();
    } else if(requestedMode == TFX_STANDALONE && mode == TFX_GLOBALMOUSE) {
        /* Restore string */
        textString = storedStandAloneStr;
    }

    mode = requestedMode;

    ///////////////////////////////////////////////////////////////////
    // Convert key in textbox
    ///////////////////////////////////////////////////////////////////
    if(mode == TFX_STANDALONE) {
        boxKey.setKeyID("clear");
        imageLoaded = keyImage.loadImage("images/keyboard/clear.png");
    }
    else if(mode == TFX_GLOBALMOUSE) {
        boxKey.setKeyID("Go\nBack");
        imageLoaded = keyImage.loadImage("images/keyboard/GoBack.png");
    }

    normImageWidth = keyImage.getWidth() * 1.0/1.5; //object stored 2x size of actual for higher res
    normImageHeight = keyImage.getHeight() * 1.0/1.5;
    if(!imageLoaded)
        boxKey.useKeyID();
}
bool textFieldEx::getReturnState(bool remove /* = true */)
{
    bool ret = returnState;
    if(remove)
        returnState = false;
    return ret;
}

