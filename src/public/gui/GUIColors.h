/*
*	OXWARE developed by oxiKKK
*	Copyright (c) 2023
*
*	This program is licensed under the MIT license. By downloading, copying,
*	installing or using this software you agree to this license.
*
*	License Agreement
*
*	Permission is hereby granted, free of charge, to any person obtaining a
*	copy of this software and associated documentation files (the "Software"),
*	to deal in the Software without restriction, including without limitation
*	the rights to use, copy, modify, merge, publish, distribute, sublicense,
*	and/or sell copies of the Software, and to permit persons to whom the
*	Software is furnished to do so, subject to the following conditions:
*
*	The above copyright notice and this permission notice shall be included
*	in all copies or substantial portions of the Software.
*
*	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
*	OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
*	THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
*	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
*	IN THE SOFTWARE.
*/

#ifndef GUICOLORS_H
#define GUICOLORS_H
#pragma once


enum EGUIColorId
{
	GUICLR_Invalid,


	GUICLR_TextLight,
	GUICLR_TextRegular,
	GUICLR_TextDark,
	GUICLR_TextBlack,
	GUICLR_TextWhite,


	GUICLR_FrameBg,
	GUICLR_FrameBgHovered,
	GUICLR_FrameBgActive,
	GUICLR_FrameBgSelected,
	GUICLR_FrameActive,


	GUICLR_WindowBackground,
	GUICLR_ChildBackground,


	GUICLR_Button,


	GUICLR_URLHovered,


	GUICLR_ScrollbarGrab,


	GUICLR_CheckBoxBackground,
	GUICLR_CheckBoxCheckMark,


	GUICLR_SliderHovered,


	GUICLR_InputTextBg,


	GUICLR_Separator,


	GUICLR_Tab,
	GUICLR_TabHovered,
	GUICLR_TabActive,


	GUICLR_ResizeGrip,
	GUICLR_ResizeGripHovered,
	GUICLR_ResizeGripActive,


	GUICLR_TableHeaderBg,


	GUICLR_ListBoxBackground,
	GUICLR_ListBoxBorder,
	GUICLR_ListBoxArrowBackground,

	GUICLR_MAX
};

#endif