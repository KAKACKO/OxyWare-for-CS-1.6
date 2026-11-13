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

#ifndef PLAYERLIST_H
#define PLAYERLIST_H
#pragma once

struct cached_player_info_t
{
	cached_player_info_t()
	{}

	BaseTexture m_avatar_picture;

	Steam3ID_t m_steam3_id;
	Steam2ID_t m_steam2_id;

	int m_player_id, m_user_id;
	std::string m_team;

	bool m_is_steam_user;


	bool m_local_is_friend_with;

	std::string m_steam_name;
	hl::EPersonaState m_persona_state;
};

class CUIPlayerList
{
public:
	DECL_BASIC_CLASS(CUIPlayerList);

public:
	void render_ui();

private:
	void initialize();

	void render_players();
	void render_selected_player_info();

	void render_player_info(const cached_player_info_t& cached_player);
	void render_player_avatar(const cached_player_info_t& cached_player, float avatar_size);

	void add_steam_community_profile_link(const cached_player_info_t& cached_player);
	void add_additional_steam_info_about_player(const cached_player_info_t& cached_player);

	void manage_timers();


	void add_gui_condional_colored_text(const std::string& text, bool condition);
	
	int m_selected_plr_idx;


	std::array<cached_player_info_t, MAX_CLIENTS> m_cached_players;
	void create_cached_player_info(int id, const CGenericPlayer& player);
	void update_avatars(int id, cached_player_info_t& cached_player);
	bool m_update_player_cache = false;
	uint32_t m_last_cache_update_time = 0;


	bool m_precache_avatar_picture = false;
	uint32_t m_last_avatar_precache_time = 0;

	BaseTexture m_bot_pfp, m_nonsteam_pfp, m_invalid_pfp, m_avatar_not_found;

	std::string get_avatar_texture_playerid(int id);


	bool m_is_local_steam = false;


	bool m_filter_allow_steam;
	bool m_filter_allow_non_steam;
	bool m_filter_allow_bots;

	bool apply_filter(const CGenericPlayer& player);
};

#endif