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

#include "ofMain.h"
#include "buttonRect.h"
#include "ofxTextSuite.h"
#include "roundedRect.h"
#include <string>

#pragma once

/**
 * @class key key.h "keyboard/key.h"
 * Creates a key/button that can be pressed by the user
 * or programmatically. Holding the cursor over the key
 * for a certain amount of time pressed the key and generates
 * an output that is retrieved by the program with
 * getKeyOut(bool removeState). When the user is hovering over
 * the key, the key automatically draws the highlight state/color.
 * When the users clicks the key, the key draws the registered state/color
 * briefly to signify that the user has clicked the key.
 */
class key : protected buttonRect
{
    public:
        key();
        key(ofTrueTypeFont *fontToLoad);
        virtual ~key();

        void update(float _eyeX, float _eyeY); //update the position of the "eyecursor"
        void drawKey(); //draws key including highlighting if required

        /**
         *
         * @param removeState Sets "key pressed state"
         * to false if removeState is true, else "key pressed state" remains true.
         * @return Returns true if the key has been "pressed"?
         */
        bool getKeyOut(bool removeState = true);

        /**
         * Set the key output state programmatically.
         * @param out The new output state.
         */
        void setKeyOut(bool out) { drawReg = output = out; }

        /**
         * Set the color for the default key background.
         * @param r The red channel. A value between 0 and 255.
         * @param g The green channel. A value between 0 and 255.
         * @param b The blue channel. A value between 0 and 255.
         * @param a The alpha channel. A value between 0 and 255.
         * @return Returns false if function failed. Currently returns false only if
         * invalid paramenters are passed.
         */
        bool setKeyColor(int r, int g, int b, int a = 255);

        /**
         * Set the color for the highlighted key background.
         * @param r The red channel. A value between 0 and 255.
         * @param g The green channel. A value between 0 and 255.
         * @param b The blue channel. A value between 0 and 255.
         * @return Returns false if function failed. Currently returns false only if
         * invalid paramenters are passed.
         */
        bool setHighlightColor(int r, int g, int b);

        /**
         * Set the color for the registered key background.
         * @param r The red channel. A value between 0 and 255.
         * @param g The green channel. A value between 0 and 255.
         * @param b The blue channel. A value between 0 and 255.
         * @param a The alpha channel. A value between 0 and 255.
         * @return Returns false if function failed. Currently returns false only if
         * invalid paramenters are passed.
         */
        bool setRegisteredColor(int r, int g, int b, int a = 255);

        /**
         * Set the x-coordinate of the key.
         * @param posX The x-coordinate in pixels
         */
        void    setX(float posX)        {   x = posX;   }
        /**
         * Get the x-coordinate of the key.
         * @return The current x-coordinate in pixels
         */
        float   getX()                  {   return x;   }

        /**
         * Set the y-coordinate of the key.
         * @param posY The y-coordinate in pixels
         */
        void    setY(float posY)        {   y = posY;   }

        /**
         * Get the y-coordinate of the key.
         * @return The current y-coordinate in pixels
         */
        float   getY()                  {   return y;   }

        /**
         * Set the position of the key.
         * @param posX The x-coordinate in pixels.
         * @param posY The y-coordinate in pixels.
         */
        void    setPos(float posX, float posY) { x = posX; y = posY;}

        /**
         * Set the width of the key.
         * @param width The width in pixels.
         */
        void    setWidth(float width)   {   w = width;  }
        /**
         * Get the width of the key.
         * @return The width in pixels.
         */
        float   getWidth()              {   return w;   }

        /**
         * Set the height of the key.
         * @param height The height in pixels.
         */
        void    setHeight(float height) {   h = height; }
        /**
         * Get the height of the key.
         * @return The height in pixels.
         */
        float   getHeight()             {   return h;   }

        /**
         * Set the radius of the arcs forming the corners of the key.
         * @param radius The radius in pixels
         */
        void    setRadius(float radius) {   r = radius; }
        /**
         * Get the radius of the arcs forming the corners of the key.
         * @return The radius in pixels
         */
        float   getRadius()             {   return r;   }

        /**
         * Set the dimensions of the key.
         * @param posX The x-coordinate in pixels.
         * @param posY The y-coordinate in pixels.
         * @param width The width in pixels.
         * @param height The height in pixels.
         * @param radius The radius of the arcs forming the corners of the key in pixels.
         */
        void    setDimensions(int posX, int posY, int width, int height, int radius = 5) { x = posX; y = posY; w = width; h = height; r = radius; }

        /**
         * Set the font size for the key label.
         * @param size The font size.
         */
        void    setFontSize(int size);

        /**
         * Set the pointer containing the font to use for the key.
         * If multiple keys are being rendered, by default each key has its
         * own font. To save memory, it is recommended to create one font object
         * and set the font pointer of all keys to point to that font object.
         * It is optimal to set the font pointer in the constructor and use this function
         * to update the font pointer if need be. However, it is possible to not set the font
         * pointer in the constructor and set one here. This is much slower (O(N) slower) and not recommended.
         * @param fontPtr The font object.
         */
        void    setFontPtr(ofTrueTypeFont *fontPtr);

        /**
         * Set the ID of the key. By default the key id is displayed
         * on the label of the key. It is also used for identifying the key.
         * @param id A string containing the desired ID for the key
         */
        void    setKeyID(string id) { keyID = id; }

        /**
         * Get the ID of the key.
         * @return A string containing the key ID.
         */
        string  getKeyID()          { return keyID; }

        /**
         * Sets how long it takes for a key to be triggered.
         * @param clickTime The amount of time in milliseconds a user must
         * hover over a key before it clicks.
         */
        static void setKeyPressTime(int keyPressTime) { maxCount = keyPressTime; }

    protected:
        /* listed in  the order that they are drawn or called */

        /** resize anything that needs to be resized (or otherwise initiliazed) here */
        virtual void resizeObjects() {}
        /** draws the background of the key */
        virtual void drawKeyBackground();
        /**
         * Draws whether the key has been "pressed" (registered)
         * @param timeSinceRegMillis The amount of time since the key was first pressed
         * @return Whether to continue drawing.
         */
        virtual bool drawKeyRegistered(float timeSinceRegMillis);
        /**
         * Draws a rect indicating that the mouse is over the key
         * The rect in this function is set to fade in
         * @param alpha Alpha from 0-255 for rect (or other shape),
         * updated automatically by the key process
         */
        virtual void drawKeyHighlight(float alpha);
        /**
         * Draws the label for the key (usually the letter of the key
         * or some type of graphic indicating the key's purpose
         */
        virtual void drawKeyLabel(); //or foreground

        /**
         * What to do when getKeyOut(true) is called. The default calls for setting the key to
         * "not pressed" when getKeyOut() is called
         * @param *output A pointer containing info as to whether the key is "pressed"
         * @param *drawHighlight A pointer containing info as to whether drawKeyHighlight(float alpha)
         * @param removeState the removeState as passed to getKeyOut()
         * should be executed
         */
        virtual void keyOutProcess(bool *output, bool *drawHighlight, bool removeState); //allows implementing a toggle

        /**
         * What to do when getKeyOut(false) is called. By default nothing happens.
         * @param removeState The removeState as passed to getKeyOut()
         */
        virtual void keyNoOutProcess(bool removeState) {}

        float   w;
        float   h;
        float   r;

        /* The X and Y coordinates of the cursor  */
        float   eyeX;
        float   eyeY;

        int     keyColor[4];
        int     highlightColor[3]; //cannot hold alpha
        int     registeredColor[4];

        bool    output;

    private:
        void initKey();
        void mouseOverKey();    //set ALPHA and other variables inside of key, use key highlight to draw the key
        void mouseLeaveKey();   //set ALPHA and other variables, allows fade out

        float   counter;
        bool    setCounterStart;
        int     counterStart;   //time in milliseconds
        static int maxCount;

        bool    drawHighlight;

        int     regCounter;     //the registered rect must display for a certain amount of time
        int     regCounterStart;
        bool    drawReg;
 #define REGISTER_DISPLAY_TIME_MILLIS 100

        float   alpha;

        string  keyID;
        ofTrueTypeFont *font;
        bool initFontByPtr;
        void initFont();
};

