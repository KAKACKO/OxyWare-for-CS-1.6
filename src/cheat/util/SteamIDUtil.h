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

#ifndef STEAMIDUTIL_H
#define STEAMIDUTIL_H
#pragma once


struct Steam2ID_t
{
	uint16_t m_SteamInstanceID;

	union
	{
		uint64_t	As64bits;
		struct
		{
			unsigned int	Low32bits;
			unsigned int	High32bits;
		} Split;
	} m_SteamLocalUserID;
};


union Steam3ID_t
{
	Steam3ID_t() :
		m_unAll64Bits(0ULL)
	{
	}

	Steam3ID_t(uint64_t id) :
		m_unAll64Bits(id)
	{
	}

	struct
	{
		uint32_t		m_unAccountID : 32;
		uint32_t		m_unAccountInstance : 20;
		uint32_t		m_EAccountType : 4;
		hl::EUniverse	m_EUniverse : 8;
	};

	struct
	{
		uint32_t		Low32bits : 32;
		uint32_t		High32bits : 32;
	};

	uint64_t m_unAll64Bits;
};

class CSteamIDUtil
{
public:
	DECL_BASIC_CLASS(CSteamIDUtil);

public:

	std::string render_steam3_id_variant_1(Steam3ID_t steam3_id);


	std::string render_steam3_id_variant_2(Steam3ID_t steam3_id);
	std::string render_steam2_id_variant_2(Steam3ID_t steam3_id);


	std::string render_steam_account(Steam3ID_t steam3_id);


	std::string steam_id_to_url(Steam3ID_t steam3_id);


	std::string sub_path_from_account_type(Steam3ID_t steam3_id);


	Steam3ID_t get_steam_id_account_identifier(Steam3ID_t steam3_id);


	Steam3ID_t convert_to_steam_community_id(Steam3ID_t steam3_id);


	Steam2ID_t to_steam2(Steam3ID_t steam3_id);


	bool is_steam_user(Steam3ID_t steam3_id);


	bool is_bot(Steam3ID_t steam3_id);


	bool is_valid(Steam3ID_t steam3_id);


	bool is_using_sid_changer(Steam3ID_t steam3_id);
};

#endif