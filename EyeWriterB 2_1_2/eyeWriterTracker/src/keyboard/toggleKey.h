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

#ifndef TOGGLEKEY_H
#define TOGGLEKEY_H

#include "key.h"

/**
 * @class toggleKey toggleKey.h "keyboard/toggleKey.h"
 * @brief A \ref key that toggles on or off rather than
 * registering a single click
 */
class toggleKey : virtual public key
{
public:
    /** Default constructor */
    toggleKey();
    virtual ~toggleKey() {};

    /**
     * Toggle the key.
     *
     * If the key is off, calling this function will toggle the key on.
     * If the key is on, calling this function will toggle the key off.
     */
    void toggle() { output ^= 1;}

    /**
     * Toggle the key on.
     */
    void toggleOn() { output = true;}

    /**
     * Toggle the key off.
     */
    void toggleOff() { output = false;}

    /**
     * Get the previous output of the key
     * @return The most recent value obtained from key::getOutput(bool removeState)
     */
    bool getPrevOutput() { return prevOutput; }
protected:
    virtual void keyOutProcess(bool *output, bool *drawHighlight, bool removeState);
    virtual bool drawKeyRegistered(float timeSinceRegMillis);
    virtual void keyNoOutProcess(bool removeState);
private:
    bool prevOutput;
};


#endif // TOGGLEKEY_H
