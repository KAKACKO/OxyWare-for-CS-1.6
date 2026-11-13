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

#ifndef IGUIFONTMANAGER_H
#define IGUIFONTMANAGER_H
#pragma once

#include <array>

enum EFontDecoration
{
	FDC_Regular,
	FDC_Bold,
	FDC_Light,

	NUM_FONT_DECORATIONS
};

static const char* g_font_decors[NUM_FONT_DECORATIONS] =
{
	"regular",
	"bold",
	"light",
};

enum EFontId
{
	FID_SegoeUI,
	FID_ProggyClean,


	NUM_FONTS,
};

static const char* g_font_filenames[NUM_FONTS] =
{
	"segoeui",
	"proggyclean",
};


inline static constexpr int k_font_size_base = 10;
enum EFontSize
{
	FSZ_10px,
	FSZ_13px,
	FSZ_14px,
	FSZ_16px,
	FSZ_18px,
	FSZ_21px,
	FSZ_24px,
	FSZ_27px,
	FSZ_30px,
	FSZ_33px,
	FSZ_45px,

	FSZ_COUNT
};

inline static constexpr std::array<float, FSZ_COUNT> s_EFontSizeToFloat =
{
	/*FSZ_10px*/ 10.0f,
	/*FSZ_13px*/ 13.0f,
	/*FSZ_14px*/ 14.0f,
	/*FSZ_16px*/ 16.0f,
	/*FSZ_20px*/ 20.0f,
	/*FSZ_21px*/ 21.0f,
	/*FSZ_24px*/ 24.0f,
	/*FSZ_27px*/ 27.0f,
	/*FSZ_30px*/ 30.0f,
	/*FSZ_33px*/ 33.0f,
	/*FSZ_45px*/ 45.0f,
};


enum FreeTypeBuilderFlags
{
	FreeTypeBuilderFlags_None = 0,
	FreeTypeBuilderFlags_NoHinting = 1 << 0,
	FreeTypeBuilderFlags_NoAutoHint = 1 << 1,
	FreeTypeBuilderFlags_ForceAutoHint = 1 << 2,
	FreeTypeBuilderFlags_LightHinting = 1 << 3,
	FreeTypeBuilderFlags_MonoHinting = 1 << 4,
	FreeTypeBuilderFlags_Bold = 1 << 5,
	FreeTypeBuilderFlags_Oblique = 1 << 6,
	FreeTypeBuilderFlags_Monochrome = 1 << 7,
	FreeTypeBuilderFlags_LoadColor = 1 << 8,
	FreeTypeBuilderFlags_Bitmap = 1 << 9
};
DEFINE_ENUM_BITWISE_OPERATORS(FreeTypeBuilderFlags);

class IGUIFontManager
{
public:
	virtual void initialize() = 0;


	virtual bool pre_newframe() = 0;

	virtual void push_default_font() = 0;
	virtual void pop_default_font() = 0;

	virtual ImFont* get_font(EFontId id, EFontSize size, EFontDecoration decor = FDC_Regular) = 0;
	virtual ImFont* get_default_font() = 0;

	virtual Vector2D calc_font_text_size(ImFont* font, const char* text) = 0;

	virtual void rebuild_fonts() = 0;

	virtual void add_freetype_builder_flags(FreeTypeBuilderFlags f, bool set) = 0;
	virtual FreeTypeBuilderFlags get_freetype_builder_flags() = 0;

	virtual void render_debug_ui() = 0;
};

extern IGUIFontManager* g_gui_fontmgr_i;

#define IGUIFONTMANAGER_INTERFACEID "IGUIFontManager"

#endif