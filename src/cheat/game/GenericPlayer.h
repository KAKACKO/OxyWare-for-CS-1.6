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

#ifndef GENERICPLAYER_H
#define GENERICPLAYER_H
#pragma once

class CGenericPlayer : public CGenericEntity
{
public:
	virtual void update(hl::cl_entity_t* ent);


	virtual void update_player_info(int index, hl::player_info_t* pinfo);

	virtual bool is_valid() const { return CGenericEntity::is_valid() && m_extra_playerinfo && m_playerinfo; }


	hl::extra_player_info_t* get_extrainfo() const { return m_extra_playerinfo; }
	hl::player_info_t* get_playerinfo() const { return m_playerinfo; }

	virtual bool is_alive() const;

	CColor get_color_based_on_team() const;

	hl::TeamName get_team() const { return m_extra_playerinfo ? (hl::TeamName)m_extra_playerinfo->teamnumber : hl::UNASSIGNED; }
	std::string get_team_as_string() const;

	bool is_local_player() const;
	bool is_local_spectating() const;

	bool is_standing() const { return m_ent->curstate.usehull == HULL_STANDING;}
	bool is_ducking() const { return m_ent->curstate.usehull == HULL_DUCKING; }


	int get_index_safe() const { return m_playerinfo_index; }


	Vector get_eye_pos() const { return m_ent->origin + CMemoryHookMgr::the().cl().get()->viewheight; }

	Vector get_default_bounding_box_min() const { return Vector(-16.0f, -16.0f, -18.0f); }
	Vector get_default_bounding_box_max() const { return Vector(16.0f, 16.0f, 18.0f); }


	uint64_t get_steam_id() const;


	bool is_bot() const;
	bool is_steam_user() const;


	bool is_local_friend_with() const;


	std::string get_steam_persona_name() const;


	hl::EPersonaState get_persona_state() const;

private:
	hl::extra_player_info_t* m_extra_playerinfo = nullptr;
	hl::player_info_t* m_playerinfo = nullptr;


	int m_playerinfo_index = 0;
};

#endif