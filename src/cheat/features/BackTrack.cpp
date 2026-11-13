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


#define LAG_COMPENSATION_TELEPORTED_DISTANCE_SQR ( 64.0f * 64.0f )
#define LAG_COMPENSATION_EPS_SQR ( 0.1f * 0.1f )

#define LAG_COMPENSATION_ERROR_EPS_SQR ( 4.0f * 4.0f )

#define LAG_COMPENSATION_DATA_TIME	1.0f

VarBoolean backtrack_enable("backtrack_enable", "Enables backtracking on players", false);
VarInteger backtrack_team("backtrack_team", "Which team to backtrack on", 0, 0, 4);
VarBoolean backtrack_self("backtrack_self", "Applies backtracking on your player", false);

void CBacktrack::update()
{
	if (!backtrack_enable.get_value())
	{
		return;
	}

	auto cmd = CClientMovementPacket::the().get_cmd();

	if (!cmd)
	{
		return;
	}

	for (auto& [index, player] : CEntityMgr::the().m_known_players)
	{
		if (!player.is_valid() || (player.is_local_player() && !backtrack_self.get_value()) || player.is_local_spectating() || !player.is_alive() || player.is_out_of_update_for(1.0f))
		{
			continue;
		}

		auto cl_ent = player.cl_entity();

		Vector simorg, simang;
		if (!backtrack_entity(cl_ent, cmd->lerp_msec, simorg, simang))
		{
			continue;
		}

		int selected_team = backtrack_team.get_value();

		auto local = CLocalState::the().local_player();

		int player_team = player.get_team();
		int local_team = local ? local->get_team() : hl::UNASSIGNED;

		if (

			(selected_team == 0 && player_team == hl::TERRORIST) ||

			(selected_team == 1 && player_team == hl::CT) ||

			(selected_team == 2 && player_team == local_team) ||

			(selected_team == 3 && player_team != local_team) ||

			(selected_team == 4 && (player_team == hl::TERRORIST || player_team == hl::CT)))
		{

			static hl::cl_entity_t copy;

			copy = *cl_ent;


			construct_backtrack_entity(copy, simorg, simang);

			CFakePlayerRenderer::the().render_player(copy, ENT_ID_BACKTRACK);
		}
	}
}


bool CBacktrack::find_interpolation_updates(hl::cl_entity_t* ent, float targettime,
											hl::position_history_t** ph0, hl::position_history_t** ph1)
{
	int		i, i0, i1, imod;
	float	at;
	bool	extrapolate;

	imod = ent->current_position;
	i0 = (imod - 0) & HISTORY_MASK;
	i1 = (imod - 1) & HISTORY_MASK;

	extrapolate = true;


	if (targettime <= ent->ph[i0].animtime)
	{
		for (i = 1; i < HISTORY_MAX - 1; i++)
		{

			at = ent->ph[(imod - i) & HISTORY_MASK].animtime;
			if (at == 0.0f)
				break;


			if (targettime > at)
			{
				i0 = ((imod - i) + 1) & HISTORY_MASK;
				i1 = ((imod - i) + 0) & HISTORY_MASK;
				extrapolate = false;
				break;
			}
		}
	}


	if (ph0)
		*ph0 = &ent->ph[i0];
	if (ph1)
		*ph1 = &ent->ph[i1];

	return extrapolate;
}


float CBacktrack::bind_interpolation_fraction(float frac)
{
	return std::clamp(frac, 0.0f, 1.2f);
}


float CBacktrack::calc_update_interval_ms(float updaterate)
{

	return 1.0f / std::max(updaterate, 10.0f);
}


float CBacktrack::get_raw_client_latency(hl::frame_t * frame)
{

	float lat = 0.0f;
	if (fake_latency.get_value())
	{
		lat = CNetchanSequenceHistory::the().get_desired_fake_latency();
	}
	else
	{


		lat = frame->latency;
	}

	return lat;
}

float CBacktrack::calc_true_latency(float updaterate, float max_unlag, float ft, float client_latency, float update_interval_ms)
{

	float l = std::min(client_latency, 1.5f);


	l -= update_interval_ms;


	l -= ft/* * 0.5*/;


	float correction = std::min(LAG_COMPENSATION_DATA_TIME, l);


	if (max_unlag)
	{

		if (max_unlag < 0.0f)
			max_unlag = 0.0f;


		correction = std::min(correction, max_unlag);
	}

	return correction;
}

float CBacktrack::calc_interpolation_amount(float ex_interp, float update_interval_ms, float lerp_msec)
{

	return std::min((lerp_msec == -1) ? ex_interp : (lerp_msec / 1000.0f), 0.1f);
}

bool CBacktrack::backtrack_entity(hl::cl_entity_t* ent, float lerp_msec, Vector& simorg, Vector& simang)
{
	auto ex_interp = CGoldSrcCommandMgr::the().get_cvar("ex_interp");
	auto sv_unlagpush = CGoldSrcCommandMgr::the().get_cvar("sv_unlagpush");
	auto cl_updaterate = CGoldSrcCommandMgr::the().get_cvar("cl_updaterate");
	auto sv_maxunlag = CGoldSrcCommandMgr::the().get_cvar("sv_maxunlag");

	if (!ex_interp || !sv_unlagpush || !cl_updaterate || !sv_maxunlag)
	{
		CConsole::the().derror("Something went horribly wrong. Couldn't get ex_interp, sv_unlagpush, etc.");
		return false;
	}


	float ft = CLocalState::the().get_engine_frametime();
	auto frame = CLocalState::the().get_current_frame();
	float realtime = CMemoryHookMgr::the().cl().get()->time;


	float update_interval_ms = calc_update_interval_ms(cl_updaterate->value);


	float raw_client_latency = get_raw_client_latency(frame);


	float true_client_latency = calc_true_latency(cl_updaterate->value, sv_maxunlag->value, ft, raw_client_latency, update_interval_ms);


	float target_time = realtime - true_client_latency;


	float client_interp_amount = calc_interpolation_amount(ex_interp->value, update_interval_ms, lerp_msec);
	target_time -= client_interp_amount;


	target_time += sv_unlagpush->value;


	target_time = std::min(realtime, target_time);


	hl::position_history_t* newer, *older;
	find_interpolation_updates(ent, target_time, &newer, &older);

	if (!newer || !older)
	{
		return false;
	}

	if (newer->animtime != 0)
	{
		Vector pos, ang;
		Vector delta;
		float frac;

		delta = newer->origin - older->origin;

		if (newer->animtime != older->animtime)
		{
			frac = (target_time - older->animtime) / (newer->animtime - older->animtime);
		}
		else
		{
			frac = 1.0f;
		}

		frac = bind_interpolation_fraction(frac);


		CMath::the().vector_ma(older->origin, frac, delta, pos);
		CMath::the().vector_ma(older->angles, frac, delta, ang);

		simorg = pos;
		simang = ang;
	}
	else
	{
		simorg = older->origin;
		simang = older->angles;
	}

	return true;
}

void CBacktrack::construct_backtrack_entity(hl::cl_entity_t& in, const Vector& interpolated_origin, const Vector& interpolated_angles)
{
	in.origin = interpolated_origin;
	in.angles = interpolated_angles;


	in.curstate.weaponmodel = 0;


	in.curstate.renderamt = 0;
	in.curstate.rendercolor = {};
	in.curstate.renderfx = hl::kRenderFxNone;
	in.curstate.rendermode = hl::kRenderNormal;
}
