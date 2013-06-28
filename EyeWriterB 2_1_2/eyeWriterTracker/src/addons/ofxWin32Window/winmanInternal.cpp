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

#include "winmanInternal.h"

winmanStateStruct winmanState = {0}; //must be global so we can call it in the LRESULT CALLBACK Window Procedure


void	WINMAN_APIENTRY		winmanMouseFunc(mouseFunc callback)
{
	winmanState.callbacks.winmanMouseFuncCB = callback;
}

void	WINMAN_APIENTRY		winmanMotionFunc(motionFunc callback)
{
	winmanState.callbacks.winmanMotionFuncCB = callback;
}

void	WINMAN_APIENTRY		winmanPassiveMotionFunc(passiveMotionFunc callback)
{
	winmanState.callbacks.winmanPassiveMotionFuncCB = callback;
}

void	WINMAN_APIENTRY		winmanIdleFunc(idleFunc callback)
{
	winmanState.callbacks.winmanIdleFuncCB = callback;

}

void	WINMAN_APIENTRY		winmanDisplayFunc(displayFunc callback)
{
	winmanState.callbacks.winmanDisplayFuncCB = callback;
}




void	WINMAN_APIENTRY		winmanKeyboardFunc(keyboardFunc callback)
{
	winmanState.callbacks.winmanKeyboardFuncCB = callback;
}

void	WINMAN_APIENTRY		winmanKeyboardUpFunc(keyboardUpFunc callback)
{
	winmanState.callbacks.winmanKeyboardUpFuncCB = callback;
}

void	WINMAN_APIENTRY		winmanSpecialFunc(specialFunc callback)
{
	winmanState.callbacks.winmanSpecialFuncCB = callback;
}

void	WINMAN_APIENTRY		winmanSpecialUpFunc(specialUpFunc callback)
{
	winmanState.callbacks.winmanSpecialUpFuncCB = callback;
}




void	WINMAN_APIENTRY		winmanReshapeFunc(reshapeFunc callback)
{
	winmanState.callbacks.winmanReshapeFuncCB = callback;
}
