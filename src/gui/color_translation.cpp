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

#include "precompiled.h"

gui_color_translation_t g_gui_color_translation[GUICLR_MAX] =
{
	{ GUICLR_Invalid,						"invalid" },


	{ GUICLR_TextLight,						"Text light" },
	{ GUICLR_TextRegular,					"Text regular" },
	{ GUICLR_TextDark,						"Text dark" },
	{ GUICLR_TextBlack,						"Text black" },
	{ GUICLR_TextWhite,						"Text white" },


	{ GUICLR_FrameBg,						"Frame bg" },
	{ GUICLR_FrameBgHovered,				"Frame bg hovered" },
	{ GUICLR_FrameBgActive,					"Frame bg active" },
	{ GUICLR_FrameBgSelected,				"Frame bg selected" },
	{ GUICLR_FrameActive,					"Frame active" },


	{ GUICLR_WindowBackground,				"Window bg" },
	{ GUICLR_ChildBackground,				"Child bg" },


	{ GUICLR_Button,						"Button" },


	{ GUICLR_URLHovered,					"URL hovered" },


	{ GUICLR_ScrollbarGrab,					"Scrollbar grab" },


	{ GUICLR_CheckBoxBackground,			"Checkbox bg" },
	{ GUICLR_CheckBoxCheckMark,				"Checkbox check mark" },


	{ GUICLR_SliderHovered,					"Slider hovered" },


	{ GUICLR_InputTextBg,					"Intput text bg" },


	{ GUICLR_Separator,						"Separator" },


	{ GUICLR_Tab,							"Tab" },
	{ GUICLR_TabHovered,					"Tab hovered" },
	{ GUICLR_TabActive,						"Tab active" },


	{ GUICLR_ResizeGrip,					"Resize grip" },
	{ GUICLR_ResizeGripHovered,				"Resize grip hovered" },
	{ GUICLR_ResizeGripActive,				"Resize grip active" },


	{ GUICLR_TableHeaderBg,					"Table header bg" },


	{ GUICLR_ListBoxBackground,				"Listbox bg" },
	{ GUICLR_ListBoxBorder,					"Listbox border" },
	{ GUICLR_ListBoxArrowBackground,		"Listbox arrow bg" },
};
