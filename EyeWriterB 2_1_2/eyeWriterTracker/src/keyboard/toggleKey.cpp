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

#include "toggleKey.h"

toggleKey::toggleKey() : key(), prevOutput(false)
{

}

void toggleKey::keyOutProcess(bool *output, bool *drawHighlight, bool removeState)
{
    *drawHighlight = true;
    if(removeState)
        prevOutput = true;
}

bool toggleKey::drawKeyRegistered(float timeSinceRegMillis)
{
    ofSetColor(registeredColor[0], registeredColor[1], registeredColor[2], registeredColor[3]);
    roundedRect(x, y, w, h ,r);

    return output;
}

void toggleKey::keyNoOutProcess(bool removeState)
{
    if(removeState)
        prevOutput = false;
}
