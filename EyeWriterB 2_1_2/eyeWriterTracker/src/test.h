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

#define TEST_H_INCLUDED //prevent from compiling

#ifndef TEST_H_INCLUDED
#define TEST_H_INCLUDED

#include "ofMain.h"
#include "clVideoGrabber.h"
#include "ofxClEye.h"
#include "clEye.h"
#include "ofxOpenCv.h"
class test : public ofBaseApp
{
public:
    test(){}
    void setup()
    {
        cl.setup(640, 480, false);
        ci.allocate(640, 480);
    }
    void update()
    {
        cl.update();
        ci = cl.colorImg;
    }
    void draw()
    {
        ci.draw(0, 0);
    }
    clVideoGrabber cl;
    ofxCvGrayscaleImage gi;
    ofxCvColorImage ci;
};


#endif // TEST_H_INCLUDED
